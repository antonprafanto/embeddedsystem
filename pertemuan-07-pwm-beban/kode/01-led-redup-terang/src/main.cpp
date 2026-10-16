/*
 * =====================================================================
 *  Pertemuan 07 · Praktikum 1 — LED Redup-Terang dengan PWM
 * =====================================================================
 *  Tujuan   : Mengatur kecerahan LED dengan PWM (periferal LEDC ESP32):
 *             LED perlahan makin terang, lalu perlahan meredup.
 *  Rangkaian: GPIO 23 → resistor 220 Ω → anode LED (kaki panjang);
 *             katode LED (kaki pendek) → GND.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → buka Serial Monitor.
 *    - Wokwi web  : salin isi berkas ini ke sketch.ino dan isi diagram.json
 *                   ke tab diagram.json, lalu klik ▶.
 *  Hasil yang diharapkan:
 *    LED berangsur terang (±1 detik) lalu berangsur redup (±1 detik),
 *    terus berulang. Serial Monitor menampilkan nilai duty dan tegangan
 *    rata-rata setiap kelipatan 20%.
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_LED = 23;           // LED eksternal
const int FREKUENSI_PWM = 5000;   // 5 kHz: kedipan terlalu cepat untuk terlihat mata
const int RESOLUSI_PWM = 8;       // 8 bit → nilai duty 0–255
const int DUTY_MAKS = 255;        // 2^8 − 1
const int JEDA_LANGKAH_MS = 4;    // 255 langkah × 4 ms ≈ 1 detik per arah

void tampilkanDuty(int duty);

void setup() {
  Serial.begin(115200);
  // ledcAttach() memasang pin ke salah satu kanal LEDC yang masih bebas.
  // Nilai kembaliannya false bila frekuensi/resolusi tidak bisa dipenuhi.
  if (!ledcAttach(PIN_LED, FREKUENSI_PWM, RESOLUSI_PWM)) {
    Serial.println("[PERINGATAN] ledcAttach gagal. Cek pin, frekuensi, dan resolusi.");
    while (true) {
      delay(1000);
    }
  }
  Serial.println("[INFO] PWM siap: GPIO 23, 5000 Hz, resolusi 8 bit (duty 0-255)");
}

void loop() {
  Serial.println("[INFO] Redup -> terang");
  for (int duty = 0; duty <= DUTY_MAKS; duty++) {
    ledcWrite(PIN_LED, duty);     // jalan pintas setara: analogWrite(PIN_LED, duty)
    tampilkanDuty(duty);
    delay(JEDA_LANGKAH_MS);
  }

  Serial.println("[INFO] Terang -> redup");
  for (int duty = DUTY_MAKS; duty >= 0; duty--) {
    ledcWrite(PIN_LED, duty);
    tampilkanDuty(duty);
    delay(JEDA_LANGKAH_MS);
  }
}

// Cetak hanya pada kelipatan 51 (20% dari 255) agar Serial Monitor tidak banjir.
void tampilkanDuty(int duty) {
  if (duty % 51 != 0) {
    return;
  }
  float persen = duty * 100.0 / DUTY_MAKS;
  float teganganRata = duty * 3.3 / DUTY_MAKS;   // V rata-rata = D × 3,3 V
  Serial.printf("[DATA] duty = %3d/255 (%3.0f%%) -> V rata-rata = %.2f V\n",
                duty, persen, teganganRata);
}
