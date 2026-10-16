/*
 * =====================================================================
 *  Pertemuan 07 · Praktikum 5 — Relay sebagai Sakelar Beban
 * =====================================================================
 *  Tujuan   : Menyalakan dan memadamkan beban lewat relay yang dikendalikan
 *             GPIO, karena pin ESP32 tidak kuat menggerakkan kumparan relay.
 *  Rangkaian: GPIO 33 → IN modul relay; VCC modul → VIN (5 V); GND → GND.
 *             Wokwi: COM → 3V3, NO → 220 Ω → LED hijau → GND,
 *                    NC → 220 Ω → LED merah → GND.
 *             Board asli tanpa modul: GPIO 33 → 680 Ω → basis 2N2222A,
 *             emitor → GND, kolektor → kumparan relay → 5 V,
 *             dioda 1N4007 paralel kumparan (garis/katode ke 5 V).
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → buka Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan diagram.json ke tab
 *                   diagram.json, lalu klik ▶.
 *  Hasil yang diharapkan:
 *    Relay berganti NYALA/PADAM setiap 2 detik (LED hijau dan merah
 *    menyala bergantian di Wokwi). Serial Monitor mencatat setiap siklus.
 *  PERINGATAN: jangan menyambungkan relay ke listrik 220 V tanpa
 *    pengawasan dosen/teknisi.
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_RELAY = 33;
// Wokwi (atribut "transistor": "npn") dan rangkaian transistor NPN: aktif HIGH.
// Banyak modul relay di pasaran aktif LOW (relay menyala saat IN = LOW):
// untuk modul seperti itu, ubah menjadi true.
const bool RELAY_AKTIF_LOW = false;
const int LAMA_NYALA_MS = 2000;
const int LAMA_PADAM_MS = 2000;

int jumlahSiklus = 0;

void aturRelay(bool nyala);

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RELAY, OUTPUT);
  aturRelay(false);               // mulai dari keadaan aman: relay padam
  Serial.printf("[INFO] Relay di GPIO 33, aktif %s\n", RELAY_AKTIF_LOW ? "LOW" : "HIGH");
}

void loop() {
  jumlahSiklus++;
  aturRelay(true);
  Serial.printf("[INFO] Siklus %d: relay NYALA -> COM-NO tersambung (beban menyala)\n", jumlahSiklus);
  delay(LAMA_NYALA_MS);

  aturRelay(false);
  Serial.printf("[INFO] Siklus %d: relay PADAM -> COM-NC tersambung (beban padam)\n", jumlahSiklus);
  delay(LAMA_PADAM_MS);
}

// Satu tempat untuk mengurus aktif HIGH/LOW, jadi loop() cukup berpikir "nyala/padam".
void aturRelay(bool nyala) {
  if (RELAY_AKTIF_LOW) {
    digitalWrite(PIN_RELAY, nyala ? LOW : HIGH);
  } else {
    digitalWrite(PIN_RELAY, nyala ? HIGH : LOW);
  }
}
