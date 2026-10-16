/*
 * =====================================================================
 *  Pertemuan 13 · Praktikum 3 — Mengendalikan LED lewat MQTT
 * =====================================================================
 *  Tujuan   : Berlangganan (subscribe) topik perintah; payload "ON"/"OFF"
 *             menyalakan/memadamkan LED. Status LED diterbitkan kembali,
 *             dan LWT "offline" (retain) memberi tahu bila ESP32 terputus.
 *             Sambung ulang Wi-Fi & MQTT dibuat tanpa memblokir loop().
 *  Rangkaian: GPIO 23 → resistor 220 Ω → anoda LED; katoda LED → GND.
 *             LED biru bawaan (GPIO 2) = indikator: berkedip lambat bila
 *             tersambung ke broker, berkedip cepat bila belum.
 *  Topik    : kuliah-st/<NIM>/led         ← perintah ON/OFF (subscribe)
 *             kuliah-st/<NIM>/led/status  → keadaan LED (publish, retain)
 *             kuliah-st/<NIM>/status      → online/offline (retain + LWT)
 *  Cara menjalankan:
 *    - Wokwi web  : salin berkas ini ke sketch.ino + diagram.json, tambahkan
 *                   library "PubSubClient", lalu klik ▶.
 *    - Board asli : buat include/rahasia.h dari rahasia.example.h, lalu
 *                   Build → Upload → Serial Monitor.
 *    - Kirim ON/OFF dari MQTTX Web ke topik kuliah-st/<NIM>/led.
 *  Hasil yang diharapkan:
 *    Setelah tersambung, "online" terbit di topik status. Mengirim ON
 *    menyalakan LED merah, OFF memadamkannya, dan led/status ikut berubah.
 *    Bila simulasi dihentikan, broker menerbitkan "offline" (LWT).
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

// ← GANTI dengan NIM-mu (angka/huruf saja)
const char *NIM = "GANTI_NIM";

const char *MQTT_BROKER = "broker.emqx.io";  // broker publik: jangan untuk alat berbahaya!
const uint16_t MQTT_PORT = 1883;

const int PIN_LED = 23;        // LED yang dikendalikan lewat MQTT
const int PIN_INDIKATOR = 2;   // LED biru bawaan: indikator koneksi

const unsigned long JEDA_COBA_WIFI_MS = 10000;  // jeda antarpercobaan sambung Wi-Fi
const unsigned long JEDA_COBA_MQTT_MS = 5000;   // jeda antarpercobaan sambung broker

WiFiClient klienTcp;
PubSubClient mqtt(klienTcp);

char topikPerintah[64];   // kuliah-st/<NIM>/led
char topikStatusLed[64];  // kuliah-st/<NIM>/led/status
char topikStatus[64];     // kuliah-st/<NIM>/status
char idKlien[32] = "";

bool ledMenyala = false;
bool perluLaporLed = false;       // diisi callback, dikerjakan di loop()
bool wifiTersambungTadi = false;
unsigned long waktuCobaWiFi = 0;
unsigned long waktuCobaMqtt = 0;  // 0 = belum pernah mencoba
unsigned long waktuKedip = 0;
bool indikatorNyala = false;

void jagaWiFi();
void jagaMqtt();
void pesanMasuk(char *topik, byte *isi, unsigned int panjang);
void laporStatusLed();
void kedipIndikator();

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== Praktikum 3: Kendali LED lewat MQTT ===");
  if (strcmp(NIM, "GANTI_NIM") == 0) {
    Serial.println("[PERINGATAN] Konstanta NIM belum diganti!");
  }
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_INDIKATOR, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  snprintf(topikPerintah, sizeof(topikPerintah), "kuliah-st/%s/led", NIM);
  snprintf(topikStatusLed, sizeof(topikStatusLed), "kuliah-st/%s/led/status", NIM);
  snprintf(topikStatus, sizeof(topikStatus), "kuliah-st/%s/status", NIM);
  Serial.printf("[INFO] Topik perintah : %s\n", topikPerintah);
  Serial.printf("[INFO] Topik status   : %s\n", topikStatus);

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(pesanMasuk);  // fungsi yang dipanggil saat ada pesan masuk

  Serial.printf("[INFO] Pengaturan Wi-Fi: %s\n", SUMBER_WIFI);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_SANDI, WIFI_KANAL);  // tidak menunggu di sini
  waktuCobaWiFi = millis();
}

void loop() {
  jagaWiFi();        // cek & sambung ulang Wi-Fi bila perlu (tanpa while)
  jagaMqtt();        // cek & sambung ulang broker bila perlu (tanpa while)
  mqtt.loop();       // proses pesan masuk + keep alive

  if (perluLaporLed && mqtt.connected()) {
    perluLaporLed = false;
    laporStatusLed();
  }
  kedipIndikator();  // bukti loop() tidak macet: LED biru terus berkedip
}

void jagaWiFi() {
  bool tersambung = (WiFi.status() == WL_CONNECTED);
  if (tersambung && !wifiTersambungTadi) {
    Serial.print("[INFO] Wi-Fi tersambung, IP: ");
    Serial.println(WiFi.localIP());
  } else if (!tersambung && wifiTersambungTadi) {
    Serial.println("[PERINGATAN] Wi-Fi terputus!");
  }
  wifiTersambungTadi = tersambung;

  // Belum tersambung? Coba lagi, tetapi paling sering sekali per 10 detik.
  if (!tersambung && millis() - waktuCobaWiFi >= JEDA_COBA_WIFI_MS) {
    waktuCobaWiFi = millis();
    Serial.println("[INFO] Mencoba menyambung ulang Wi-Fi...");
    WiFi.reconnect();
  }
}

void jagaMqtt() {
  if (WiFi.status() != WL_CONNECTED || mqtt.connected()) return;
  // Beri jeda 5 detik antarpercobaan agar tidak membanjiri broker
  if (waktuCobaMqtt != 0 && millis() - waktuCobaMqtt < JEDA_COBA_MQTT_MS) return;
  waktuCobaMqtt = millis();

  if (idKlien[0] == '\0') {  // buat ID unik sekali saja
    snprintf(idKlien, sizeof(idKlien), "st%sp3%04lx", NIM, (unsigned long)random(0x10000));
  }
  Serial.printf("[INFO] Menyambung ke broker %s:%d sebagai %s ... ", MQTT_BROKER, (int)MQTT_PORT, idKlien);
  // connect(id, topikWasiat, qosWasiat, retainWasiat, pesanWasiat):
  // bila ESP32 putus tanpa pamit, broker menerbitkan "offline" atas namanya (LWT).
  if (mqtt.connect(idKlien, topikStatus, 1, true, "offline")) {
    Serial.println("berhasil!");
    mqtt.publish(topikStatus, "online", true);  // retain: pelanggan baru langsung tahu
    Serial.printf("[STATUS] %s = online (retain)\n", topikStatus);
    mqtt.subscribe(topikPerintah, 1);           // PubSubClient: subscribe QoS 0 atau 1
    Serial.printf("[INFO] Berlangganan %s (QoS 1)\n", topikPerintah);
    laporStatusLed();
  } else {
    Serial.printf("gagal (state %d), coba lagi 5 detik lagi\n", mqtt.state());
  }
}

// Dipanggil dari dalam mqtt.loop() setiap ada pesan di topik yang dilanggani.
// Payload TIDAK diakhiri '\0', jadi salin dulu ke larik sendiri.
void pesanMasuk(char *topik, byte *isi, unsigned int panjang) {
  char perintah[16];
  unsigned int n = (panjang < sizeof(perintah) - 1) ? panjang : sizeof(perintah) - 1;
  memcpy(perintah, isi, n);
  perintah[n] = '\0';
  Serial.printf("[PESAN] %s = %s\n", topik, perintah);

  if (strcasecmp(perintah, "ON") == 0) {
    ledMenyala = true;
  } else if (strcasecmp(perintah, "OFF") == 0) {
    ledMenyala = false;
  } else {
    Serial.println("[PERINGATAN] Perintah tidak dikenal. Kirim ON atau OFF.");
    return;
  }
  digitalWrite(PIN_LED, ledMenyala ? HIGH : LOW);
  Serial.printf("[INFO] LED %s\n", ledMenyala ? "menyala" : "padam");
  // Jangan publish di sini: PubSubClient memakai satu buffer untuk pesan
  // masuk dan keluar. Cukup beri tanda, lalu kirim dari loop().
  perluLaporLed = true;
}

void laporStatusLed() {
  const char *teks = ledMenyala ? "ON" : "OFF";
  mqtt.publish(topikStatusLed, teks, true);  // retain: dasbor baru langsung tahu
  Serial.printf("[STATUS] %s = %s (retain)\n", topikStatusLed, teks);
}

void kedipIndikator() {
  // Setengah periode: 1 detik bila tersambung ke broker, 0,15 detik bila belum
  unsigned long jeda = mqtt.connected() ? 1000 : 150;
  if (millis() - waktuKedip >= jeda) {
    waktuKedip = millis();
    indikatorNyala = !indikatorNyala;
    digitalWrite(PIN_INDIKATOR, indikatorNyala ? HIGH : LOW);
  }
}
