/*
 * =====================================================================
 *  Pertemuan 04 · Praktikum 1 — Tombol dan LED
 * =====================================================================
 *  Tujuan   : Membaca tombol dengan digitalRead() memakai pull-up internal
 *             (INPUT_PULLUP), lalu menyalakan LED selama tombol ditekan.
 *             Nilai digitalRead dicetak agar "logika terbalik" terlihat.
 *  Rangkaian: - Tombol 4 kaki: satu kaki ke GPIO 4, kaki diagonalnya ke GND.
 *               Tidak perlu resistor karena memakai pull-up internal ESP32.
 *             - GPIO 23 → resistor 220 Ω → anoda LED (kaki panjang);
 *               katoda LED (kaki pendek) → GND.
 *  Cara menjalankan:
 *    - Wokwi web  : salin isi berkas ini ke sketch.ino dan isi diagram.json
 *                   ke tab diagram.json, klik ▶, lalu klik dan tahan tombol.
 *    - Board asli : rangkai di breadboard, Build (✓) → Upload (→), lalu
 *                   buka Serial Monitor (115200).
 *  Hasil yang diharapkan:
 *    - Tombol dilepas : LED padam,   "[DATA] digitalRead = 1 (HIGH) -> ..."
 *    - Tombol ditekan : LED menyala, "[DATA] digitalRead = 0 (LOW)  -> ..."
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_TOMBOL = 4;      // Tombol ke GND, memakai pull-up internal
const int PIN_LED = 23;        // LED eksternal lewat resistor 220 Ω
const int JEDA_BACA_MS = 200;  // Jeda antarbacaan agar Serial Monitor tidak banjir

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TOMBOL, INPUT_PULLUP);  // Aktifkan resistor pull-up internal (±45 kΩ)
  pinMode(PIN_LED, OUTPUT);
  Serial.println("[INFO] Praktikum 1: tombol di GPIO 4, LED di GPIO 23.");
  Serial.println("[INFO] Tekan dan tahan tombol, lalu amati nilai digitalRead.");
}

void loop() {
  int nilai = digitalRead(PIN_TOMBOL);  // Hasilnya 1 (HIGH) atau 0 (LOW)

  Serial.print("[DATA] digitalRead = ");
  Serial.print(nilai);

  // Logika terbalik akibat pull-up: ditekan = LOW, dilepas = HIGH
  if (nilai == LOW) {
    digitalWrite(PIN_LED, HIGH);
    Serial.println(" (LOW)  -> tombol DITEKAN, LED nyala");
  } else {
    digitalWrite(PIN_LED, LOW);
    Serial.println(" (HIGH) -> tombol dilepas, LED padam");
  }

  delay(JEDA_BACA_MS);
}
