/*
 * =====================================================================
 *  Pertemuan 11 · Praktikum 1 — Lampu Lalu Lintas FSM
 * =====================================================================
 *  Tujuan   : Membuat lampu lalu lintas dengan tombol penyeberang sebagai
 *             mesin keadaan hingga (FSM) non-blocking: enum class + switch
 *             + millis(), dengan kode yang dipecah menjadi beberapa modul.
 *  Rangkaian: LED merah GPIO 25, kuning GPIO 26, hijau GPIO 27 (tiap LED
 *             lewat resistor 220 Ω, katoda ke GND). Tombol penyeberang
 *             GPIO 4 ke GND (INPUT_PULLUP).
 *  Berkas   : src/main.cpp (FSM) · include/konfigurasi.h (pin & durasi)
 *             include/lampu.h + src/lampu.cpp · include/tombol.h + src/tombol.cpp
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino; buat berkas baru
 *                   konfigurasi.h, lampu.h, lampu.cpp, tombol.h, tombol.cpp
 *                   (tanpa folder) lalu salin isinya; salin diagram.json; klik ▶.
 *  Hasil yang diharapkan:
 *    Hijau 8 s → kuning 2 s → merah 5 s → berulang. Tombol ditekan saat
 *    hijau → hijau dipersingkat (minimal 3 s). Setiap perpindahan keadaan
 *    tercatat di Serial Monitor beserta waktunya.
 * =====================================================================
 */
#include <Arduino.h>
#include "konfigurasi.h"
#include "lampu.h"
#include "tombol.h"

// ---- Daftar keadaan (state) FSM ----
enum class Keadaan { HIJAU, KUNING, MERAH };

Keadaan keadaan = Keadaan::HIJAU;   // keadaan yang sedang aktif (selalu tepat satu)
unsigned long waktuMasuk = 0;       // millis() saat masuk keadaan sekarang
unsigned long waktuMulai = 0;       // millis() saat program mulai (untuk cap waktu)
bool adaPenyeberang = false;        // "ingatan": tombol sudah ditekan, belum dilayani

// ---- Deklarasi fungsi ----
void masukKeadaan(Keadaan baru, const char *alasan);
const char *namaKeadaan(Keadaan k);
void cetakWaktu();

void setup() {
  Serial.begin(115200);
  lampuMulai();
  tombolMulai();
  waktuMulai = millis();
  Serial.println("[INFO] Lampu lalu lintas FSM siap. Tekan tombol untuk menyeberang.");
  masukKeadaan(Keadaan::HIJAU, "mulai");
}

void loop() {
  // Sudah berapa lama di keadaan ini? Pengurangan unsigned tetap benar
  // walau millis() meluap kembali ke 0 setelah ±49,7 hari.
  unsigned long lama = millis() - waktuMasuk;

  // 1) PERISTIWA: periksa tombol (tidak pernah menunggu)
  if (tombolDitekan()) {
    cetakWaktu();
    if (keadaan == Keadaan::MERAH) {
      Serial.println("PERISTIWA: tombol ditekan, lampu sudah merah -> silakan menyeberang");
    } else {
      adaPenyeberang = true;
      Serial.println("PERISTIWA: tombol penyeberang ditekan");
    }
  }

  // 2) TRANSISI: satu case per keadaan, berisi aturan kapan harus pindah
  switch (keadaan) {
    case Keadaan::HIJAU:
      if (lama >= LAMA_HIJAU_MS) {
        masukKeadaan(Keadaan::KUNING, "waktu hijau habis");
      } else if (adaPenyeberang && lama >= LAMA_HIJAU_MIN_MS) {
        masukKeadaan(Keadaan::KUNING, "ada penyeberang");
      }
      break;

    case Keadaan::KUNING:
      if (lama >= LAMA_KUNING_MS) {
        masukKeadaan(Keadaan::MERAH, "waktu kuning habis");
      }
      break;

    case Keadaan::MERAH:
      if (lama >= LAMA_MERAH_MS) {
        masukKeadaan(Keadaan::HIJAU, "waktu merah habis");
      }
      break;
  }
}

// 3) AKSI MASUK-KEADAAN: semua yang terjadi SEKALI saat pindah keadaan.
// Satu-satunya tempat yang boleh mengubah variabel "keadaan".
void masukKeadaan(Keadaan baru, const char *alasan) {
  keadaan = baru;
  waktuMasuk = millis();            // mulai menghitung lama di keadaan baru

  switch (baru) {
    case Keadaan::HIJAU:
      lampuTampilkan(Warna::HIJAU);
      break;
    case Keadaan::KUNING:
      lampuTampilkan(Warna::KUNING);
      break;
    case Keadaan::MERAH:
      lampuTampilkan(Warna::MERAH);
      adaPenyeberang = false;       // permintaan terlayani: penyeberang jalan saat merah
      break;
  }

  cetakWaktu();
  Serial.printf("KEADAAN -> %-6s (%s)\n", namaKeadaan(baru), alasan);
}

const char *namaKeadaan(Keadaan k) {
  switch (k) {
    case Keadaan::HIJAU:  return "HIJAU";
    case Keadaan::KUNING: return "KUNING";
    case Keadaan::MERAH:  return "MERAH";
  }
  return "?";                       // tidak akan terjadi; menenangkan kompilator
}

// Cap waktu sejak program mulai, mis. "[  12.3 s] "
void cetakWaktu() {
  unsigned long ms = millis() - waktuMulai;
  Serial.printf("[%4lu.%lu s] ", ms / 1000, (ms % 1000) / 100);
}
