/*
 * =====================================================================
 *  Pertemuan 01 · Praktikum 1 — Halo, ESP32!
 * =====================================================================
 *  Tujuan   : Program pertamamu. ESP32 menyapa lewat Serial Monitor,
 *             menampilkan informasi cip, lalu menghitung detik.
 *  Rangkaian: Tidak perlu komponen tambahan. Cukup board ESP32 + kabel USB
 *             (atau simulator Wokwi).
 *  Cara menjalankan:
 *    - Board asli : klik Build (✓) → Upload (→) → Serial Monitor (ikon colokan).
 *    - Wokwi web  : salin seluruh isi berkas ini ke sketch.ino, lalu klik ▶.
 *  Hasil yang diharapkan (Serial Monitor, 115200 baud):
 *    =========================================
 *      Halo, ESP32! Saya siap belajar sistem tertanam.
 *    ...
 *    [INFO] Sudah menyala 1 detik
 *    [INFO] Sudah menyala 2 detik
 * =====================================================================
 */
#include <Arduino.h>

// Ganti teks di dalam tanda kutip dengan nama dan NIM-mu.
const char *NAMA_PEMROGRAM = "Nama Kamu (NIM)";

// Variabel untuk menghitung berapa detik sudah berlalu.
unsigned long jumlahDetik = 0;

void setup() {
  // setup() dijalankan SEKALI saja ketika board menyala atau di-reset.
  Serial.begin(115200);   // Buka jalur komunikasi ke laptop, kecepatan 115200 baud
  delay(1000);            // Beri waktu 1 detik agar Serial Monitor siap

  Serial.println();
  Serial.println("=========================================");
  Serial.println("  Halo, ESP32! Saya siap belajar sistem tertanam.");
  Serial.print("  Pemrogram    : ");
  Serial.println(NAMA_PEMROGRAM);
  Serial.println("=========================================");

  // Informasi tentang cip ESP32 yang sedang kamu pakai
  Serial.print("  Model cip    : ");
  Serial.println(ESP.getChipModel());
  Serial.print("  Jumlah inti  : ");
  Serial.println(ESP.getChipCores());
  Serial.print("  Frekuensi CPU: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("  Memori flash : ");
  Serial.print(ESP.getFlashChipSize() / (1024 * 1024));
  Serial.println(" MB");
  Serial.println("=========================================");
}

void loop() {
  // loop() dijalankan BERULANG-ULANG selama board menyala.
  delay(1000);            // Tunggu 1 detik (1000 milidetik)
  jumlahDetik = jumlahDetik + 1;

  Serial.print("[INFO] Sudah menyala ");
  Serial.print(jumlahDetik);
  Serial.println(" detik");
}
