/*
 * =====================================================================
 *  Pertemuan 03 · Praktikum 1 — LED Eksternal di GPIO 23
 * =====================================================================
 *  Tujuan   : Menyalakan LED yang kamu rangkai sendiri di breadboard dan
 *             mencetak statusnya di Serial Monitor.
 *  Rangkaian: GPIO 23 → resistor 220 Ω → kaki panjang (anoda) LED merah;
 *             kaki pendek (katode) LED → GND (pin GND di sisi kanan board).
 *  Cara menjalankan:
 *    - Board asli : rangkai dulu saat USB tercabut, colokkan USB, lalu
 *                   Build (✓) → Upload (→) → Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke tab sketch.ino dan isi diagram.json
 *                   ke tab diagram.json, lalu klik ▶.
 *  Hasil yang diharapkan:
 *    LED merah menyala 1 detik, padam 1 detik, terus berulang.
 *    Serial Monitor mencetak perkiraan arus LED, lalu status tiap kedipan.
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_LED = 23;            // LED eksternal lewat resistor 220 ohm
const int LAMA_NYALA_MS = 1000;    // lama LED menyala (milidetik)
const int LAMA_PADAM_MS = 1000;    // lama LED padam (milidetik)

// Angka untuk memperkirakan arus LED dengan hukum Ohm
const float TEGANGAN_PIN = 3.3;    // tegangan pin saat HIGH (volt)
const float VF_LED = 2.0;          // tegangan maju LED merah (volt, perkiraan)
const float RESISTOR_OHM = 220.0;  // nilai resistor (ohm)

unsigned long jumlahKedip = 0;     // sudah berapa kali LED menyala

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);        // GPIO 23 menjadi keluaran
  digitalWrite(PIN_LED, LOW);      // mulai dari keadaan padam

  // I = (3,3 V - Vf) / R, lalu dikali 1000 agar satuannya miliampere (mA)
  float arusMiliampere = (TEGANGAN_PIN - VF_LED) / RESISTOR_OHM * 1000.0;
  Serial.println("[INFO] Praktikum 1: LED eksternal di GPIO 23");
  Serial.print("[INFO] Perkiraan arus LED: ");
  Serial.print(arusMiliampere, 1); // tampilkan 1 angka di belakang titik
  Serial.println(" mA");
}

void loop() {
  digitalWrite(PIN_LED, HIGH);     // pin = 3,3 V → arus mengalir → LED menyala
  jumlahKedip++;
  Serial.print("[DATA] Kedip ke-");
  Serial.print(jumlahKedip);
  Serial.println(": LED NYALA (GPIO 23 = HIGH)");
  delay(LAMA_NYALA_MS);

  digitalWrite(PIN_LED, LOW);      // pin = 0 V → arus berhenti → LED padam
  Serial.print("[DATA] Kedip ke-");
  Serial.print(jumlahKedip);
  Serial.println(": LED PADAM (GPIO 23 = LOW)");
  delay(LAMA_PADAM_MS);
}
