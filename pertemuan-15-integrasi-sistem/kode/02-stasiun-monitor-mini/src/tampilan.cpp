#include "tampilan.h"
#include "konfigurasi.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 oled(128, 64, &Wire, -1);   // 128×64 piksel, tanpa pin reset
static bool oledSiap = false;

static void gambarKepala(const char *judul, const IsiLayar &isi) {
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print(judul);
  oled.setCursor(110, 0);
  oled.printf("%d/%d", static_cast<int>(isi.halaman) + 1, JUMLAH_HALAMAN);
  oled.drawFastHLine(0, 10, 128, SSD1306_WHITE);
}

static void cetakSuhu(float nilai, bool valid) {    // contoh: 27.0°C
  if (valid) oled.printf("%.1f", nilai);
  else oled.print("--.-");
  oled.write(248);                                  // simbol derajat (CP437)
  oled.print("C");
}

static void gambarUtama(const IsiLayar &isi) {
  gambarKepala("STASIUN MONITOR", isi);
  oled.setTextSize(2);
  oled.setCursor(0, 15);
  cetakSuhu(isi.suhu, isi.sensorValid);
  oled.setTextSize(1);
  oled.setCursor(0, 35);
  oled.print("Lembap: ");
  if (isi.sensorValid) oled.printf("%.1f %%", isi.kelembapan);
  else oled.print("--.- %");
  oled.setCursor(0, 45);
  oled.print("Ambang: ");
  cetakSuhu(isi.ambang, true);
  oled.setCursor(0, 55);
  oled.print("Status: ");
  oled.print(alarmNamaStatus(isi.status));
}

static void gambarJaringan(const IsiLayar &isi) {
  gambarKepala("JARINGAN", isi);
  const InfoJaringan &j = isi.jaringan;
  bool wifiOk = (j.keadaan == KeadaanJaringan::MQTT_MENYAMBUNG ||
                 j.keadaan == KeadaanJaringan::TERSAMBUNG);
  const char *mqttTeks = (j.keadaan == KeadaanJaringan::TERSAMBUNG) ? "TERSAMBUNG"
                         : (wifiOk ? "MENYAMBUNG" : "-");
  unsigned long detik = millis() / 1000;
  oled.setCursor(0, 15);
  oled.printf("WiFi : %s", wifiOk ? "TERSAMBUNG" : "MENYAMBUNG");
  oled.setCursor(0, 25);
  oled.printf("IP   : %s", j.ip);
  oled.setCursor(0, 35);
  oled.printf("MQTT : %s", mqttTeks);
  oled.setCursor(0, 45);
  oled.printf("Kirim: %lu kali", (unsigned long)j.jumlahKirim);
  oled.setCursor(0, 55);
  oled.printf("Aktif: %02lu:%02lu:%02lu", detik / 3600, (detik / 60) % 60, detik % 60);
}

static void gambarAtur(const IsiLayar &isi) {
  gambarKepala("ATUR AMBANG", isi);
  oled.setCursor(0, 15);
  oled.print("Putar potensiometer");
  oled.setTextSize(2);
  oled.setCursor(0, 26);
  cetakSuhu(isi.ambang, true);
  oled.setTextSize(1);
  oled.setCursor(0, 45);
  oled.print("Suhu kini: ");
  cetakSuhu(isi.suhu, isi.sensorValid);
  oled.setCursor(0, 55);
  oled.print("Tekan tombol=simpan");
}

bool tampilanMulai() {
  Wire.begin(PIN_SDA, PIN_SCL);
  // periphBegin = false: Wire sudah dimulai di atas dengan pin dari konfigurasi.h
  oledSiap = oled.begin(SSD1306_SWITCHCAPVCC, ALAMAT_OLED, true, false);
  if (!oledSiap) return false;
  oled.cp437(true);                                 // tabel CP437: kode 248 = simbol derajat
  oled.setTextColor(SSD1306_WHITE);
  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println("Stasiun Monitor Mini");
  oled.println("Memulai...");
  oled.display();
  return true;
}

void tampilanGambar(const IsiLayar &isi) {
  if (!oledSiap) return;
  oled.clearDisplay();
  switch (isi.halaman) {
    case Halaman::UTAMA:       gambarUtama(isi); break;
    case Halaman::JARINGAN:    gambarJaringan(isi); break;
    case Halaman::ATUR_AMBANG: gambarAtur(isi); break;
  }
  oled.display();          // kirim 1.024 byte buffer lewat I2C 400 kHz (±25 ms)
}
