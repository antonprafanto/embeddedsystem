/*
 * =====================================================================
 *  Pertemuan 10 · Praktikum 2 — Sensor Jarak Ultrasonik HC-SR04
 * =====================================================================
 *  Tujuan   : Mengukur jarak dengan HC-SR04 persis seperti petunjuk
 *             datasheet: pulsa trigger 10 µs, ukur lebar pulsa ECHO dengan
 *             pulseIn() (memakai batas waktu), lalu jarak (cm) = µs / 58.
 *  Rangkaian:
 *    HC-SR04 : VCC → VIN (5 V) · TRIG → GPIO 17 (TX2) · GND → GND
 *              ECHO → GPIO 16 (RX2). Board asli: ECHO bertegangan 5 V,
 *              jadi lewatkan pembagi tegangan 1 kΩ (seri) + 2 kΩ (ke GND).
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan isi diagram.json,
 *                   klik ▶, lalu klik sensor untuk menggeser nilai jarak.
 *  Hasil yang diharapkan:
 *    Tiap 0,5 detik Serial Monitor menampilkan durasi ECHO (µs) dan jarak
 *    (cm), atau "di luar jangkauan" bila tidak ada pantulan yang sah.
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_TRIG = 17;                       // keluaran: pulsa pemicu
const int PIN_ECHO = 16;                       // masukan: pulsa gema

const unsigned long INTERVAL_UKUR_MS = 500;    // datasheet: siklus ukur sebaiknya > 60 ms
const unsigned long BATAS_TUNGGU_US = 30000;   // 30 ms ≈ 517 cm, lebih jauh dari batas 4 m
const float JARAK_MIN_CM = 2.0;                // jangkauan datasheet: 2 cm – 4 m
const float JARAK_MAKS_CM = 400.0;

unsigned long waktuUkurTerakhir = 0;

unsigned long ukurDurasiEcho();

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  Serial.println("[INFO] Sensor jarak HC-SR04 siap. Mengukur tiap 500 ms.");
}

void loop() {
  if (millis() - waktuUkurTerakhir >= INTERVAL_UKUR_MS) {
    waktuUkurTerakhir = millis();

    unsigned long durasiUs = ukurDurasiEcho();
    float jarakCm = durasiUs / 58.0;           // rumus datasheet: µs / 58 = cm

    Serial.print("[DATA] Durasi ECHO: ");
    Serial.print(durasiUs);
    Serial.print(" us | Jarak: ");
    // durasi 0 berarti tidak ada pulsa sampai batas waktu habis
    if (durasiUs == 0 || jarakCm < JARAK_MIN_CM || jarakCm > JARAK_MAKS_CM) {
      Serial.println("di luar jangkauan");
    } else {
      Serial.print(jarakCm, 1);
      Serial.println(" cm");
    }
  }
}

unsigned long ukurDurasiEcho() {
  digitalWrite(PIN_TRIG, LOW);                 // mulai dari LOW yang bersih
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);                // pulsa trigger 10 µs (datasheet)
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  // Lama ECHO bernilai HIGH = waktu tempuh bunyi pergi + pulang.
  // pulseIn() mengembalikan 0 bila tidak ada pulsa utuh sebelum batas waktu.
  return pulseIn(PIN_ECHO, HIGH, BATAS_TUNGGU_US);
}
