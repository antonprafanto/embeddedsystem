/*
 * =====================================================================
 *  Pertemuan 05 · Praktikum 2 — Interupsi Tombol
 * =====================================================================
 *  Tujuan   : Menghitung tekanan tombol memakai interupsi (ISR), lengkap
 *             dengan debounce sederhana dan akses variabel bersama yang aman.
 *  Rangkaian: Tombol    → GPIO 4 dan GND (memakai INPUT_PULLUP)
 *             LED merah → resistor 220 Ω → GPIO 23, katoda ke GND
 *             (LED hijau di GPIO 19 boleh tetap terpasang, tidak dipakai)
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200 baud).
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan isi diagram.json,
 *                   lalu klik ▶.
 *  Hasil yang diharapkan:
 *    Setiap tekanan langsung membalik LED merah, lalu tercetak misalnya:
 *      [DATA] Tekanan sah: 1 | interupsi mentah: 9
 *    Angka "interupsi mentah" lebih besar karena pantulan (bouncing).
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_TOMBOL    = 4;
const int PIN_LED_MERAH = 23;
const unsigned long JEDA_DEBOUNCE_MS = 200;  // tekanan sah minimal berjarak 200 ms
const unsigned long JEDA_TENANG_MS   = 50;   // lapor setelah pantulan reda

// Variabel bersama ISR <-> loop() WAJIB volatile:
// nilainya bisa berubah kapan saja di luar alur loop().
volatile unsigned long jumlahTekan      = 0;  // tekanan sah (sudah disaring)
volatile unsigned long jumlahInterupsi  = 0;  // semua interupsi, termasuk pantulan
volatile unsigned long waktuSahTerakhir = 0;  // kapan tekanan sah terakhir (ms)
volatile unsigned long waktuInterupsiTerakhir = 0;

// "Kunci" untuk bagian kritis (critical section) — pola resmi ESP32
portMUX_TYPE kunci = portMUX_INITIALIZER_UNLOCKED;

// Variabel milik loop() saja
unsigned long tekanSebelumnya = 0;
bool ledNyala   = false;
bool perluLapor = false;

// ISR: singkat, tanpa Serial, tanpa delay(), disimpan di IRAM
void IRAM_ATTR isrTombol() {
  unsigned long sekarang = millis();
  portENTER_CRITICAL_ISR(&kunci);
  jumlahInterupsi = jumlahInterupsi + 1;   // volatile: tulis x = x + 1, bukan x++
  waktuInterupsiTerakhir = sekarang;
  if (sekarang - waktuSahTerakhir >= JEDA_DEBOUNCE_MS) {  // debounce sederhana
    jumlahTekan = jumlahTekan + 1;
    waktuSahTerakhir = sekarang;
  }
  portEXIT_CRITICAL_ISR(&kunci);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED_MERAH, OUTPUT);
  pinMode(PIN_TOMBOL, INPUT_PULLUP);   // tidak ditekan = HIGH, ditekan = LOW

  // Panggil isrTombol() setiap tegangan pin turun dari HIGH ke LOW
  attachInterrupt(digitalPinToInterrupt(PIN_TOMBOL), isrTombol, FALLING);

  Serial.println();
  Serial.println("=== Pertemuan 05 · Interupsi Tombol ===");
  Serial.println("Tekan tombol di GPIO 4. Penghitung bekerja di dalam ISR.");
}

void loop() {
  // Salin variabel bersama di dalam bagian kritis supaya tidak "tabrakan"
  // dengan ISR yang bisa datang di tengah-tengah proses penyalinan.
  portENTER_CRITICAL(&kunci);
  unsigned long tekan      = jumlahTekan;
  unsigned long interupsi  = jumlahInterupsi;
  unsigned long tInterupsi = waktuInterupsiTerakhir;
  portEXIT_CRITICAL(&kunci);

  if (tekan != tekanSebelumnya) {      // ada tekanan sah baru → bereaksi seketika
    tekanSebelumnya = tekan;
    ledNyala = !ledNyala;
    digitalWrite(PIN_LED_MERAH, ledNyala ? HIGH : LOW);
    perluLapor = true;
  }

  // Cetak laporan setelah pantulan reda, supaya hitungan mentahnya lengkap
  if (perluLapor && millis() - tInterupsi >= JEDA_TENANG_MS) {
    perluLapor = false;
    Serial.printf("[DATA] Tekanan sah: %lu | interupsi mentah: %lu\n", tekan, interupsi);
  }

  // loop() bebas mengerjakan hal lain di sini; tekanan tidak akan terlewat.
}
