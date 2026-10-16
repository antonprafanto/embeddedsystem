/*
 * =====================================================================
 *  Pertemuan 06 · Praktikum 1 — Voltmeter Mini dengan Potensiometer
 * =====================================================================
 *  Tujuan   : Membaca tegangan analog dari potensiometer dengan ADC ESP32,
 *             lalu menampilkannya dalam empat bentuk: nilai mentah (0–4095),
 *             milivolt hasil analogReadMilliVolts(), tegangan hasil hitung
 *             (rumus linear), dan "bar" teks beserta persentasenya.
 *  Rangkaian: Potensiometer 10 kΩ sebagai pembagi tegangan:
 *               kaki VCC → 3V3, kaki GND → GND, kaki tengah (SIG) → GPIO 34.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200),
 *                   lalu putar kenop potensiometer.
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan isi diagram.json,
 *                   klik ▶, lalu klik potensiometer dan geser kenopnya.
 *  Hasil yang diharapkan:
 *    Setiap 0,5 detik muncul satu baris, misalnya (kenop di tengah, Wokwi)
 *    [DATA] mentah: 2048 | mV: .... | hitung: 1.650 V | [##########----------]  50%
 *    Semua angka naik saat kenop diputar ke kanan.
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_POT = 34;             // Kaki tengah potensiometer → GPIO 34 (ADC1_CH6)
const float TEGANGAN_PENUH = 3.3;   // Anggapan ideal: nilai 4095 setara 3,3 V
const int NILAI_MAKS = 4095;        // ADC 12 bit → nilai 0–4095
const int PANJANG_BAR = 20;         // Jumlah kotak pada bar teks
const unsigned long JEDA_MS = 500;  // Jarak antarpembacaan

bool sudahDiperingatkan = false;    // Agar peringatan kalibrasi muncul sekali saja

void cetakBar(int nilaiMentah);

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);         // 12 bit (bawaan), ditulis agar jelas
  analogSetAttenuation(ADC_11db);   // Atenuasi 11 dB: rentang ±150–3.100 mV (bawaan)
  Serial.println("Voltmeter mini siap. Putar kenop potensiometer.");
}

void loop() {
  int mentah = analogRead(PIN_POT);                   // Nilai mentah, belum dikoreksi
  uint32_t milivolt = analogReadMilliVolts(PIN_POT);  // Milivolt terkoreksi kalibrasi eFuse
  float teganganHitung = mentah * TEGANGAN_PENUH / NILAI_MAKS;  // Hitung dengan float
  int persen = map(mentah, 0, NILAI_MAKS, 0, 100);    // map() menghasilkan bilangan bulat

  Serial.printf("[DATA] mentah: %4d | mV: %4lu | hitung: %.3f V | ",
                mentah, (unsigned long)milivolt, teganganHitung);
  cetakBar(mentah);
  Serial.printf(" %3d%%\n", persen);

  // analogReadMilliVolts() bernilai 0 bila data kalibrasi di eFuse tidak tersedia
  if (milivolt == 0 && mentah > 100 && !sudahDiperingatkan) {
    Serial.println("[PERINGATAN] mV = 0: data kalibrasi eFuse tidak tersedia (wajar di simulator).");
    sudahDiperingatkan = true;
  }
  delay(JEDA_MS);
}

// Cetak bar teks: '#' untuk bagian terisi, '-' untuk sisanya
void cetakBar(int nilaiMentah) {
  int kotakPenuh = (nilaiMentah * PANJANG_BAR + NILAI_MAKS / 2) / NILAI_MAKS;  // Dibulatkan
  Serial.print('[');
  for (int i = 0; i < PANJANG_BAR; i++) {
    Serial.print(i < kotakPenuh ? '#' : '-');
  }
  Serial.print(']');
}
