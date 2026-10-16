/*
 * =====================================================================
 *  Pertemuan 13 · Praktikum 2 — Mengirim Data Sensor lewat MQTT
 * =====================================================================
 *  Tujuan   : Membaca potensiometer setiap 5 detik, menyusunnya menjadi
 *             JSON sederhana dengan snprintf, lalu menerbitkannya (publish)
 *             ke broker MQTT publik broker.emqx.io.
 *  Rangkaian: Potensiometer — SIG (kaki tengah) ke GPIO 34, VCC ke 3V3,
 *             GND ke GND. GPIO 34 ada di ADC1, aman dipakai bersama Wi-Fi.
 *  Topik    : kuliah-st/<NIM>/sensor   (ganti konstanta NIM di bawah!)
 *  Cara menjalankan:
 *    - Wokwi web  : salin berkas ini ke sketch.ino + diagram.json, tambahkan
 *                   library "PubSubClient" (Library Manager), lalu klik ▶.
 *    - Board asli : buat include/rahasia.h dari rahasia.example.h, lalu
 *                   Build → Upload → Serial Monitor.
 *    - Pantau di MQTTX Web: berlangganan topik kuliah-st/<NIM>/#.
 *  Hasil yang diharapkan:
 *    Tersambung ke Wi-Fi dan broker, lalu setiap 5 detik muncul
 *    [KIRIM] {"adc":...,"persen":...,"nomor":...} -> OK, dan pesan yang
 *    sama tampil di MQTTX. Nilai ADC mengikuti putaran kenop.
 * =====================================================================
 */
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ---------------------------------------------------------------------
// Pengaturan Wi-Fi
//  - Board asli: salin include/rahasia.example.h menjadi include/rahasia.h,
//    lalu isi SSID & sandi hotspot 2,4 GHz-mu (rahasia.h tidak di-commit).
//  - Wokwi: rahasia.h tidak ada, jadi otomatis memakai Wokwi-GUEST.
// ---------------------------------------------------------------------
#if __has_include("rahasia.h")
#include "rahasia.h"
#define SUMBER_WIFI "include/rahasia.h"
#else
#define WIFI_SSID "Wokwi-GUEST"  // Wi-Fi virtual Wokwi: terbuka, tanpa sandi
#define WIFI_SANDI ""
#define WIFI_KANAL 6             // Wokwi-GUEST ada di kanal 6 (sambung lebih cepat)
#define SUMBER_WIFI "bawaan Wokwi (rahasia.h tidak ditemukan)"
#endif
#ifndef WIFI_KANAL
#define WIFI_KANAL 0             // 0 = cari jaringan di semua kanal
#endif

// ← GANTI dengan NIM-mu (angka/huruf saja) agar topikmu tidak bertabrakan
const char *NIM = "GANTI_NIM";

const char *MQTT_BROKER = "broker.emqx.io";  // broker publik: jangan untuk data pribadi!
const uint16_t MQTT_PORT = 1883;             // MQTT lewat TCP biasa (tanpa enkripsi)

const int PIN_POT = 34;                       // potensiometer di ADC1
const unsigned long INTERVAL_KIRIM_MS = 5000;  // kirim tiap 5 detik

WiFiClient klienTcp;          // "pipa" TCP ke broker
PubSubClient mqtt(klienTcp);  // klien MQTT yang berjalan di atas pipa itu

char topikSensor[64];
char idKlien[32];
unsigned long waktuKirimTerakhir = 0;
unsigned long nomorPesan = 0;

void sambungWiFi();
void sambungMqtt();
void kirimData();

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== Praktikum 2: Kirim Data Sensor lewat MQTT ===");
  if (strcmp(NIM, "GANTI_NIM") == 0) {
    Serial.println("[PERINGATAN] Konstanta NIM belum diganti!");
  }

  sambungWiFi();

  // Topik dan ID klien disusun dari NIM. Akhiran acak membuat ID tetap unik
  // walaupun kamu menjalankan dua simulasi sekaligus.
  snprintf(topikSensor, sizeof(topikSensor), "kuliah-st/%s/sensor", NIM);
  snprintf(idKlien, sizeof(idKlien), "st%sp2%04lx", NIM, (unsigned long)random(0x10000));
  Serial.printf("[INFO] Topik     : %s\n", topikSensor);
  Serial.printf("[INFO] ID klien  : %s\n", idKlien);

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
}

void loop() {
  if (!mqtt.connected()) {
    sambungMqtt();  // versi sederhana: menunggu sampai tersambung
  }
  mqtt.loop();      // WAJIB sering dipanggil: memproses paket & menjaga keep alive

  if (millis() - waktuKirimTerakhir >= INTERVAL_KIRIM_MS) {
    waktuKirimTerakhir = millis();
    kirimData();
  }
}

void sambungWiFi() {
  Serial.printf("[INFO] Pengaturan Wi-Fi: %s\n", SUMBER_WIFI);
  Serial.printf("[INFO] Menyambung ke Wi-Fi \"%s\"", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_SANDI, WIFI_KANAL);
  while (WiFi.status() != WL_CONNECTED) {  // versi sederhana: tunggu terus
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Tersambung!");
  Serial.print("[INFO] Alamat IP : ");
  Serial.println(WiFi.localIP());
}

void sambungMqtt() {
  while (!mqtt.connected()) {
    Serial.printf("[INFO] Menyambung ke broker %s:%d ... ", MQTT_BROKER, (int)MQTT_PORT);
    if (mqtt.connect(idKlien)) {
      Serial.println("berhasil!");
    } else {
      // state(): -2 = gagal membuka TCP (cek internet/port), -4 = waktu habis, dst.
      Serial.printf("gagal (state %d). Coba lagi 5 detik lagi.\n", mqtt.state());
      delay(5000);
    }
  }
}

void kirimData() {
  int nilaiAdc = analogRead(PIN_POT);          // 0–4095 (12 bit)
  float persen = nilaiAdc * 100.0f / 4095.0f;  // 0–100 %
  nomorPesan++;

  // Susun JSON dengan snprintf. Tanda \" = karakter kutip di dalam string C.
  char payload[80];
  int panjang = snprintf(payload, sizeof(payload), "{\"adc\":%d,\"persen\":%.1f,\"nomor\":%lu}",
                         nilaiAdc, persen, nomorPesan);
  if (panjang < 0 || panjang >= (int)sizeof(payload)) {
    Serial.println("[PERINGATAN] Payload terlalu panjang, tidak dikirim.");
    return;
  }

  bool terkirim = mqtt.publish(topikSensor, payload);  // QoS 0, tanpa retain
  Serial.printf("[KIRIM] %s -> %s\n", payload, terkirim ? "OK" : "GAGAL");
}
