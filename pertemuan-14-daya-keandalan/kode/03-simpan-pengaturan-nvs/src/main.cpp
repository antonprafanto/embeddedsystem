/*
 * =====================================================================
 *  Pertemuan 14 · Praktikum 3 — Menyimpan Pengaturan di NVS
 * =====================================================================
 *  Tujuan   : Menyimpan pengaturan (interval kedip LED) dan penghitung
 *             nyala di NVS memakai pustaka Preferences, sehingga tetap ada
 *             walaupun ESP32 di-reset atau catunya dicabut.
 *  Rangkaian: Tidak perlu komponen tambahan (LED bawaan GPIO 2).
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor, lalu ketik
 *                   perintah: lihat | interval 250 | hapus | restart
 *    - Wokwi web  : salin sketch.ino + diagram.json, klik ▶, ketik perintah
 *                   di Serial Monitor. Uji dengan "restart", jangan Stop.
 *  Hasil yang diharapkan:
 *    "interval 250" membuat LED berkedip lebih cepat dan nilainya tetap
 *    250 ms setelah "restart", tombol EN, atau kabel USB dicabut-colok.
 *    "Jumlah nyala" bertambah satu setiap kali ESP32 menyala.
 * =====================================================================
 */
#include <Arduino.h>
#include <Preferences.h>

const int PIN_LED = 2;
const char *NAMA_RUANG = "pengaturan";       // namespace NVS (maks. 15 karakter)
const char *KUNCI_INTERVAL = "interval";     // key (maks. 15 karakter)
const char *KUNCI_NYALA = "jmlNyala";
const uint32_t INTERVAL_BAWAAN_MS = 500;     // nilai bawaan yang aman (fail-safe)
const uint32_t INTERVAL_MIN_MS = 100;
const uint32_t INTERVAL_MAKS_MS = 5000;

Preferences pengaturan;
uint32_t intervalKedip = INTERVAL_BAWAAN_MS;
uint32_t jumlahNyala = 0;
unsigned long waktuKedipTerakhir = 0;
bool ledMenyala = false;

void muatPengaturan();
void tampilkanPengaturan();
void tampilkanBantuan();
void prosesPerintah(String perintah);

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(PIN_LED, OUTPUT);

  pengaturan.begin(NAMA_RUANG, false);       // false = boleh baca dan tulis
  muatPengaturan();

  // Penghitung nyala ditulis SEKALI per boot (bukan di loop) agar flash awet
  jumlahNyala = pengaturan.getUInt(KUNCI_NYALA, 0) + 1;
  pengaturan.putUInt(KUNCI_NYALA, jumlahNyala);

  Serial.println();
  Serial.println("=== Praktikum 3: Pengaturan Tersimpan di NVS ===");
  tampilkanPengaturan();
  tampilkanBantuan();
}

void loop() {
  // 1) Kedipkan LED tanpa delay() agar perintah Serial tetap terlayani
  if (millis() - waktuKedipTerakhir >= intervalKedip) {
    waktuKedipTerakhir = millis();
    ledMenyala = !ledMenyala;
    digitalWrite(PIN_LED, ledMenyala ? HIGH : LOW);
  }

  // 2) Layani perintah dari Serial Monitor, satu baris per perintah
  if (Serial.available()) {
    String perintah = Serial.readStringUntil('\n');
    perintah.trim();
    if (perintah.length() > 0) {
      prosesPerintah(perintah);
    }
  }
}

void muatPengaturan() {
  uint32_t nilai = pengaturan.getUInt(KUNCI_INTERVAL, INTERVAL_BAWAAN_MS);
  // Validasi: data tersimpan bisa saja rusak atau berasal dari program lama
  if (nilai < INTERVAL_MIN_MS || nilai > INTERVAL_MAKS_MS) {
    Serial.println("[PERINGATAN] Interval tersimpan tidak wajar, pakai nilai bawaan.");
    nilai = INTERVAL_BAWAAN_MS;
  }
  intervalKedip = nilai;
}

void tampilkanPengaturan() {
  Serial.printf("[DATA] Jumlah nyala   : %lu kali\n", (unsigned long)jumlahNyala);
  Serial.printf("[DATA] Interval kedip : %lu ms\n", (unsigned long)intervalKedip);
}

void tampilkanBantuan() {
  Serial.println("Perintah: lihat | interval <100-5000> | hapus | restart");
}

void prosesPerintah(String perintah) {
  if (perintah == "lihat") {
    tampilkanPengaturan();
  } else if (perintah.startsWith("interval ")) {
    long nilai = perintah.substring(9).toInt();     // toInt() = 0 bila bukan angka
    if (nilai < (long)INTERVAL_MIN_MS || nilai > (long)INTERVAL_MAKS_MS) {
      Serial.println("[PERINGATAN] Ditolak: interval harus 100 sampai 5000 ms.");
      return;
    }
    if ((uint32_t)nilai == intervalKedip) {
      Serial.println("[INFO] Nilainya sama, flash tidak perlu ditulis.");
      return;
    }
    intervalKedip = (uint32_t)nilai;
    pengaturan.putUInt(KUNCI_INTERVAL, intervalKedip);   // tulis hanya bila berubah
    Serial.printf("[INFO] Interval %lu ms disimpan ke NVS.\n", (unsigned long)intervalKedip);
  } else if (perintah == "hapus") {
    pengaturan.clear();                       // hapus semua key di namespace ini
    intervalKedip = INTERVAL_BAWAAN_MS;
    Serial.println("[INFO] Semua pengaturan dihapus, kembali ke nilai bawaan.");
  } else if (perintah == "restart") {
    Serial.println("[INFO] Memulai ulang ESP32...");
    Serial.flush();
    pengaturan.end();
    ESP.restart();
  } else {
    Serial.printf("[PERINGATAN] Perintah tidak dikenal: %s\n", perintah.c_str());
    tampilkanBantuan();
  }
}
