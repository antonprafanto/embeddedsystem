/*
 * =====================================================================
 *  Pertemuan 15 · Praktikum 1 — Uji Perangkat Keras Kit
 * =====================================================================
 *  Tujuan   : Memeriksa setiap komponen SEBELUM dirakit menjadi proyek.
 *             Enam bagian diuji bergiliran, lalu Serial Monitor
 *             menampilkan ringkasan LULUS / GAGAL / PERIKSA.
 *               LULUS   = terbukti bekerja (dicek otomatis oleh program)
 *               GAGAL   = terbukti bermasalah → ikuti petunjuk perbaikan
 *               PERIKSA = butuh mata/telingamu, atau langkahnya belum
 *                         kamu lakukan dalam batas waktu
 *  Rangkaian (sama dengan Praktikum 2):
 *    LED biru 23, merah 25, kuning 26, hijau 27 → resistor 220 Ω → LED → GND
 *    Buzzer pasif   : + → GPIO 18, − → GND
 *    Tombol         : GPIO 4 ↔ GND (INPUT_PULLUP)
 *    Potensiometer  : SIG → GPIO 34, kaki luar → 3V3 dan GND
 *    OLED SSD1306   : SDA → 21, SCL → 22, VCC → 3V3, GND → GND (0x3C)
 *    DHT22          : DATA → GPIO 14, VCC → 3V3, GND → GND
 *                     (+ pull-up 10 kΩ ke 3V3 bila sensor 4 kaki tanpa modul)
 *  Library  : DHT sensor library, Adafruit Unified Sensor.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor, lalu ikuti
 *                   petunjuk bertanda ">>".
 *    - Wokwi web  : salin berkas ini ke sketch.ino, salin diagram.json dan
 *                   libraries.txt, klik ▶, lalu ikuti petunjuk di Serial.
 *  Hasil yang diharapkan:
 *    Petunjuk tiap langkah tampil di Serial Monitor, diakhiri tabel
 *    ringkasan, misalnya "LULUS 4 · GAGAL 0 · PERIKSA 2". Tekan tombol
 *    (GPIO 4) untuk mengulang semua uji.
 * =====================================================================
 */
#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>

// ---------- Peta pin (peta pin standar kursus) ----------
const int PIN_LED_BIRU   = 23;
const int PIN_LED_MERAH  = 25;
const int PIN_LED_KUNING = 26;
const int PIN_LED_HIJAU  = 27;
const int PIN_BUZZER     = 18;
const int PIN_TOMBOL     = 4;
const int PIN_POT        = 34;   // ADC1: tetap bisa dibaca walau Wi-Fi aktif
const int PIN_SDA        = 21;
const int PIN_SCL        = 22;
const int PIN_DHT        = 14;

const unsigned long BATAS_TUNGGU_MS = 10000;  // waktu untukmu menekan tombol / memutar potensio
const int FREKUENSI_BUZZER = 2000;            // Hz, nada bip

// ---------- Catatan hasil uji ----------
enum Hasil { LULUS, GAGAL, PERIKSA };

struct Uji {
  const char *nama;    // nama komponen di tabel ringkasan
  Hasil hasil;
  String catatan;      // keterangan singkat
};

const int JUMLAH_UJI = 6;
Uji daftarUji[JUMLAH_UJI] = {
  {"LED 23/25/26/27", PERIKSA, ""},
  {"Buzzer 18", PERIKSA, ""},
  {"Tombol 4", PERIKSA, ""},
  {"Potensiometer 34", PERIKSA, ""},
  {"I2C 21/22", PERIKSA, ""},
  {"DHT22 14", PERIKSA, ""},
};

DHT dht(PIN_DHT, DHT22);
bool tombolSebelumnya = HIGH;

// ---------- Deklarasi fungsi ----------
void jalankanSemuaUji();
void ujiLed(Uji &u);
void ujiBuzzer(Uji &u);
void ujiTombol(Uji &u);
void ujiPotensiometer(Uji &u);
void ujiI2C(Uji &u);
void ujiDht22(Uji &u);
void cetakJudul(int nomor, const char *judul);
void cetakHasil(const Uji &u);
void cetakPetunjuk(const char *teks);
void cetakRingkasan();
const char *teksHasil(Hasil h);
const char *tebakPerangkat(uint8_t alamat);

void setup() {
  Serial.begin(115200);
  delay(1000);                                  // beri waktu Serial Monitor siap
  const int pinLed[] = {PIN_LED_BIRU, PIN_LED_MERAH, PIN_LED_KUNING, PIN_LED_HIJAU};
  for (int pin : pinLed) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
  pinMode(PIN_TOMBOL, INPUT_PULLUP);
  ledcAttach(PIN_BUZZER, FREKUENSI_BUZZER, 8);  // PWM 2 kHz, resolusi 8 bit (0–255)
  ledcWrite(PIN_BUZZER, 0);                     // diam dulu
  dht.begin();
  jalankanSemuaUji();
}

void loop() {
  // Setelah ringkasan: tekan tombol sekali untuk mengulang semua uji.
  bool sekarang = digitalRead(PIN_TOMBOL);
  if (tombolSebelumnya == HIGH && sekarang == LOW) {
    delay(30);                                  // abaikan pantulan (bouncing)
    if (digitalRead(PIN_TOMBOL) == LOW) jalankanSemuaUji();
  }
  tombolSebelumnya = digitalRead(PIN_TOMBOL);
  delay(10);
}

// Program uji memang berurutan (langkah demi langkah), jadi delay() di sini
// wajar. Program proyek sesungguhnya (Praktikum 2) tidak boleh begini.
void jalankanSemuaUji() {
  Serial.println();
  Serial.println("===============================================");
  Serial.println("  UJI PERANGKAT KERAS — Kit Stasiun Monitor");
  Serial.println("===============================================");
  Serial.println("Ikuti petunjuk bertanda >> di setiap langkah.");
  ujiLed(daftarUji[0]);
  ujiBuzzer(daftarUji[1]);
  ujiTombol(daftarUji[2]);
  ujiPotensiometer(daftarUji[3]);
  ujiI2C(daftarUji[4]);
  ujiDht22(daftarUji[5]);
  cetakRingkasan();
}

void ujiLed(Uji &u) {
  cetakJudul(1, "LED biru 23, merah 25, kuning 26, hijau 27");
  Serial.println("  >> Perhatikan LED: masing-masing berkedip 2 kali, bergiliran.");
  const int pinLed[] = {PIN_LED_BIRU, PIN_LED_MERAH, PIN_LED_KUNING, PIN_LED_HIJAU};
  const char *namaLed[] = {"biru 23", "merah 25", "kuning 26", "hijau 27"};
  for (int i = 0; i < 4; i++) {
    Serial.printf("     LED %s\n", namaLed[i]);
    for (int k = 0; k < 2; k++) {
      digitalWrite(pinLed[i], HIGH);
      delay(300);
      digitalWrite(pinLed[i], LOW);
      delay(200);
    }
  }
  u.hasil = PERIKSA;                            // program tidak bisa "melihat" LED
  u.catatan = "pastikan keempat LED tadi menyala";
  cetakHasil(u);
  cetakPetunjuk("ada yang padam? Kaki panjang (anode) ke resistor/GPIO, kaki pendek ke GND.");
}

void ujiBuzzer(Uji &u) {
  cetakJudul(2, "Buzzer pasif di GPIO 18");
  Serial.println("  >> Dengarkan: tiga bunyi bip.");
  for (int i = 0; i < 3; i++) {
    ledcWrite(PIN_BUZZER, 128);                 // siklus kerja 50% → berbunyi 2 kHz
    delay(150);
    ledcWrite(PIN_BUZZER, 0);                   // 0% → diam
    delay(250);
  }
  u.hasil = PERIKSA;                            // program tidak bisa "mendengar"
  u.catatan = "pastikan tiga bip terdengar";
  cetakHasil(u);
  cetakPetunjuk("tidak berbunyi? Cek kaki + ke GPIO 18 dan kaki - ke GND. Di Wokwi, cek suara peramban.");
}

void ujiTombol(Uji &u) {
  cetakJudul(3, "Tombol di GPIO 4 (INPUT_PULLUP)");
  // Tanpa disentuh, pull-up internal membuat pin terbaca HIGH.
  if (digitalRead(PIN_TOMBOL) == LOW) {
    delay(200);
    if (digitalRead(PIN_TOMBOL) == LOW) {
      u.hasil = GAGAL;
      u.catatan = "terbaca ditekan terus";
      cetakHasil(u);
      cetakPetunjuk("tombol mungkin terpasang terputar 90°, atau GPIO 4 tersambung langsung ke GND.");
      return;
    }
  }
  Serial.println("  >> Tekan lalu lepaskan tombol dalam 10 detik...");
  bool pernahDitekan = false;
  unsigned long mulai = millis();
  while (millis() - mulai < BATAS_TUNGGU_MS) {
    bool ditekan = (digitalRead(PIN_TOMBOL) == LOW);
    if (!pernahDitekan && ditekan) {
      delay(30);                                // pastikan bukan pantulan
      if (digitalRead(PIN_TOMBOL) == LOW) {
        pernahDitekan = true;
        Serial.println("     Terbaca: DITEKAN");
      }
    } else if (pernahDitekan && !ditekan) {
      delay(30);
      if (digitalRead(PIN_TOMBOL) == HIGH) {
        Serial.println("     Terbaca: DILEPAS");
        u.hasil = LULUS;
        u.catatan = "ditekan dan dilepas terbaca";
        cetakHasil(u);
        return;
      }
    }
    delay(5);
  }
  u.hasil = PERIKSA;
  u.catatan = pernahDitekan ? "ditekan, tetapi tidak dilepas" : "tidak ada tekanan dalam 10 detik";
  cetakHasil(u);
  cetakPetunjuk("sudah menekan tetapi tidak terbaca? Cek kabel GPIO 4 dan GND pada tombol.");
}

void ujiPotensiometer(Uji &u) {
  cetakJudul(4, "Potensiometer di GPIO 34 (ADC1)");
  Serial.println("  >> Putar knop pelan-pelan dari ujung ke ujung dalam 10 detik...");
  int nilaiMin = 4095, nilaiMaks = 0, lompatan = 0;
  int sebelumnya = analogRead(PIN_POT);
  unsigned long mulai = millis(), cetakTerakhir = mulai;
  while (millis() - mulai < BATAS_TUNGGU_MS) {
    int nilai = analogRead(PIN_POT);            // 0–4095 (12 bit)
    nilaiMin = min(nilaiMin, nilai);
    nilaiMaks = max(nilaiMaks, nilai);
    if (abs(nilai - sebelumnya) > 1500) lompatan++;   // lompatan sebesar ini dalam 20 ms tidak wajar
    sebelumnya = nilai;
    if (millis() - cetakTerakhir >= 1000) {     // tampilkan nilai tiap detik
      cetakTerakhir = millis();
      Serial.printf("     ADC = %4d\n", nilai);
    }
    delay(20);
  }
  u.catatan = "min " + String(nilaiMin) + ", maks " + String(nilaiMaks);
  if (lompatan >= 5) {
    u.hasil = GAGAL;
    cetakHasil(u);
    cetakPetunjuk("nilai melompat-lompat: kaki tengah (SIG) mungkin lepas sehingga pin mengambang.");
  } else if (nilaiMaks - nilaiMin >= 3000) {
    u.hasil = LULUS;
    cetakHasil(u);
  } else if (nilaiMaks < 100) {
    u.hasil = GAGAL;
    cetakHasil(u);
    cetakPetunjuk("selalu sekitar 0: kaki luar belum ke 3V3, atau SIG tersambung ke GND.");
  } else if (nilaiMin > 3995) {
    u.hasil = GAGAL;
    cetakHasil(u);
    cetakPetunjuk("selalu sekitar 4095: kaki luar belum ke GND.");
  } else {
    u.hasil = PERIKSA;
    cetakHasil(u);
    cetakPetunjuk("rentang masih sempit. Putar sampai mentok di kedua ujung, lalu ulangi uji.");
  }
}

void ujiI2C(Uji &u) {
  cetakJudul(5, "Bus I2C: SDA 21, SCL 22");
  // Saat diam, kedua jalur I2C harus HIGH (ditarik resistor pull-up).
  pinMode(PIN_SDA, INPUT_PULLUP);
  pinMode(PIN_SCL, INPUT_PULLUP);
  delay(5);
  if (digitalRead(PIN_SDA) == LOW || digitalRead(PIN_SCL) == LOW) {
    u.hasil = GAGAL;
    u.catatan = "SDA/SCL tertahan LOW";
    cetakHasil(u);
    cetakPetunjuk("cek apakah SDA atau SCL tersambung ke GND, atau ada modul I2C yang rusak.");
    return;
  }
  Wire.begin(PIN_SDA, PIN_SCL);
  int jumlah = 0;
  bool adaOled = false;
  for (uint8_t alamat = 0x08; alamat <= 0x77; alamat++) {
    Wire.beginTransmission(alamat);
    if (Wire.endTransmission() == 0) {          // 0 = ada yang menjawab ACK
      jumlah++;
      Serial.printf("     Ditemukan perangkat di 0x%02X %s\n", alamat, tebakPerangkat(alamat));
      if (alamat == 0x3C || alamat == 0x3D) adaOled = true;
    }
  }
  Wire.end();                                   // lepaskan pin agar uji bisa diulang dengan bersih
  if (adaOled) {
    u.hasil = LULUS;
    u.catatan = String(jumlah) + " perangkat, termasuk OLED";
    cetakHasil(u);
  } else if (jumlah > 0) {
    u.hasil = PERIKSA;
    u.catatan = String(jumlah) + " perangkat, tetapi tanpa OLED";
    cetakHasil(u);
    cetakPetunjuk("alamat OLED biasanya 0x3C (kadang 0x3D). Cek kabel OLED.");
  } else {
    u.hasil = GAGAL;
    u.catatan = "tidak ada perangkat yang menjawab";
    cetakHasil(u);
    cetakPetunjuk("cek VCC ke 3V3 dan GND modul, SDA ke 21, SCL ke 22 (sering tertukar).");
  }
}

void ujiDht22(Uji &u) {
  cetakJudul(6, "Sensor DHT22 di GPIO 14");
  Serial.println("     Membaca sensor (butuh sekitar 2 detik)...");
  delay(2000);                                  // DHT22 butuh jeda minimal 2 s antarpembacaan
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();
  if (isnan(suhu) || isnan(kelembapan)) {       // coba sekali lagi: sekali gagal belum tentu rusak
    delay(2500);
    suhu = dht.readTemperature();
    kelembapan = dht.readHumidity();
  }
  if (isnan(suhu) || isnan(kelembapan)) {
    u.hasil = GAGAL;
    u.catatan = "tidak ada jawaban (NaN)";
    cetakHasil(u);
    cetakPetunjuk("cek DATA ke GPIO 14, VCC ke 3V3, GND, dan pull-up 10 kΩ (sensor tanpa modul).");
    return;
  }
  u.catatan = String(suhu, 1) + " °C, " + String(kelembapan, 1) + " %";
  if (suhu < -40 || suhu > 80 || kelembapan < 0 || kelembapan > 100) {
    u.hasil = PERIKSA;                          // di luar jangkauan ukur DHT22
    cetakHasil(u);
    cetakPetunjuk("nilai di luar jangkauan DHT22 (-40 s.d. 80 °C). Cek kabel dan sensornya.");
  } else {
    u.hasil = LULUS;
    cetakHasil(u);
  }
}

void cetakJudul(int nomor, const char *judul) {
  Serial.println();
  Serial.printf("[%d/%d] %s\n", nomor, JUMLAH_UJI, judul);
}

void cetakHasil(const Uji &u) {
  Serial.printf("  Hasil: %s (%s)\n", teksHasil(u.hasil), u.catatan.c_str());
}

void cetakPetunjuk(const char *teks) {
  Serial.printf("     Petunjuk: %s\n", teks);
}

void cetakRingkasan() {
  int lulus = 0, gagal = 0, periksa = 0;
  Serial.println();
  Serial.println("================ RINGKASAN UJI ================");
  Serial.println("  No  Komponen           Hasil    Keterangan");
  for (int i = 0; i < JUMLAH_UJI; i++) {
    const Uji &u = daftarUji[i];
    Serial.printf("  %-3d %-18s %-8s %s\n", i + 1, u.nama, teksHasil(u.hasil), u.catatan.c_str());
    if (u.hasil == LULUS) lulus++;
    else if (u.hasil == GAGAL) gagal++;
    else periksa++;
  }
  Serial.println("-----------------------------------------------");
  Serial.printf("  LULUS %d · GAGAL %d · PERIKSA %d\n", lulus, gagal, periksa);
  if (gagal > 0) {
    Serial.println("  Perbaiki dulu butir GAGAL (lihat petunjuk di atas), lalu uji ulang.");
  } else {
    Serial.println("  Tidak ada yang GAGAL. Pastikan butir PERIKSA sudah kamu cek");
    Serial.println("  dengan mata dan telinga, lalu lanjutkan ke Praktikum 2.");
  }
  Serial.println("  Tekan tombol (GPIO 4) atau EN untuk mengulang uji.");
  Serial.println("===============================================");
}

const char *teksHasil(Hasil h) {
  switch (h) {
    case LULUS: return "LULUS";
    case GAGAL: return "GAGAL";
    default:    return "PERIKSA";
  }
}

const char *tebakPerangkat(uint8_t alamat) {
  switch (alamat) {
    case 0x3C: case 0x3D: return "(kemungkinan OLED SSD1306)";
    case 0x68: case 0x69: return "(kemungkinan MPU6050)";
    default:              return "(cek lembar datanya)";
  }
}
