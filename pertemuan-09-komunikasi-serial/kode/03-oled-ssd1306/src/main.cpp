/*
 * =====================================================================
 *  Pertemuan 09 · Praktikum 3 — Menampilkan Teks di OLED SSD1306
 * =====================================================================
 *  Tujuan   : Menulis salam, angka penghitung detik, dan bar sederhana
 *             di layar OLED 128×64 piksel yang tersambung lewat I2C.
 *  Rangkaian: OLED SSD1306 (alamat 0x3C):
 *               GND → GND, VCC → 3V3, SCL → GPIO 22, SDA → GPIO 21.
 *  Library  : Adafruit SSD1306 dan Adafruit GFX Library
 *             (PlatformIO: lib_deps · Wokwi: libraries.txt/Library Manager).
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → amati layar OLED.
 *    - Wokwi web  : salin berkas ini ke sketch.ino, isi diagram.json, tambah
 *                   library dari libraries.txt, lalu klik ▶.
 *  Hasil yang diharapkan:
 *    OLED menampilkan "Halo, ESP32!", hitungan detik yang naik setiap
 *    detik, dan bar yang bertambah 10% per detik (penuh tiap 10 detik).
 *    Serial Monitor: [DATA] detik = 0 | bar = 10%  (dan seterusnya)
 * =====================================================================
 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int PIN_SDA = 21;
const int PIN_SCL = 22;
const int LEBAR_LAYAR = 128;          // piksel
const int TINGGI_LAYAR = 64;          // piksel
const uint8_t ALAMAT_OLED = 0x3C;     // alamat I2C OLED (lihat Praktikum 2)

// Objek layar: ukuran, bus I2C yang dipakai, pin reset (-1 = tidak ada)
Adafruit_SSD1306 oled(LEBAR_LAYAR, TINGGI_LAYAR, &Wire, -1);

unsigned long waktuMulai = 0;         // millis() saat setup() selesai
long detikTerakhir = -1;              // detik yang terakhir digambar

void gambarLayar(unsigned long detik);

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(PIN_SDA, PIN_SCL);

  // begin() mengirim urutan perintah inisialisasi ke cip SSD1306.
  if (!oled.begin(SSD1306_SWITCHCAPVCC, ALAMAT_OLED)) {
    Serial.println("[GALAT] OLED tidak ditemukan di alamat 0x3C. Periksa kabel!");
    while (true) {
      delay(1000);                    // berhenti di sini; tidak ada gunanya lanjut
    }
  }
  Serial.println("[INFO] OLED siap di alamat 0x3C.");
  oled.setTextColor(SSD1306_WHITE);   // piksel menyala = putih/biru
  waktuMulai = millis();
}

void loop() {
  unsigned long detik = (millis() - waktuMulai) / 1000;
  if ((long)detik != detikTerakhir) { // gambar ulang hanya saat detik berganti
    detikTerakhir = detik;
    gambarLayar(detik);
  }
}

void gambarLayar(unsigned long detik) {
  int persen = (detik % 10 + 1) * 10;          // 10, 20, ..., 100, lalu ulang
  int lebarIsi = map(persen, 0, 100, 0, 96);   // ubah persen → piksel (maks 96)

  oled.clearDisplay();                         // 1) kosongkan buffer di RAM ESP32

  oled.setTextSize(1);                         // huruf 6×8 piksel
  oled.setCursor(0, 0);                        // (x, y) dari pojok kiri atas
  oled.print("Halo, ESP32!");
  oled.setCursor(0, 10);
  oled.print("Pertemuan 09: OLED");
  oled.drawFastHLine(0, 20, LEBAR_LAYAR, SSD1306_WHITE);  // garis pemisah

  oled.setTextSize(2);                         // huruf 12×16 piksel
  oled.setCursor(0, 26);
  oled.print("Detik:");
  oled.print(detik);

  oled.drawRect(0, 50, 100, 12, SSD1306_WHITE);        // bingkai bar
  oled.fillRect(2, 52, lebarIsi, 8, SSD1306_WHITE);    // isi bar
  oled.setTextSize(1);
  oled.setCursor(104, 52);
  oled.print(persen);
  oled.print("%");

  oled.display();                              // 2) kirim seluruh buffer ke layar

  Serial.printf("[DATA] detik = %lu | bar = %d%%\n", detik, persen);
}
