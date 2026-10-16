/*
 * =====================================================================
 *  Pertemuan 09 · Praktikum 2 — Pemindai I2C
 * =====================================================================
 *  Tujuan   : Mencari semua perangkat yang tersambung ke bus I2C dan
 *             menampilkan alamatnya dalam bentuk tabel (mirip perintah
 *             i2cdetect di Linux).
 *  Rangkaian: OLED SSD1306 dan MPU6050 berbagi dua kabel bus yang sama:
 *               SDA → GPIO 21, SCL → GPIO 22, VCC → 3V3, GND → GND.
 *             Pull-up SDA/SCL umumnya sudah ada di modul (di Wokwi tidak perlu).
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor.
 *    - Wokwi web  : salin isi berkas ini ke sketch.ino dan diagram.json,
 *                   lalu klik ▶.
 *  Hasil yang diharapkan (Serial Monitor, 115200 baud):
 *    Tabel alamat dengan isian 3C dan 68, lalu:
 *    Ditemukan 2 perangkat:
 *      - 0x3C : OLED SSD1306
 *      - 0x68 : MPU6050 (AD0 = LOW) atau RTC DS1307/DS3231
 * =====================================================================
 */
#include <Arduino.h>
#include <Wire.h>

const int PIN_SDA = 21;                      // jalur data I2C
const int PIN_SCL = 22;                      // jalur clock I2C
const unsigned long JEDA_PINDAI_MS = 5000;   // pindai ulang setiap 5 detik

// Alamat 0x00–0x07 dan 0x78–0x7F dicadangkan oleh spesifikasi I2C,
// jadi kita hanya memindai 0x08–0x77.
const uint8_t ALAMAT_AWAL = 0x08;
const uint8_t ALAMAT_AKHIR = 0x77;

int pindaiBus();
const char *tebakPerangkat(uint8_t alamat);

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(PIN_SDA, PIN_SCL);              // ESP32 menjadi controller, 100 kHz
  Serial.println();
  Serial.println("=== Pemindai I2C (SDA = GPIO 21, SCL = GPIO 22) ===");
}

void loop() {
  pindaiBus();
  Serial.println("Pindai ulang dalam 5 detik ...");
  delay(JEDA_PINDAI_MS);
}

// Mengetuk setiap alamat; perangkat yang ada akan menjawab dengan ACK.
// Mengembalikan jumlah perangkat yang ditemukan.
int pindaiBus() {
  uint8_t ditemukan[112];                    // paling banyak 112 alamat
  int jumlah = 0;

  Serial.println();
  Serial.println("Memindai alamat 0x08 sampai 0x77 ...");
  Serial.println("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
  for (int baris = 0; baris < 8; baris++) {
    Serial.printf("%02X:", baris * 16);      // label baris: 00, 10, ..., 70
    for (int kolom = 0; kolom < 16; kolom++) {
      uint8_t alamat = baris * 16 + kolom;
      if (alamat > ALAMAT_AKHIR) {
        break;                               // 0x78–0x7F tidak dipindai
      }
      if (alamat < ALAMAT_AWAL) {
        Serial.print("   ");                 // 0x00–0x07 tidak dipindai
        continue;
      }
      Wire.beginTransmission(alamat);        // START + alamat + bit tulis
      uint8_t hasil = Wire.endTransmission();// STOP; 0 = ada yang menjawab ACK
      if (hasil == 0) {
        Serial.printf(" %02X", alamat);
        ditemukan[jumlah] = alamat;
        jumlah++;
      } else {
        Serial.print(" --");                 // tidak ada jawaban (NACK)
      }
    }
    Serial.println();
  }

  if (jumlah == 0) {
    Serial.println("Tidak ada perangkat. Periksa kabel SDA, SCL, VCC, dan GND.");
  } else {
    Serial.printf("Ditemukan %d perangkat:\n", jumlah);
    for (int i = 0; i < jumlah; i++) {
      Serial.printf("  - 0x%02X : %s\n", ditemukan[i], tebakPerangkat(ditemukan[i]));
    }
  }
  return jumlah;
}

// Tebakan nama perangkat dari alamat yang umum (lihat lembar data masing-masing).
const char *tebakPerangkat(uint8_t alamat) {
  switch (alamat) {
    case 0x27: return "modul LCD I2C (PCF8574)";
    case 0x3C: return "OLED SSD1306";
    case 0x3D: return "OLED SSD1306 (alamat alternatif)";
    case 0x68: return "MPU6050 (AD0 = LOW) atau RTC DS1307/DS3231";
    case 0x69: return "MPU6050 (AD0 = HIGH)";
    case 0x76:
    case 0x77: return "sensor tekanan BMP280/BME280";
    default:   return "perangkat lain (cek lembar datanya)";
  }
}
