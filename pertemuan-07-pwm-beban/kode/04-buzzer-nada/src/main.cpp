/*
 * =====================================================================
 *  Pertemuan 07 · Praktikum 4 — Memainkan Nada dengan Buzzer
 * =====================================================================
 *  Tujuan   : Membuat nada dengan buzzer pasif: frekuensi PWM menentukan
 *             tinggi nada, lama bunyi menentukan ketukan.
 *  Rangkaian: Buzzer pasif: kaki + → GPIO 18, kaki − → GND.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → buka Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan diagram.json ke tab
 *                   diagram.json, klik ▶ (nyalakan suara peramban).
 *  Hasil yang diharapkan:
 *    Terdengar tangga nada do–re–mi (C4–C5), lalu potongan melodi
 *    "Ode to Joy". Nama dan frekuensi nada tampil di Serial Monitor.
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_BUZZER = 18;

// Frekuensi nada dalam Hz, dibulatkan dari rumus f = 440 × 2^(n/12)
const int NADA_C4 = 262;
const int NADA_D4 = 294;
const int NADA_E4 = 330;
const int NADA_F4 = 349;
const int NADA_G4 = 392;
const int NADA_A4 = 440;
const int NADA_B4 = 494;
const int NADA_C5 = 523;
const int DIAM = 0;               // frekuensi 0 = buzzer diam

const int KETUK_MS = 500;         // tempo 120 ketuk per menit → 1 ketuk = 500 ms

// Tangga nada C mayor
const int TANGGA[] = {NADA_C4, NADA_D4, NADA_E4, NADA_F4, NADA_G4, NADA_A4, NADA_B4, NADA_C5};
const char *NAMA_TANGGA[] = {"C4 (do)", "D4 (re)", "E4 (mi)", "F4 (fa)",
                             "G4 (sol)", "A4 (la)", "B4 (si)", "C5 (do')"};
const int JUMLAH_TANGGA = 8;

// "Ode to Joy", Beethoven (Simfoni No. 9, 1824) — sudah domain publik.
// Durasi dalam satuan setengah ketuk: 2 = 1 ketuk, 3 = 1,5 ketuk, 1 = ½ ketuk, 4 = 2 ketuk.
const int MELODI[] = {
  NADA_E4, NADA_E4, NADA_F4, NADA_G4, NADA_G4, NADA_F4, NADA_E4, NADA_D4,
  NADA_C4, NADA_C4, NADA_D4, NADA_E4, NADA_E4, NADA_D4, NADA_D4,
  NADA_E4, NADA_E4, NADA_F4, NADA_G4, NADA_G4, NADA_F4, NADA_E4, NADA_D4,
  NADA_C4, NADA_C4, NADA_D4, NADA_E4, NADA_D4, NADA_C4, NADA_C4};
const int DURASI[] = {
  2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 3, 1, 4,
  2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 3, 1, 4};
const int JUMLAH_MELODI = sizeof(MELODI) / sizeof(MELODI[0]);

void mainkanNada(int frekuensi, int lamaMs);

void setup() {
  Serial.begin(115200);
  // Pasang pin ke LEDC. Frekuensi awal bebas; ledcWriteTone() akan menggantinya.
  ledcAttach(PIN_BUZZER, 1000, 10);
  Serial.println("[INFO] Buzzer pasif siap di GPIO 18");
}

void loop() {
  Serial.println("[INFO] Tangga nada C mayor:");
  for (int i = 0; i < JUMLAH_TANGGA; i++) {
    Serial.printf("[NADA] %-8s %3d Hz\n", NAMA_TANGGA[i], TANGGA[i]);
    mainkanNada(TANGGA[i], KETUK_MS);
  }
  delay(1000);

  Serial.println("[INFO] Melodi: Ode to Joy (Beethoven, domain publik)");
  for (int i = 0; i < JUMLAH_MELODI; i++) {
    mainkanNada(MELODI[i], DURASI[i] * KETUK_MS / 2);
  }
  Serial.println("[INFO] Selesai. Diulang 3 detik lagi.");
  delay(3000);
}

// Bunyikan 90% durasi, lalu diam 10% agar dua nada yang sama terdengar terpisah.
void mainkanNada(int frekuensi, int lamaMs) {
  int lamaBunyi = lamaMs * 9 / 10;
  ledcWriteTone(PIN_BUZZER, frekuensi);   // gelombang kotak 50% pada frekuensi nada
  delay(lamaBunyi);
  ledcWriteTone(PIN_BUZZER, DIAM);
  delay(lamaMs - lamaBunyi);
  // Jalan pintas lain: tone(PIN_BUZZER, frekuensi, lamaBunyi); — jangan dicampur
  // dengan ledcAttach() pada pin yang sama.
}
