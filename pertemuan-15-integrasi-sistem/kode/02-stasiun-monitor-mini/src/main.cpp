/*
 * =====================================================================
 *  Pertemuan 15 · Praktikum 2 — Stasiun Monitor Mini (Contoh Integrasi)
 * =====================================================================
 *  Tujuan   : Menggabungkan sensor, layar, alarm, penyimpanan, dan IoT
 *             menjadi SATU sistem yang tetap responsif:
 *             DHT22 → suhu & kelembapan · potensiometer → ambang suhu
 *             (disimpan di NVS) · LED hijau/kuning/merah + buzzer alarm ·
 *             tombol → ganti halaman OLED · data dikirim ke broker MQTT.
 *  Arsitektur:
 *    loop() (inti 1) menjalankan modul tanpa delay(): tombol, sensor_dht,
 *    ambang, alarm (FSM), tampilan. Wi-Fi + MQTT diurus tugas FreeRTOS
 *    terpisah (modul jaringan, inti 0), sehingga proses menyambung ulang
 *    yang lambat tidak menghentikan pembacaan sensor dan alarm.
 *  Rangkaian: sama dengan Praktikum 1; semua pin ada di include/konfigurasi.h.
 *  Library  : DHT sensor library, Adafruit Unified Sensor, Adafruit SSD1306,
 *             Adafruit GFX Library, PubSubClient.
 *  Cara menjalankan:
 *    - Board asli : ganti NIM di include/konfigurasi.h, salin
 *                   include/rahasia.example.h menjadi include/rahasia.h dan
 *                   isi Wi-Fi-mu → Build → Upload → Serial Monitor.
 *    - Wokwi web  : ganti NIM, salin berkas ini ke sketch.ino, buat berkas
 *                   .h dan .cpp lainnya (▾ → New file...), salin diagram.json
 *                   dan libraries.txt, lalu klik ▶.
 *  Hasil yang diharapkan:
 *    OLED menampilkan suhu, ambang, dan status; LED hijau menyala bila
 *    normal. Serial menampilkan [DATA] tiap 2,5 s dan [MQTT] tiap 10 s
 *    (atau segera saat status berubah). Pantau kuliah-st/<NIM>/stasiun/#
 *    di MQTTX. Ketik "status" di Serial Monitor untuk diagnosis.
 * =====================================================================
 */
#include <Arduino.h>
#include "konfigurasi.h"
#include "tombol.h"
#include "sensor_dht.h"
#include "ambang.h"
#include "alarm.h"
#include "tampilan.h"
#include "jaringan.h"

// ---------- Keadaan aplikasi ----------
static DataSensor dataSensor = {NAN, NAN, false, 0};
static Halaman halaman = Halaman::UTAMA;
static unsigned long waktuLayar = 0;
static unsigned long loopTerlamaUs = 0;   // durasi loop() terlama sejak "status" terakhir

// ---------- Deklarasi fungsi ----------
void tanganiTombol();
void tanganiSensor();
void tanganiAmbang();
void perbaruiAlarm();
void titipKeJaringan();
void ledJaringan();
void segarkanLayar();
void kedipDetak();
void layaniPerintahSerial();
void jalankanPerintah(const String &perintah);
void cetakStatus();

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("=== Stasiun Monitor Mini — Pertemuan 15 ===");
  if (strcmp(NIM, "GANTI_NIM") == 0) {
    Serial.println("[PERINGATAN] NIM belum diganti di konfigurasi.h. Topikmu bisa bercampur dengan teman.");
  }
  pinMode(PIN_LED_DETAK, OUTPUT);
  pinMode(PIN_LED_BIRU, OUTPUT);
  tombolMulai();
  sensorMulai();
  ambangMulai();
  alarmMulai();
  if (!tampilanMulai()) {
    Serial.println("[PERINGATAN] OLED tidak menjawab di 0x3C. Sistem tetap jalan tanpa layar.");
  }
  jaringanMulai();
  Serial.println("[INFO] Siap. Tekan tombol untuk ganti halaman. Ketik 'bantuan' di Serial.");
}

// Setiap fungsi di bawah selesai dalam hitungan milidetik (tidak ada delay()),
// jadi loop() berputar ratusan kali per detik dan semua bagian tetap responsif.
void loop() {
  unsigned long mulai = micros();

  tanganiTombol();          // input : tombol → ganti halaman (+ simpan ambang)
  tanganiSensor();          // input : DHT22 tiap 2,5 s → alarm → jaringan
  tanganiAmbang();          // input : potensiometer, hanya di halaman ATUR
  alarmJalankanKeluaran();  // output: LED status + pola bunyi buzzer
  ledJaringan();            // output: LED biru status MQTT
  segarkanLayar();          // output: OLED tiap 250 ms
  kedipDetak();             // diagnosis: LED bawaan berkedip = loop() sehat
  layaniPerintahSerial();   // servis : status, restart, bantuan

  unsigned long durasi = micros() - mulai;
  if (durasi > loopTerlamaUs) loopTerlamaUs = durasi;
}

void tanganiTombol() {
  if (!tombolDitekan()) return;
  if (halaman == Halaman::ATUR_AMBANG) ambangSimpanBilaBerubah();   // keluar halaman ATUR = simpan
  halaman = static_cast<Halaman>((static_cast<int>(halaman) + 1) % JUMLAH_HALAMAN);
  Serial.printf("[INFO] Halaman %d/%d\n", static_cast<int>(halaman) + 1, JUMLAH_HALAMAN);
  waktuLayar = 0;           // paksa layar segera digambar ulang
}

void tanganiSensor() {
  if (!sensorPerbarui(dataSensor)) return;       // belum ada bacaan baru
  perbaruiAlarm();
  if (dataSensor.valid) {
    Serial.printf("[DATA] Suhu %.1f °C | Kelembapan %.1f %% | Ambang %.1f °C | %s\n",
                  dataSensor.suhu, dataSensor.kelembapan, ambangNilai(),
                  alarmNamaStatus(alarmStatus()));
  } else {
    Serial.printf("[PERINGATAN] Gagal membaca DHT22 (%dx berturut-turut).\n",
                  dataSensor.gagalBerturut);
  }
  titipKeJaringan();
}

void tanganiAmbang() {
  if (halaman != Halaman::ATUR_AMBANG) return;   // di halaman lain potensio diabaikan
  float sebelum = ambangNilai();
  ambangAturDariPot();
  if (ambangNilai() != sebelum) {                // ambang berubah → nilai ulang status
    perbaruiAlarm();
    titipKeJaringan();
  }
}

void perbaruiAlarm() {
  StatusAlarm lama = alarmStatus();
  if (alarmPerbarui(dataSensor.valid, dataSensor.gagalBerturut, dataSensor.suhu, ambangNilai())) {
    Serial.printf("[ALARM] %s → %s\n", alarmNamaStatus(lama), alarmNamaStatus(alarmStatus()));
  }
}

void titipKeJaringan() {
  PaketData paket = {dataSensor.suhu, dataSensor.kelembapan, ambangNilai(),
                     dataSensor.valid, alarmStatus()};
  jaringanKirim(paket);     // hanya menaruh di kotak surat; pengiriman diurus tugas jaringan
}

void ledJaringan() {
  bool tersambung = (jaringanInfo().keadaan == KeadaanJaringan::TERSAMBUNG);
  digitalWrite(PIN_LED_BIRU, tersambung || (millis() / 500) % 2 == 0);   // berkedip = menyambung
}

void segarkanLayar() {
  if (millis() - waktuLayar < INTERVAL_LAYAR_MS) return;
  waktuLayar = millis();
  IsiLayar isi = {halaman, dataSensor.suhu, dataSensor.kelembapan, ambangNilai(),
                  dataSensor.valid, alarmStatus(), jaringanInfo()};
  tampilanGambar(isi);
}

void kedipDetak() {
  // Kedip singkat 50 ms tiap 1 s. Bila berhenti berkedip, ada kode yang memblokir loop().
  digitalWrite(PIN_LED_DETAK, (millis() % 1000) < 50);
}

// Sengaja tidak memakai Serial.readStringUntil(): fungsi itu bisa MENUNGGU hingga
// 1 detik selama kamu mengetik. Di sini huruf dikumpulkan sedikit demi sedikit.
void layaniPerintahSerial() {
  static String baris;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c != '\n') {
      if (baris.length() < 32) baris += c;
      continue;
    }
    baris.trim();
    baris.toLowerCase();
    jalankanPerintah(baris);
    baris = "";
  }
}

void jalankanPerintah(const String &perintah) {
  if (perintah == "status") {
    cetakStatus();
  } else if (perintah == "restart") {
    Serial.println("[INFO] Memulai ulang...");
    Serial.flush();
    ESP.restart();
  } else if (perintah == "bantuan") {
    Serial.println("Perintah: status (diagnosis), restart (mulai ulang), bantuan");
  } else if (perintah.length() > 0) {
    Serial.printf("[PERINGATAN] Perintah '%s' tidak dikenal. Ketik 'bantuan'.\n", perintah.c_str());
  }
}

void cetakStatus() {
  InfoJaringan info = jaringanInfo();
  Serial.println("------------- STATUS -------------");
  Serial.printf("Aktif           : %lu s\n", millis() / 1000);
  Serial.printf("Ambang          : %.1f °C\n", ambangNilai());
  Serial.printf("Alarm           : %s\n", alarmNamaStatus(alarmStatus()));
  Serial.printf("Jaringan        : %s, IP %s\n", jaringanNamaKeadaan(info.keadaan), info.ip);
  Serial.printf("Paket terkirim  : %lu\n", (unsigned long)info.jumlahKirim);
  Serial.printf("loop() terlama  : %.1f ms\n", loopTerlamaUs / 1000.0);
  Serial.printf("Heap bebas      : %lu byte\n", (unsigned long)ESP.getFreeHeap());
  Serial.printf("Sisa stack tugas jaringan: %lu byte\n", (unsigned long)jaringanSisaStack());
  Serial.println("----------------------------------");
  loopTerlamaUs = 0;        // mulai mengukur ulang
}
