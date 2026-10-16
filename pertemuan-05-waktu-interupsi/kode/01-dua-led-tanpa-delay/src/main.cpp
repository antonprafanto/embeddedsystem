/*
 * =====================================================================
 *  Pertemuan 05 · Praktikum 1 — Dua LED tanpa delay()
 * =====================================================================
 *  Tujuan   : Menjalankan beberapa "tugas" sekaligus tanpa delay():
 *             LED merah berganti tiap 300 ms, LED hijau tiap 1000 ms,
 *             dan tombol tetap langsung ditanggapi kapan pun ditekan.
 *  Rangkaian: LED merah → resistor 220 Ω → GPIO 23, katoda ke GND
 *             LED hijau → resistor 220 Ω → GPIO 19, katoda ke GND
 *             Tombol    → GPIO 4 dan GND (memakai INPUT_PULLUP)
 *             LED biru bawaan board (GPIO 2) menjadi penanda tombol.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200 baud).
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan isi diagram.json,
 *                   lalu klik ▶.
 *  Hasil yang diharapkan:
 *    Kedua LED berkedip dengan irama berbeda. LED biru menyala selama
 *    tombol ditahan, dan setiap tekanan langsung tercatat, misalnya:
 *      [TOMBOL] Ditekan ke-1 pada t = 3120 ms
 *      [INFO] loop() berputar 1043211 kali dalam 5 detik terakhir
 * =====================================================================
 */
#include <Arduino.h>

// ----- Pin -----
const int PIN_LED_MERAH  = 23;
const int PIN_LED_HIJAU  = 19;
const int PIN_TOMBOL     = 4;
const int PIN_LED_BAWAAN = 2;    // LED biru di board ESP32 DevKit V1

// ----- Selang waktu tiap tugas (milidetik) -----
const unsigned long INTERVAL_MERAH   = 300;
const unsigned long INTERVAL_HIJAU   = 1000;
const unsigned long INTERVAL_LAPORAN = 5000;
const unsigned long JEDA_DEBOUNCE    = 50;   // lama tombol harus stabil

// ----- "Pengatur waktu" milik masing-masing tugas -----
// Selalu pakai unsigned long untuk menyimpan hasil millis().
unsigned long waktuMerah   = 0;   // kapan LED merah terakhir berganti
unsigned long waktuHijau   = 0;   // kapan LED hijau terakhir berganti
unsigned long waktuLaporan = 0;   // kapan laporan terakhir dicetak

bool ledMerahNyala = false;
bool ledHijauNyala = false;

// ----- Data tombol untuk debouncing (lihat Pertemuan 4) -----
int bacaanTerakhir = HIGH;        // bacaan mentah pada putaran sebelumnya
int statusTombol   = HIGH;        // status yang sudah stabil
unsigned long waktuBerubah = 0;   // kapan bacaan mentah terakhir berubah
unsigned long jumlahTekan  = 0;

unsigned long jumlahPutaran = 0;  // berapa kali loop() sudah berputar

// Deklarasi fungsi (supaya sah sebagai main.cpp maupun sketch.ino)
void tugasLedMerah(unsigned long sekarang);
void tugasLedHijau(unsigned long sekarang);
void tugasTombol(unsigned long sekarang);
void tugasLaporan(unsigned long sekarang);

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED_MERAH, OUTPUT);
  pinMode(PIN_LED_HIJAU, OUTPUT);
  pinMode(PIN_LED_BAWAAN, OUTPUT);
  pinMode(PIN_TOMBOL, INPUT_PULLUP);   // tidak ditekan = HIGH, ditekan = LOW

  Serial.println();
  Serial.println("=== Pertemuan 05 · Dua LED tanpa delay() ===");
  Serial.println("LED merah 300 ms, LED hijau 1000 ms. Tekan tombol kapan saja!");
}

void loop() {
  unsigned long sekarang = millis();   // baca "jam" sekali di awal putaran

  // Setiap tugas hanya MENGECEK apakah sudah waktunya, lalu segera selesai.
  tugasLedMerah(sekarang);
  tugasLedHijau(sekarang);
  tugasTombol(sekarang);
  tugasLaporan(sekarang);

  jumlahPutaran++;
}

// Tugas 1: balik LED merah setiap INTERVAL_MERAH milidetik
void tugasLedMerah(unsigned long sekarang) {
  if (sekarang - waktuMerah >= INTERVAL_MERAH) {   // pola aman, tahan luapan
    waktuMerah = sekarang;                         // catat waktu pergantian
    ledMerahNyala = !ledMerahNyala;
    digitalWrite(PIN_LED_MERAH, ledMerahNyala ? HIGH : LOW);
  }
}

// Tugas 2: balik LED hijau setiap INTERVAL_HIJAU milidetik
void tugasLedHijau(unsigned long sekarang) {
  if (sekarang - waktuHijau >= INTERVAL_HIJAU) {
    waktuHijau = sekarang;
    ledHijauNyala = !ledHijauNyala;
    digitalWrite(PIN_LED_HIJAU, ledHijauNyala ? HIGH : LOW);
  }
}

// Tugas 3: baca tombol di SETIAP putaran (tidak pernah menunggu)
void tugasTombol(unsigned long sekarang) {
  int bacaan = digitalRead(PIN_TOMBOL);

  // LED biru mengikuti tombol seketika: bukti program tidak "membeku"
  digitalWrite(PIN_LED_BAWAAN, bacaan == LOW ? HIGH : LOW);

  if (bacaan != bacaanTerakhir) {      // bacaan berubah (bisa jadi pantulan)
    waktuBerubah = sekarang;
    bacaanTerakhir = bacaan;
  }

  // Terima perubahan hanya jika sudah stabil selama JEDA_DEBOUNCE
  if (sekarang - waktuBerubah >= JEDA_DEBOUNCE && bacaan != statusTombol) {
    statusTombol = bacaan;
    if (statusTombol == LOW) {         // baru saja ditekan
      jumlahTekan++;
      // %lu = tempat untuk mencetak angka bertipe unsigned long
      Serial.printf("[TOMBOL] Ditekan ke-%lu pada t = %lu ms\n", jumlahTekan, sekarang);
    }
  }
}

// Tugas 4: lapor kecepatan loop() setiap 5 detik
void tugasLaporan(unsigned long sekarang) {
  if (sekarang - waktuLaporan >= INTERVAL_LAPORAN) {
    waktuLaporan = sekarang;
    Serial.printf("[INFO] loop() berputar %lu kali dalam 5 detik terakhir\n", jumlahPutaran);
    jumlahPutaran = 0;
  }
}
