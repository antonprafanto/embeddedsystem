/*
 * =====================================================================
 *  Pertemuan 07 · Praktikum 3 — Menggerakkan Servo
 * =====================================================================
 *  Tujuan   : Memahami sinyal servo (50 Hz, lebar pulsa = sudut):
 *             uji sapuan 0° → 180° → 0°, lalu sudut servo mengikuti
 *             putaran potensiometer.
 *  Rangkaian: Servo SG90: kabel oranye (sinyal) → GPIO 19, merah (V+) → 5 V,
 *             cokelat (GND) → GND. Potensiometer: kiri → GND,
 *             tengah → GPIO 34, kanan → 3V3.
 *             Board asli: lebih aman pakai catu 5 V terpisah untuk servo
 *             dengan GND disatukan ke GND ESP32.
 *  Pustaka  : ESP32Servo (madhephaestus/ESP32Servo@^3.2.1)
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → buka Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino, diagram.json ke tab
 *                   diagram.json, dan isi libraries.txt ke tab Library
 *                   Manager (atau tambahkan pustaka "ESP32Servo"), klik ▶.
 *  Hasil yang diharapkan:
 *    Servo menyapu 0° → 180° → 0°, lalu mengikuti putaran potensiometer.
 *    Serial Monitor menampilkan sudut dan lebar pulsanya.
 * =====================================================================
 */
#include <Arduino.h>
#include <ESP32Servo.h>

const int PIN_SERVO = 19;
const int PIN_POT = 34;
const int PULSA_MIN_US = 500;     // lebar pulsa untuk 0°
const int PULSA_MAKS_US = 2500;   // lebar pulsa untuk 180° (batas atas ESP32Servo)
const int JEDA_SAPUAN_MS = 15;    // beri waktu servo bergerak tiap 1°

Servo servo;
int sudutTerakhir = -100;         // pasti berbeda dengan sudut pertama

int sudutKePulsa(int sudut);
void gerakDanCetak(int sudut);

void setup() {
  Serial.begin(115200);
  servo.setPeriodHertz(50);       // 50 Hz → satu periode 20 ms
  // Tanpa angka min/maks, attach(pin) memakai 544–2400 µs.
  servo.attach(PIN_SERVO, PULSA_MIN_US, PULSA_MAKS_US);
  Serial.println("[INFO] Servo siap di GPIO 19 (50 Hz, pulsa 500-2500 us)");

  Serial.println("[INFO] Uji sapuan 0 -> 180 -> 0 derajat");
  for (int sudut = 0; sudut <= 180; sudut++) {
    gerakDanCetak(sudut);
  }
  for (int sudut = 179; sudut >= 0; sudut--) {
    gerakDanCetak(sudut);
  }
  Serial.println("[INFO] Sapuan selesai. Sekarang putar potensiometer.");
}

void loop() {
  int nilaiPot = analogRead(PIN_POT);              // 0–4095
  int sudut = map(nilaiPot, 0, 4095, 0, 180);      // ubah ke 0–180°
  if (abs(sudut - sudutTerakhir) >= 2) {           // abaikan goyangan kecil ADC
    sudutTerakhir = sudut;
    servo.write(sudut);
    Serial.printf("[DATA] pot = %4d -> sudut = %3d derajat -> pulsa = %4d us\n",
                  nilaiPot, sudut, sudutKePulsa(sudut));
  }
  delay(20);                                       // satu periode sinyal servo
}

// Sama dengan perhitungan di dalam ESP32Servo: 0° → 500 µs, 180° → 2500 µs
int sudutKePulsa(int sudut) {
  return map(sudut, 0, 180, PULSA_MIN_US, PULSA_MAKS_US);
}

void gerakDanCetak(int sudut) {
  servo.write(sudut);
  if (sudut % 90 == 0) {
    Serial.printf("[DATA] sudut = %3d derajat -> pulsa = %4d us\n", sudut, sudutKePulsa(sudut));
  }
  delay(JEDA_SAPUAN_MS);
}
