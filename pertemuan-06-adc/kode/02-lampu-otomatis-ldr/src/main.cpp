/*
 * =====================================================================
 *  Pertemuan 06 · Praktikum 2 — Lampu Otomatis dengan LDR dan Histeresis
 * =====================================================================
 *  Tujuan   : Membaca modul sensor cahaya (LDR), mengubah nilai ADC menjadi
 *             perkiraan lux, lalu menyalakan "lampu taman" saat gelap.
 *             Dua ambang (histeresis) mencegah lampu berkedip di batas.
 *  Rangkaian: Modul wokwi-photoresistor-sensor: VCC → 3V3, GND → GND,
 *             AO → GPIO 35. LED: GPIO 23 → resistor 220 Ω → anoda LED,
 *             katoda LED → GND.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200),
 *                   lalu tutup/buka LDR dengan tangan.
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan isi diagram.json,
 *                   klik ▶, lalu klik modul LDR dan geser nilai lux-nya.
 *  Hasil yang diharapkan:
 *    Setiap 1 detik muncul baris [DATA]. Lampu NYALA saat cahaya < 50 lux
 *    dan baru PADAM lagi saat cahaya > 100 lux. Di antara 50–100 lux,
 *    keadaan lampu tidak berubah.
 * =====================================================================
 */
#include <Arduino.h>
#include <math.h>   // pow()

const int PIN_LDR = 35;              // AO modul LDR → GPIO 35 (ADC1_CH7)
const int PIN_LAMPU = 23;            // LED "lampu taman" → GPIO 23
const int NILAI_MAKS = 4095;         // ADC 12 bit
const float TEGANGAN_PENUH = 3.3;    // Modul diberi catu 3,3 V
const float R_SERI = 10000.0;        // Resistor tetap di dalam modul (10 kΩ)
const float RL10 = 50.0;             // Hambatan LDR pada 10 lux, kΩ (atribut rl10 Wokwi)
const float GAMMA = 0.7;             // Kemiringan kurva LDR (atribut gamma Wokwi)
const float LUX_NYALA = 50.0;        // Lebih gelap dari ini → lampu menyala
const float LUX_PADAM = 100.0;       // Lebih terang dari ini → lampu padam
const unsigned long JEDA_CETAK_MS = 1000;

bool lampuMenyala = false;
unsigned long waktuCetakTerakhir = 0;

float hitungLux(int mentah);

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LAMPU, OUTPUT);
  digitalWrite(PIN_LAMPU, LOW);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  Serial.println("Lampu otomatis siap. Ambang: nyala < 50 lux, padam > 100 lux.");
}

void loop() {
  int mentah = analogRead(PIN_LDR);
  float lux = hitungLux(mentah);

  // Histeresis: ambang untuk MENYALAKAN berbeda dengan ambang untuk MEMADAMKAN
  if (!lampuMenyala && lux < LUX_NYALA) {
    lampuMenyala = true;
    digitalWrite(PIN_LAMPU, HIGH);
    Serial.println("[INFO] Gelap  -> lampu NYALA");
  } else if (lampuMenyala && lux > LUX_PADAM) {
    lampuMenyala = false;
    digitalWrite(PIN_LAMPU, LOW);
    Serial.println("[INFO] Terang -> lampu PADAM");
  }

  // Cetak data tiap 1 detik tanpa menghentikan pengecekan (pola millis())
  if (millis() - waktuCetakTerakhir >= JEDA_CETAK_MS) {
    waktuCetakTerakhir = millis();
    float tegangan = mentah * TEGANGAN_PENUH / NILAI_MAKS;
    Serial.printf("[DATA] mentah: %4d | AO: %.3f V | cahaya: %8.1f lux | lampu: %s\n",
                  mentah, tegangan, lux, lampuMenyala ? "NYALA" : "PADAM");
  }
  delay(50);
}

// Ubah nilai mentah menjadi perkiraan lux (model LDR Wokwi, catu 3,3 V)
float hitungLux(int mentah) {
  if (mentah <= 0) return 100000.0;      // Sangat terang: dibatasi agar tidak "inf"
  if (mentah >= NILAI_MAKS) return 0.0;  // Sangat gelap
  // Pembagi tegangan: AO = VCC × R_LDR / (R_SERI + R_LDR)
  // → R_LDR = R_SERI × mentah / (4095 − mentah)
  float hambatanLdr = R_SERI * mentah / (NILAI_MAKS - mentah);
  // Model LDR: R = RL10 × (lux / 10)^(−gamma) → lux = 10 × (RL10 / R)^(1 / gamma)
  return 10.0 * pow(RL10 * 1000.0 / hambatanLdr, 1.0 / GAMMA);
}
