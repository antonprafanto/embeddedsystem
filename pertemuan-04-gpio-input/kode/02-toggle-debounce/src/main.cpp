/*
 * =====================================================================
 *  Pertemuan 04 · Praktikum 2 — Toggle LED dengan Debounce
 * =====================================================================
 *  Tujuan   : Sekali tekan = LED berganti keadaan (toggle). Langkah 1
 *             memperlihatkan masalah pantulan (bouncing) tanpa debounce;
 *             Langkah 2 mengatasinya dengan debounce berbasis millis()
 *             dan deteksi tepi.
 *  Rangkaian: Sama dengan Praktikum 1 — tombol GPIO 4 ke GND (INPUT_PULLUP),
 *             LED + resistor 220 Ω di GPIO 23.
 *  Cara menjalankan:
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan diagram.json ke tab
 *                   diagram.json, klik ▶, lalu klik tombol beberapa kali.
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200).
 *  Langkah:
 *    1. Jalankan dengan PAKAI_DEBOUNCE = false → hitungan sering meloncat.
 *    2. Ubah menjadi PAKAI_DEBOUNCE = true → satu tekanan = satu hitungan.
 *  Hasil yang diharapkan (Langkah 2):
 *    [DATA] Tekanan ke-1 -> LED NYALA
 *    [DATA] Tekanan ke-2 -> LED PADAM
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_TOMBOL = 4;
const int PIN_LED = 23;

// Langkah 1: biarkan false. Langkah 2: ubah menjadi true.
const bool PAKAI_DEBOUNCE = false;
const unsigned long WAKTU_DEBOUNCE_MS = 50;  // Bacaan harus stabil lebih dari 50 ms

int bacaanTerakhir = HIGH;       // Bacaan mentah pada putaran loop() sebelumnya
int keadaanStabil = HIGH;        // Keadaan tombol yang sudah lolos debounce
unsigned long waktuBerubah = 0;  // "Stopwatch": kapan bacaan mentah terakhir berubah
bool ledMenyala = false;
int jumlahTekan = 0;

// Deklarasi fungsi (isinya ada di bawah loop)
void periksaTanpaDebounce(int bacaan);
void periksaDenganDebounce(int bacaan);
void gantiLed();

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TOMBOL, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  if (PAKAI_DEBOUNCE) {
    Serial.print("[INFO] Praktikum 2: toggle LED, debounce AKTIF (");
    Serial.print(WAKTU_DEBOUNCE_MS);
    Serial.println(" ms).");
  } else {
    Serial.println("[INFO] Praktikum 2: toggle LED, debounce NONAKTIF.");
  }
}

void loop() {
  int bacaan = digitalRead(PIN_TOMBOL);  // Bacaan mentah, bisa ikut memantul

  if (PAKAI_DEBOUNCE) {
    periksaDenganDebounce(bacaan);
  } else {
    periksaTanpaDebounce(bacaan);
  }

  bacaanTerakhir = bacaan;  // Simpan untuk dibandingkan pada putaran berikutnya
}

// Versi naif: setiap perubahan HIGH → LOW dianggap satu tekanan.
// Pantulan menghasilkan beberapa perubahan sekaligus, jadi hitungan meloncat.
void periksaTanpaDebounce(int bacaan) {
  if (bacaanTerakhir == HIGH && bacaan == LOW) {
    gantiLed();
  }
}

// Versi debounce: perubahan baru dipercaya setelah bacaan stabil > 50 ms.
void periksaDenganDebounce(int bacaan) {
  if (bacaan != bacaanTerakhir) {
    waktuBerubah = millis();  // Masih memantul → stopwatch diulang dari nol
  }

  if (millis() - waktuBerubah > WAKTU_DEBOUNCE_MS) {
    if (bacaan != keadaanStabil) {  // Ada perubahan keadaan yang sah
      keadaanStabil = bacaan;
      if (keadaanStabil == LOW) {   // Tepi turun = tombol baru saja ditekan
        gantiLed();
      }
    }
  }
}

void gantiLed() {
  ledMenyala = !ledMenyala;
  digitalWrite(PIN_LED, ledMenyala ? HIGH : LOW);
  jumlahTekan++;
  Serial.print("[DATA] Tekanan ke-");
  Serial.print(jumlahTekan);
  Serial.print(" -> LED ");
  Serial.println(ledMenyala ? "NYALA" : "PADAM");
}
