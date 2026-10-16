#include "jaringan.h"
#include "konfigurasi.h"
#include <WiFi.h>
#include <PubSubClient.h>

static WiFiClient klienTcp;
static PubSubClient mqtt(klienTcp);
static QueueHandle_t kotakData = nullptr;    // antrean panjang 1 = "kotak surat" loop() → tugas
static QueueHandle_t kotakInfo = nullptr;    // kotak surat tugas → loop()
static TaskHandle_t tugas = nullptr;
static InfoJaringan info = {KeadaanJaringan::WIFI_MULAI, 0, "-"};
static char idKlien[40];

static const char TOPIK_SUHU[]       = TOPIK_DASAR "suhu";
static const char TOPIK_KELEMBAPAN[] = TOPIK_DASAR "kelembapan";
static const char TOPIK_AMBANG[]     = TOPIK_DASAR "ambang";
static const char TOPIK_ALARM[]      = TOPIK_DASAR "alarm";
static const char TOPIK_STATUS[]     = TOPIK_DASAR "status";   // online/offline (LWT)

// Ganti keadaan FSM dan kabarkan ke loop() lewat kotak surat
static void ubahKeadaan(KeadaanJaringan baru) {
  info.keadaan = baru;
  xQueueOverwrite(kotakInfo, &info);
}

static void wifiPutus() {
  Serial.println("[JARINGAN] Wi-Fi terputus. Menyambung ulang...");
  strcpy(info.ip, "-");
  WiFi.disconnect();
  ubahKeadaan(KeadaanJaringan::WIFI_MULAI);
}

static void kirimSemua(const PaketData &d) {
  char teks[16];
  bool berhasil = true;
  if (d.sensorValid) {                       // jangan kirim angka lama bila sensor gagal
    snprintf(teks, sizeof(teks), "%.1f", d.suhu);
    berhasil &= mqtt.publish(TOPIK_SUHU, teks);
    snprintf(teks, sizeof(teks), "%.1f", d.kelembapan);
    berhasil &= mqtt.publish(TOPIK_KELEMBAPAN, teks);
  }
  snprintf(teks, sizeof(teks), "%.1f", d.ambang);
  berhasil &= mqtt.publish(TOPIK_AMBANG, teks);
  berhasil &= mqtt.publish(TOPIK_ALARM, alarmNamaStatus(d.status));
  if (!berhasil) {
    Serial.println("[MQTT] Sebagian pesan gagal dikirim.");
    return;
  }
  info.jumlahKirim++;
  xQueueOverwrite(kotakInfo, &info);
  if (d.sensorValid) {
    Serial.printf("[MQTT] Kirim #%lu: %.1f °C, %.1f %%, ambang %.1f °C, %s\n",
                  (unsigned long)info.jumlahKirim, d.suhu, d.kelembapan, d.ambang,
                  alarmNamaStatus(d.status));
  } else {
    Serial.printf("[MQTT] Kirim #%lu: ambang %.1f °C, %s\n",
                  (unsigned long)info.jumlahKirim, d.ambang, alarmNamaStatus(d.status));
  }
}

// Tugas ini boleh "tertahan" beberapa detik saat menyambung (connect() memang
// memblokir). Karena berjalan terpisah dari loop(), sensor dan alarm tetap jalan.
static void tugasJaringan(void *parameter) {
  PaketData data;
  bool adaData = false;
  bool kirimSegera = false;
  StatusAlarm statusTerkirim = StatusAlarm::MENUNGGU;
  unsigned long waktuMulaiWifi = 0, waktuCobaMqtt = 0, waktuKirim = 0;
  unsigned long jedaCoba = JEDA_COBA_AWAL_MS;

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(false);              // penyambungan ulang diatur FSM di bawah
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setSocketTimeout(5);                  // tunggu jawaban broker maks. 5 s (bawaan 15 s)

  for (;;) {
    if (xQueueReceive(kotakData, &data, 0) == pdTRUE) adaData = true;   // ambil data terbaru

    switch (info.keadaan) {
      case KeadaanJaringan::WIFI_MULAI:
        Serial.printf("[JARINGAN] Menyambung ke Wi-Fi \"%s\" (%s)...\n", WIFI_SSID, SUMBER_WIFI);
        WiFi.begin(WIFI_SSID, WIFI_SANDI, WIFI_KANAL);
        waktuMulaiWifi = millis();
        ubahKeadaan(KeadaanJaringan::WIFI_MENUNGGU);
        break;

      case KeadaanJaringan::WIFI_MENUNGGU:
        if (WiFi.status() == WL_CONNECTED) {
          WiFi.localIP().toString().toCharArray(info.ip, sizeof(info.ip));
          Serial.printf("[JARINGAN] Wi-Fi tersambung. IP: %s\n", info.ip);
          jedaCoba = JEDA_COBA_AWAL_MS;
          waktuCobaMqtt = millis() - jedaCoba;          // langsung coba MQTT
          ubahKeadaan(KeadaanJaringan::MQTT_MENYAMBUNG);
        } else if (millis() - waktuMulaiWifi > BATAS_TUNGGU_WIFI_MS) {
          Serial.println("[JARINGAN] Wi-Fi belum tersambung. Mencoba lagi...");
          WiFi.disconnect();
          ubahKeadaan(KeadaanJaringan::WIFI_MULAI);
        }
        break;

      case KeadaanJaringan::MQTT_MENYAMBUNG:
        if (WiFi.status() != WL_CONNECTED) { wifiPutus(); break; }
        if (millis() - waktuCobaMqtt >= jedaCoba) {
          Serial.printf("[JARINGAN] Menyambung ke broker %s:%d...\n", MQTT_BROKER, MQTT_PORT);
          // Last Will: bila kita hilang tanpa pamit, broker menerbitkan "offline"
          if (mqtt.connect(idKlien, TOPIK_STATUS, 1, true, "offline")) {
            mqtt.publish(TOPIK_STATUS, "online", true);  // retain: pemantau baru langsung tahu
            Serial.printf("[JARINGAN] MQTT tersambung sebagai %s\n", idKlien);
            Serial.printf("[JARINGAN] Pantau topik %s#\n", TOPIK_DASAR);
            jedaCoba = JEDA_COBA_AWAL_MS;
            kirimSegera = true;
            ubahKeadaan(KeadaanJaringan::TERSAMBUNG);
          } else {
            jedaCoba = min(jedaCoba * 2, JEDA_COBA_MAKS_MS);   // gagal lagi → tunggu lebih lama
            waktuCobaMqtt = millis();
            Serial.printf("[JARINGAN] Gagal (rc=%d). Coba lagi dalam %lu s.\n",
                          mqtt.state(), jedaCoba / 1000);
          }
        }
        break;

      case KeadaanJaringan::TERSAMBUNG:
        if (WiFi.status() != WL_CONNECTED) { wifiPutus(); break; }
        if (!mqtt.connected()) {
          Serial.println("[JARINGAN] MQTT terputus. Menyambung ulang...");
          waktuCobaMqtt = millis() - jedaCoba;
          ubahKeadaan(KeadaanJaringan::MQTT_MENYAMBUNG);
          break;
        }
        mqtt.loop();                             // jaga sambungan tetap hidup (PINGREQ)
        if (adaData) {
          bool waktunya = millis() - waktuKirim >= INTERVAL_KIRIM_MS;
          bool statusBerubah = (data.status != statusTerkirim);
          if (waktunya || statusBerubah || kirimSegera) {   // status baru dikirim SEGERA
            kirimSemua(data);
            statusTerkirim = data.status;
            waktuKirim = millis();
            kirimSegera = false;
          }
        }
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(50));               // beri giliran tugas lain
  }
}

void jaringanMulai() {
  kotakData = xQueueCreate(1, sizeof(PaketData));
  kotakInfo = xQueueCreate(1, sizeof(InfoJaringan));
  xQueueOverwrite(kotakInfo, &info);
  // ID klien harus unik di broker: pakai NIM + angka acak
  snprintf(idKlien, sizeof(idKlien), "st%sp15%04lx", NIM, (unsigned long)random(0x10000));
  // Inti 0 (tempat tumpukan Wi-Fi bekerja); loop() tetap di inti 1. Stack 8.192 byte.
  xTaskCreatePinnedToCore(tugasJaringan, "Jaringan", 8192, nullptr, 1, &tugas, 0);
}

void jaringanKirim(const PaketData &paket) {
  if (kotakData) xQueueOverwrite(kotakData, &paket);   // timpa isi lama, tidak pernah menunggu
}

InfoJaringan jaringanInfo() {
  InfoJaringan salinan = {KeadaanJaringan::WIFI_MULAI, 0, "-"};
  if (kotakInfo) xQueuePeek(kotakInfo, &salinan, 0);   // intip tanpa mengambil
  return salinan;
}

const char *jaringanNamaKeadaan(KeadaanJaringan k) {
  switch (k) {
    case KeadaanJaringan::WIFI_MULAI:      return "WIFI_MULAI";
    case KeadaanJaringan::WIFI_MENUNGGU:   return "WIFI_MENUNGGU";
    case KeadaanJaringan::MQTT_MENYAMBUNG: return "MQTT_MENYAMBUNG";
    case KeadaanJaringan::TERSAMBUNG:      return "TERSAMBUNG";
  }
  return "?";
}

UBaseType_t jaringanSisaStack() {
  return tugas ? uxTaskGetStackHighWaterMark(tugas) : 0;
}
