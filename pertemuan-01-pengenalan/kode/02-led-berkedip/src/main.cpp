/*
 * =====================================================================
 *  Pertemuan 01 · Praktikum 2 — LED Berkedip (Blink)
 * =====================================================================
 *  Tujuan   : Mengendalikan dunia fisik untuk pertama kali: menyalakan dan
 *             memadamkan LED bawaan board secara bergantian.
 *  Rangkaian: Tidak perlu komponen tambahan. LED biru bawaan board
 *             ESP32 DevKit V1 sudah terhubung ke GPIO 2.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → amati LED biru di board.
 *    - Wokwi web  : salin isi berkas ini ke sketch.ino, lalu klik ▶.
 *  Hasil yang diharapkan:
 *    LED biru menyala 0,5 detik, padam 0,5 detik, terus berulang.
 *    Serial Monitor menampilkan "LED: NYALA" dan "LED: PADAM" bergantian.
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_LED = 2;        // LED bawaan board ESP32 DevKit V1 ada di GPIO 2
const int JEDA_MS = 500;      // Lama nyala dan lama padam, dalam milidetik

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);   // Jadikan GPIO 2 sebagai keluaran (output)
  Serial.println("Program LED berkedip dimulai.");
}

void loop() {
  digitalWrite(PIN_LED, HIGH);  // Pin bertegangan 3,3 V → LED menyala
  Serial.println("LED: NYALA");
  delay(JEDA_MS);

  digitalWrite(PIN_LED, LOW);   // Pin bertegangan 0 V → LED padam
  Serial.println("LED: PADAM");
  delay(JEDA_MS);
}
