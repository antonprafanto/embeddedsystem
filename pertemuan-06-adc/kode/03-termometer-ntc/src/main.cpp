/*
 * =====================================================================
 *  Pertemuan 06 · Praktikum 3 — Termometer Digital dengan NTC
 * =====================================================================
 *  Tujuan   : Membaca modul sensor suhu NTC, meredam derau dengan rata-rata
 *             16 sampel, lalu menghitung suhu (°C) dengan persamaan Beta.
 *  Rangkaian: Modul wokwi-ntc-temperature-sensor: VCC → 3V3, GND → GND,
 *             OUT → GPIO 32. Di dalam modul: resistor 10 kΩ dari VCC ke OUT
 *             dan NTC 10 kΩ dari OUT ke GND (suhu naik → OUT turun).
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200),
 *                   lalu pegang NTC dengan jari dan amati suhunya naik.
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan isi diagram.json,
 *                   klik ▶, lalu klik modul NTC dan geser suhunya.
 *  Hasil yang diharapkan:
 *    Setiap 1 detik muncul satu baris, misalnya (Wokwi, 24 °C)
 *    [DATA] mentah:  2093.0 | R_NTC:  10455 ohm | suhu:  24.0 °C
 * =====================================================================
 */
#include <Arduino.h>
#include <math.h>   // log()

const int PIN_NTC = 32;              // OUT modul NTC → GPIO 32 (ADC1_CH4)
const int NILAI_MAKS = 4095;         // ADC 12 bit
const float BETA = 3950.0;           // Koefisien Beta NTC (atribut beta Wokwi)
const float R0 = 10000.0;            // Hambatan NTC pada suhu acuan 25 °C (10 kΩ)
const float T0_KELVIN = 298.15;      // Suhu acuan 25 °C dalam kelvin
const int JUMLAH_SAMPEL = 16;        // Banyak sampel yang dirata-rata
const unsigned long JEDA_MS = 1000;

float bacaRataRata(int pin, int jumlah);
float hitungSuhuCelsius(float mentah);

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  Serial.println("Termometer NTC siap (rata-rata 16 sampel).");
}

void loop() {
  float mentah = bacaRataRata(PIN_NTC, JUMLAH_SAMPEL);

  // Nilai di ujung skala berarti kabel lepas atau terhubung singkat
  if (mentah < 1.0 || mentah > NILAI_MAKS - 1.0) {
    Serial.println("[PERINGATAN] Nilai di ujung skala: periksa kabel modul NTC.");
  } else {
    float hambatan = R0 * mentah / (NILAI_MAKS - mentah);  // R_NTC dari pembagi tegangan
    float suhu = hitungSuhuCelsius(mentah);
    Serial.printf("[DATA] mentah: %7.1f | R_NTC: %6.0f ohm | suhu: %5.1f °C\n",
                  mentah, hambatan, suhu);
  }
  delay(JEDA_MS);
}

// Rata-rata beberapa sampel: derau acak saling meniadakan
float bacaRataRata(int pin, int jumlah) {
  long total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += analogRead(pin);
    delayMicroseconds(100);            // Jeda singkat antarsampel
  }
  return (float)total / jumlah;
}

// Persamaan Beta: 1/T = 1/T0 + (1/BETA) × ln(R/R0), T dalam kelvin
float hitungSuhuCelsius(float mentah) {
  float rasio = mentah / (NILAI_MAKS - mentah);       // R/R0 (NTC di sisi bawah pembagi)
  float invT = 1.0 / T0_KELVIN + log(rasio) / BETA;   // 1/T dalam 1/kelvin
  return 1.0 / invT - 273.15;                         // kelvin → derajat Celsius
}
