/*
 * =====================================================================
 *  Pertemuan 10 · Praktikum 1 — Stasiun Cuaca Mini: DHT22 + OLED
 * =====================================================================
 *  Tujuan   : Membaca suhu dan kelembapan DHT22 memakai library, lalu
 *             menampilkannya di OLED SSD1306 dan Serial Monitor.
 *             Pembacaan dijadwalkan dengan millis() (tanpa delay) dan
 *             pembacaan gagal (NaN) ditangani dengan rapi.
 *  Rangkaian:
 *    DHT22 : VCC → 3V3 · DATA (SDA di Wokwi) → GPIO 14 · GND → GND
 *            Board asli: resistor pull-up 10 kΩ dari DATA ke 3V3
 *            (modul DHT22 3 pin biasanya sudah punya pull-up sendiri).
 *    OLED  : VCC → 3V3 · GND → GND · SDA → GPIO 21 · SCL → GPIO 22 (0x3C)
 *  Library  : DHT sensor library, Adafruit Unified Sensor, Adafruit SSD1306,
 *             Adafruit GFX Library (lihat platformio.ini dan libraries.txt).
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino, salin juga diagram.json
 *                   dan libraries.txt, lalu klik ▶.
 *  Hasil yang diharapkan:
 *    Tiap 2,5 detik suhu (°C) dan kelembapan (%) tampil di OLED dan Serial
 *    Monitor. Bila kabel data lepas, muncul [PERINGATAN] dan OLED menulis
 *    "Sensor gagal dibaca!".
 * =====================================================================
 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---------- Pin dan pengaturan ----------
const int PIN_DHT = 14;                        // jalur data DHT22
const int PIN_LED_DETAK = 2;                   // LED bawaan: tanda loop() tetap berjalan
const uint8_t ALAMAT_OLED = 0x3C;              // alamat I2C OLED 128×64
const int LEBAR_OLED = 128;
const int TINGGI_OLED = 64;

const unsigned long INTERVAL_BACA_MS = 2500;   // datasheet: periode baca rata-rata 2 s → beri cadangan
const unsigned long INTERVAL_DETAK_MS = 500;   // LED detak berkedip tiap 0,5 s

DHT dht(PIN_DHT, DHT22);                                    // objek sensor dari library
Adafruit_SSD1306 oled(LEBAR_OLED, TINGGI_OLED, &Wire, -1);  // -1 = OLED tanpa pin reset

bool oledSiap = false;
bool ledDetakNyala = false;
unsigned long waktuBacaTerakhir = 0;
unsigned long waktuDetakTerakhir = 0;
unsigned int jumlahGagal = 0;                  // jumlah gagal berturut-turut

// ---------- Deklarasi fungsi ----------
void bacaDanTampilkan();
void gambarJudul();
void tampilkanData(float suhu, float kelembapan);
void tampilkanGagal();

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED_DETAK, OUTPUT);
  dht.begin();
  Serial.println("[INFO] Stasiun cuaca mini: DHT22 + OLED");

  // begin() bernilai false bila tidak ada OLED yang menjawab di alamat 0x3C
  oledSiap = oled.begin(SSD1306_SWITCHCAPVCC, ALAMAT_OLED);
  if (oledSiap) {
    oled.cp437(true);                          // pakai tabel karakter CP437 (untuk simbol °)
    oled.setTextColor(SSD1306_WHITE);
    gambarJudul();
    oled.setCursor(0, 24);
    oled.print("Menunggu DHT22...");
    oled.display();
    Serial.println("[INFO] OLED siap di alamat 0x3C.");
  } else {
    Serial.println("[PERINGATAN] OLED tidak ditemukan. Data hanya tampil di Serial.");
  }
  Serial.print("[INFO] Membaca DHT22 tiap ");
  Serial.print(INTERVAL_BACA_MS);
  Serial.println(" ms.");
}

void loop() {
  unsigned long sekarang = millis();

  // Tugas 1: baca sensor tiap INTERVAL_BACA_MS (tanpa delay)
  if (sekarang - waktuBacaTerakhir >= INTERVAL_BACA_MS) {
    waktuBacaTerakhir = sekarang;
    bacaDanTampilkan();
  }

  // Tugas 2: LED detak — bukti loop() tidak "membeku" menunggu sensor
  if (sekarang - waktuDetakTerakhir >= INTERVAL_DETAK_MS) {
    waktuDetakTerakhir = sekarang;
    ledDetakNyala = !ledDetakNyala;
    digitalWrite(PIN_LED_DETAK, ledDetakNyala);
  }
}

void bacaDanTampilkan() {
  float kelembapan = dht.readHumidity();       // kelembapan relatif (% RH)
  float suhu = dht.readTemperature();          // derajat Celsius

  // Library mengembalikan NaN (Not a Number) bila komunikasi dengan sensor gagal
  if (isnan(suhu) || isnan(kelembapan)) {
    jumlahGagal++;
    Serial.print("[PERINGATAN] Gagal membaca DHT22 (");
    Serial.print(jumlahGagal);
    Serial.println("x berturut-turut). Cek kabel data dan pull-up.");
    tampilkanGagal();
    return;                                    // jangan tampilkan angka palsu
  }

  jumlahGagal = 0;
  Serial.print("[DATA] Suhu: ");
  Serial.print(suhu, 1);
  Serial.print(" °C | Kelembapan: ");
  Serial.print(kelembapan, 1);
  Serial.println(" %");
  tampilkanData(suhu, kelembapan);
}

void gambarJudul() {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print("Stasiun Cuaca Mini");
  oled.drawFastHLine(0, 11, LEBAR_OLED, SSD1306_WHITE);
}

void tampilkanData(float suhu, float kelembapan) {
  if (!oledSiap) return;
  gambarJudul();
  oled.setCursor(0, 22);
  oled.print("Suhu");
  oled.setTextSize(2);
  oled.setCursor(40, 18);
  oled.print(suhu, 1);
  oled.write(248);                             // 248 = simbol derajat (°) di CP437
  oled.print("C");

  oled.setTextSize(1);
  oled.setCursor(0, 48);
  oled.print("Lembap");
  oled.setTextSize(2);
  oled.setCursor(40, 44);
  oled.print(kelembapan, 1);
  oled.print("%");
  oled.display();
}

void tampilkanGagal() {
  if (!oledSiap) return;
  gambarJudul();
  oled.setCursor(0, 24);
  oled.print("Sensor gagal dibaca!");
  oled.setCursor(0, 40);
  oled.print("Cek kabel DHT22.");
  oled.display();
}
