/*
 * =====================================================================
 *  Pertemuan 14 · Praktikum 2 — Bangun dengan Tombol (EXT0)
 * =====================================================================
 *  Tujuan   : ESP32 tidur nyenyak dan bangun ketika tombol ditekan
 *             (sumber bangun EXT0) atau, sebagai cadangan, setelah 60 detik
 *             (timer). Pull-up versi RTC dijaga tetap aktif selama tidur.
 *  Rangkaian: Tombol tekan antara GPIO 33 dan GND. Pull-up memakai resistor
 *             internal RTC (rtc_gpio_pullup_en). Pilihan paling andal:
 *             tambah resistor 10 kΩ dari GPIO 33 ke 3V3. LED bawaan GPIO 2.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → buka Serial Monitor, tunggu
 *                   pesan "Tidur...", lalu tekan tombol.
 *    - Wokwi web  : salin sketch.ino + diagram.json, klik ▶ (baca README
 *                   tentang keterbatasan deep sleep di Wokwi).
 *  Hasil yang diharapkan:
 *    Setiap tombol ditekan, ESP32 bangun, LED berkedip 3 kali, dan Serial
 *    Monitor menampilkan "Bangun karena TOMBOL" beserta penghitungnya.
 *    Tanpa tombol, ESP32 bangun sendiri tiap 60 detik ("TIMER").
 * =====================================================================
 */
#include <Arduino.h>
#include "esp_sleep.h"
#include "driver/rtc_io.h"

const gpio_num_t PIN_BANGUN = GPIO_NUM_33;   // wajib RTC GPIO; tombol ke GND
const int PIN_LED = 2;                        // LED bawaan
const int TIDUR_MAKS_DETIK = 60;              // timer cadangan
const unsigned long BATAS_TUNGGU_MS = 3000;   // batas menunggu tombol dilepas

// Penghitung di memori RTC: bertahan selama deep sleep
RTC_DATA_ATTR int bangunOlehTombol = 0;
RTC_DATA_ATTR int bangunOlehTimer = 0;

void kedipkanLed(int kali);
bool tungguTombolDilepas(unsigned long batasMs);

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(PIN_LED, OUTPUT);

  // Setelah bangun oleh EXT0, pin masih diatur sebagai RTC IO.
  // Kembalikan ke GPIO digital biasa sebelum dibaca dengan digitalRead().
  rtc_gpio_deinit(PIN_BANGUN);
  pinMode(PIN_BANGUN, INPUT_PULLUP);

  esp_sleep_wakeup_cause_t penyebab = esp_sleep_get_wakeup_cause();
  Serial.println();
  if (penyebab == ESP_SLEEP_WAKEUP_EXT0) {
    bangunOlehTombol++;
    Serial.println("[INFO] Bangun karena TOMBOL ditekan (EXT0).");
    kedipkanLed(3);
  } else if (penyebab == ESP_SLEEP_WAKEUP_TIMER) {
    bangunOlehTimer++;
    Serial.println("[INFO] Bangun karena TIMER cadangan.");
    kedipkanLed(1);
  } else {
    Serial.println("[INFO] Baru menyala/di-reset.");
  }
  Serial.printf("[DATA] Bangun oleh tombol: %d kali, oleh timer: %d kali\n",
                bangunOlehTombol, bangunOlehTimer);

  // Jangan tidur selagi tombol masih ditekan: ESP32 akan langsung bangun lagi.
  bool tombolLepas = tungguTombolDilepas(BATAS_TUNGGU_MS);

  // Sumber bangun 1: timer cadangan (satuan mikrodetik)
  esp_sleep_enable_timer_wakeup((uint64_t)TIDUR_MAKS_DETIK * 1000000ULL);

  // Sumber bangun 2: EXT0, bangun ketika GPIO 33 bernilai LOW (tombol ditekan)
  if (tombolLepas) {
    rtc_gpio_pullup_en(PIN_BANGUN);      // pull-up versi RTC: tetap aktif saat tidur
    rtc_gpio_pulldown_dis(PIN_BANGUN);
    esp_sleep_enable_ext0_wakeup(PIN_BANGUN, 0);   // 0 = bangun saat LOW
    Serial.println("[INFO] Tidur... tekan tombol GPIO 33 untuk membangunkan.");
  } else {
    Serial.println("[PERINGATAN] Tombol tertahan > 3 detik. Tidur dengan timer saja.");
  }
  Serial.flush();
  digitalWrite(PIN_LED, LOW);
  esp_deep_sleep_start();
}

void loop() {
  // Tidak pernah dijalankan.
}

void kedipkanLed(int kali) {
  for (int i = 0; i < kali; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(150);
    digitalWrite(PIN_LED, LOW);
    delay(150);
  }
}

// Menunggu tombol dilepas, tetapi TIDAK selamanya: ada batas waktu.
bool tungguTombolDilepas(unsigned long batasMs) {
  unsigned long mulai = millis();
  while (digitalRead(PIN_BANGUN) == LOW) {
    if (millis() - mulai > batasMs) {
      return false;                      // menyerah: tombol macet atau tertahan
    }
    delay(10);
  }
  delay(50);                             // jeda kecil agar pantulan (bouncing) reda
  return true;
}
