/*
 * =====================================================================
 *  Pertemuan 09 · Praktikum 4 — Akselerometer MPU6050 + OLED
 * =====================================================================
 *  Tujuan   : Membaca akselerasi sumbu X, Y, Z (m/s²) dari MPU6050,
 *             menampilkannya di OLED beserta indikator kemiringan
 *             (gelembung waterpas), dan menyalakan LED bila terlalu miring.
 *  Rangkaian: OLED SSD1306 (0x3C) dan MPU6050 (0x68) di bus I2C yang sama:
 *               SDA → GPIO 21, SCL → GPIO 22, VCC → 3V3, GND → GND.
 *             LED bawaan GPIO 2 dipakai sebagai peringatan.
 *  Library  : Adafruit MPU6050, Adafruit Unified Sensor,
 *             Adafruit SSD1306, Adafruit GFX Library.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → miringkan modul MPU6050.
 *    - Wokwi web  : salin berkas ini ke sketch.ino, isi diagram.json, tambah
 *                   library, klik ▶, lalu klik MPU6050 untuk menggeser nilai.
 *  Hasil yang diharapkan (Wokwi, posisi bawaan datar):
 *    Serial: X:  0.00  Y:  0.00  Z:  9.81 m/s^2 | miring  0 derajat | DATAR
 *    OLED  : angka X/Y/Z, sudut miring, dan gelembung di tengah lingkaran.
 * =====================================================================
 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

const int PIN_SDA = 21;
const int PIN_SCL = 22;
const int PIN_LED = 2;                      // LED bawaan: peringatan terlalu miring
const float BATAS_DATAR = 5.0;              // derajat: di bawah ini dianggap datar
const float BATAS_AWAS = 45.0;              // derajat: di atas ini LED menyala
const unsigned long JEDA_BACA_MS = 100;     // baca sensor 10 kali per detik
const unsigned long JEDA_SERIAL_MS = 500;   // cetak ke Serial 2 kali per detik

// Pusat dan jari-jari lingkaran waterpas di layar (piksel)
const int PUSAT_X = 104;
const int PUSAT_Y = 38;
const int JARI_JARI = 22;
const int GESER_MAKS = 18;                  // gelembung paling jauh 18 piksel

Adafruit_SSD1306 oled(128, 64, &Wire, -1);
Adafruit_MPU6050 mpu;

unsigned long waktuBacaTerakhir = 0;
unsigned long waktuSerialTerakhir = 0;

float hitungSudutMiring(float ax, float ay, float az);
const char *statusKemiringan(float sudut);
void gambarLayar(float ax, float ay, float az, float sudut);
void berhentiDenganPesan(const char *pesan);

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  delay(500);
  Wire.begin(PIN_SDA, PIN_SCL);

  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    berhentiDenganPesan("[GALAT] OLED tidak ditemukan di alamat 0x3C. Periksa kabel!");
  }
  oled.setTextColor(SSD1306_WHITE);
  oled.cp437(true);                         // tabel karakter lengkap (ada simbol derajat)

  if (!mpu.begin(0x68)) {                   // memakai bus Wire yang sama
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.print("MPU6050 tidak ada!");
    oled.display();
    berhentiDenganPesan("[GALAT] MPU6050 tidak ditemukan di alamat 0x68. Periksa kabel!");
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);   // ±2 g: paling peka
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);     // saring getaran halus
  Serial.println("[INFO] OLED (0x3C) dan MPU6050 (0x68) siap.");
}

void loop() {
  unsigned long sekarang = millis();
  if (sekarang - waktuBacaTerakhir < JEDA_BACA_MS) {
    return;                                 // belum waktunya membaca
  }
  waktuBacaTerakhir = sekarang;

  sensors_event_t aksel, giro, suhu;
  mpu.getEvent(&aksel, &giro, &suhu);       // baca semua data sekaligus
  float ax = aksel.acceleration.x;          // satuan m/s²
  float ay = aksel.acceleration.y;
  float az = aksel.acceleration.z;
  float sudut = hitungSudutMiring(ax, ay, az);

  digitalWrite(PIN_LED, sudut > BATAS_AWAS ? HIGH : LOW);
  gambarLayar(ax, ay, az, sudut);

  if (sekarang - waktuSerialTerakhir >= JEDA_SERIAL_MS) {
    waktuSerialTerakhir = sekarang;
    int sudutBulat = (int)(sudut + 0.5f);   // pembulatan ke derajat terdekat
    Serial.printf("X:%6.2f  Y:%6.2f  Z:%6.2f m/s^2 | miring %2d derajat | %s\n",
                  ax, ay, az, sudutBulat, statusKemiringan(sudut));
  }
}

// Sudut antara sumbu Z sensor dan arah vertikal (0 derajat = datar).
float hitungSudutMiring(float ax, float ay, float az) {
  float besar = sqrt(ax * ax + ay * ay + az * az);  // ≈ 9,81 m/s² saat diam
  if (besar < 0.1) {
    return 0;                               // jatuh bebas: hindari pembagian dengan nol
  }
  float cosSudut = constrain(az / besar, -1.0f, 1.0f);
  return degrees(acos(cosSudut));
}

const char *statusKemiringan(float sudut) {
  if (sudut < BATAS_DATAR) {
    return "DATAR";
  }
  if (sudut <= BATAS_AWAS) {
    return "MIRING";
  }
  return "AWAS!";
}

void gambarLayar(float ax, float ay, float az, float sudut) {
  char teks[24];
  oled.clearDisplay();

  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print("Akselerasi (m/s^2)");

  snprintf(teks, sizeof(teks), "X:%6.2f", ax);
  oled.setCursor(0, 14);
  oled.print(teks);
  snprintf(teks, sizeof(teks), "Y:%6.2f", ay);
  oled.setCursor(0, 24);
  oled.print(teks);
  snprintf(teks, sizeof(teks), "Z:%6.2f", az);
  oled.setCursor(0, 34);
  oled.print(teks);

  snprintf(teks, sizeof(teks), "Miring:%3d", (int)(sudut + 0.5f));
  oled.setCursor(0, 46);
  oled.print(teks);
  oled.write(248);                          // simbol derajat pada tabel CP437
  oled.setCursor(0, 56);
  oled.print(statusKemiringan(sudut));

  // Waterpas: gelembung bergeser ke sisi yang lebih TINGGI, seperti aslinya.
  oled.drawCircle(PUSAT_X, PUSAT_Y, JARI_JARI, SSD1306_WHITE);
  oled.drawCircle(PUSAT_X, PUSAT_Y, 6, SSD1306_WHITE);      // zona "datar"
  int geserX = constrain((int)(ax / 9.81f * GESER_MAKS), -GESER_MAKS, GESER_MAKS);
  int geserY = constrain((int)(ay / 9.81f * GESER_MAKS), -GESER_MAKS, GESER_MAKS);
  oled.fillCircle(PUSAT_X + geserX, PUSAT_Y - geserY, 4, SSD1306_WHITE);

  oled.display();
}

void berhentiDenganPesan(const char *pesan) {
  Serial.println(pesan);
  while (true) {
    delay(1000);                            // program berhenti di sini
  }
}
