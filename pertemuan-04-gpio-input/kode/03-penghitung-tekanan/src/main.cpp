/*
 * =====================================================================
 *  Pertemuan 04 · Praktikum 3 — Penghitung Tekanan Tombol
 * =====================================================================
 *  Tujuan   : Menghitung berapa kali tombol HITUNG ditekan dan menampilkan
 *             hasilnya di Serial Monitor. Tombol RESET mengembalikan
 *             hitungan ke nol. Kedua tombol memakai debounce + deteksi tepi.
 *  Rangkaian: - Tombol HITUNG: GPIO 4 ke GND (INPUT_PULLUP).
 *             - Tombol RESET : GPIO 13 ke GND (INPUT_PULLUP).
 *             - LED + resistor 220 Ω di GPIO 23: menyala selama tombol
 *               HITUNG ditekan (menurut keadaan yang sudah di-debounce).
 *  Cara menjalankan:
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan diagram.json ke tab
 *                   diagram.json, klik ▶, lalu klik kedua tombol.
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200).
 *  Hasil yang diharapkan:
 *    [DATA] Jumlah tekanan: 1
 *    [DATA] Jumlah tekanan: 2
 *    [INFO] Penghitung di-reset ke 0.
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_HITUNG = 4;   // Tombol utama
const int PIN_RESET = 13;   // Tombol kedua
const int PIN_LED = 23;
const unsigned long WAKTU_DEBOUNCE_MS = 50;

// Data tiap tombol disimpan dalam larik: indeks 0 = HITUNG, indeks 1 = RESET
const int JUMLAH_TOMBOL = 2;
const int TOMBOL_HITUNG = 0;
const int TOMBOL_RESET = 1;
const int PIN_TOMBOL[JUMLAH_TOMBOL] = {PIN_HITUNG, PIN_RESET};
int bacaanTerakhir[JUMLAH_TOMBOL] = {HIGH, HIGH};
int keadaanStabil[JUMLAH_TOMBOL] = {HIGH, HIGH};
unsigned long waktuBerubah[JUMLAH_TOMBOL] = {0, 0};

unsigned long jumlahTekanan = 0;

bool baruDitekan(int i);  // Deklarasi fungsi (isinya di bawah loop)

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < JUMLAH_TOMBOL; i++) {
    pinMode(PIN_TOMBOL[i], INPUT_PULLUP);
  }
  pinMode(PIN_LED, OUTPUT);
  Serial.println("[INFO] Praktikum 3: penghitung tekanan tombol.");
  Serial.println("[INFO] HITUNG = GPIO 4, RESET = GPIO 13.");
}

void loop() {
  if (baruDitekan(TOMBOL_HITUNG)) {
    jumlahTekanan++;
    Serial.print("[DATA] Jumlah tekanan: ");
    Serial.println(jumlahTekanan);
  }

  if (baruDitekan(TOMBOL_RESET)) {
    jumlahTekanan = 0;
    Serial.println("[INFO] Penghitung di-reset ke 0.");
  }

  // LED mengikuti keadaan stabil tombol HITUNG (pull-up: ditekan = LOW)
  digitalWrite(PIN_LED, keadaanStabil[TOMBOL_HITUNG] == LOW ? HIGH : LOW);

  // TANTANGAN (opsional): deteksi tekan lama pada tombol HITUNG.
  // Petunjuknya ada di README, bagian Praktikum 3.
}

// Mengembalikan true SEKALI untuk setiap tekanan sah pada tombol ke-i
// (debounce berbasis millis() + deteksi tepi turun HIGH → LOW).
bool baruDitekan(int i) {
  bool hasil = false;
  int bacaan = digitalRead(PIN_TOMBOL[i]);

  if (bacaan != bacaanTerakhir[i]) {
    waktuBerubah[i] = millis();  // Masih memantul → stopwatch diulang
  }
  if (millis() - waktuBerubah[i] > WAKTU_DEBOUNCE_MS && bacaan != keadaanStabil[i]) {
    keadaanStabil[i] = bacaan;   // Perubahan keadaan yang sah
    if (keadaanStabil[i] == LOW) {
      hasil = true;              // Tepi turun = baru saja ditekan
    }
  }
  bacaanTerakhir[i] = bacaan;
  return hasil;
}
