/*
 * =====================================================================
 *  Pertemuan 07 · Praktikum 2 — Dimmer dengan Potensiometer
 * =====================================================================
 *  Tujuan   : Menggabungkan input analog (ADC) dan output PWM: putaran
 *             potensiometer mengatur kecerahan LED, seperti dimmer lampu.
 *  Rangkaian: Potensiometer: kaki kiri → GND, kaki tengah → GPIO 34,
 *             kaki kanan → 3V3. LED: GPIO 23 → resistor 220 Ω → LED → GND.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → buka Serial Monitor,
 *                   lalu putar potensiometer.
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan diagram.json ke tab
 *                   diagram.json, klik ▶, lalu klik kenop potensiometer
 *                   dan geser.
 *  Hasil yang diharapkan:
 *    Makin jauh kenop diputar, makin terang LED. Serial Monitor menampilkan
 *    nilai ADC dan duty setiap kali nilainya berubah cukup jauh.
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_POT = 34;           // ADC1, pin input saja
const int PIN_LED = 23;
const int FREKUENSI_PWM = 5000;
const int RESOLUSI_PWM = 12;      // 12 bit → duty 0–4095, sama dengan rentang analogRead()
const int DUTY_MAKS = 4095;
const int AMBANG_CETAK = 40;      // cetak hanya bila berubah ≥ 40 (±1%) agar tidak banjir
const int JEDA_BACA_MS = 20;

int nilaiTerakhir = -1000;        // pasti berbeda dengan bacaan pertama

void setup() {
  Serial.begin(115200);
  ledcAttach(PIN_LED, FREKUENSI_PWM, RESOLUSI_PWM);
  Serial.println("[INFO] Dimmer siap. Putar potensiometer untuk mengatur kecerahan LED.");
}

void loop() {
  int nilaiAdc = analogRead(PIN_POT);   // 0–4095
  // Resolusi PWM sama dengan ADC (12 bit), jadi nilainya bisa langsung dipakai.
  // Kalau resolusi PWM 8 bit: ledcWrite(PIN_LED, map(nilaiAdc, 0, 4095, 0, 255));
  ledcWrite(PIN_LED, nilaiAdc);

  if (abs(nilaiAdc - nilaiTerakhir) >= AMBANG_CETAK) {
    nilaiTerakhir = nilaiAdc;
    Serial.printf("[DATA] ADC = %4d -> duty = %4d/4095 (%3d%%)\n",
                  nilaiAdc, nilaiAdc, nilaiAdc * 100 / DUTY_MAKS);
  }
  delay(JEDA_BACA_MS);
}
