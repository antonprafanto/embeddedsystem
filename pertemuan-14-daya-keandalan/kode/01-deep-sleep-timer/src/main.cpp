/*
 * =====================================================================
 *  Pertemuan 14 · Praktikum 1 — Deep Sleep dengan Timer
 * =====================================================================
 *  Tujuan   : ESP32 bekerja sebentar, lalu tidur nyenyak (deep sleep)
 *             10 detik, bangun, dan mengulang siklus itu terus-menerus.
 *             Jumlah boot disimpan di memori RTC (RTC_DATA_ATTR), dan
 *             penyebab bangun dibaca dengan esp_sleep_get_wakeup_cause().
 *  Rangkaian: Tidak perlu komponen tambahan. LED biru bawaan (GPIO 2)
 *             menyala selama ESP32 bangun dan padam saat ESP32 tidur.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → buka Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino (+ diagram.json), klik ▶.
 *                   Dukungan deep sleep di Wokwi terbatas — baca README.
 *  Hasil yang diharapkan:
 *    Kira-kira tiap 13 detik (10 detik tidur + ±3 detik bangun) muncul
 *    blok "ESP32 bangun" dengan nomor boot yang terus bertambah.
 *    Boot pertama: penyebab bangun "bukan dari deep sleep"; boot
 *    berikutnya: "timer". Variabel biasa (SRAM) selalu bernilai 1.
 * =====================================================================
 */
#include <Arduino.h>
#include "esp_sleep.h"

const int PIN_LED = 2;               // LED bawaan: tanda ESP32 sedang bangun
const int LAMA_TIDUR_DETIK = 10;     // lama tidur di antara dua fase kerja
const int LAMA_KERJA_MS = 2000;      // pura-pura membaca sensor dan mengirim data

// Disimpan di memori RTC: nilainya BERTAHAN selama deep sleep,
// tetapi kembali 0 bila catu dicabut atau tombol EN ditekan.
RTC_DATA_ATTR int jumlahBoot = 0;

// Variabel biasa di SRAM: isinya hilang setiap kali ESP32 tidur nyenyak.
int variabelBiasa = 0;

void tampilkanPenyebabBangun();

void setup() {
  Serial.begin(115200);
  delay(500);                        // beri waktu Serial Monitor siap
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);       // LED menyala = sedang bangun

  jumlahBoot++;
  variabelBiasa++;

  Serial.println();
  Serial.println("===== ESP32 bangun =====");
  Serial.printf("Boot ke-%d\n", jumlahBoot);
  Serial.printf("Variabel biasa (SRAM): %d\n", variabelBiasa);
  tampilkanPenyebabBangun();

  // Fase kerja yang singkat. Di proyek nyata: baca sensor, kirim data, selesai.
  Serial.println("[INFO] Bekerja 2 detik...");
  delay(LAMA_KERJA_MS);

  // Atur sumber bangun SEBELUM tidur. Satuan timer: mikrodetik.
  esp_sleep_enable_timer_wakeup((uint64_t)LAMA_TIDUR_DETIK * 1000000ULL);
  Serial.printf("[INFO] Tidur %d detik. Sampai jumpa!\n", LAMA_TIDUR_DETIK);
  Serial.flush();                    // tunggu semua teks benar-benar terkirim
  digitalWrite(PIN_LED, LOW);
  esp_deep_sleep_start();            // fungsi ini tidak pernah kembali
}

void loop() {
  // Tidak pernah dijalankan: bangun dari deep sleep = mulai lagi dari setup().
}

// Menerjemahkan kode penyebab bangun menjadi teks yang mudah dibaca
void tampilkanPenyebabBangun() {
  esp_sleep_wakeup_cause_t penyebab = esp_sleep_get_wakeup_cause();
  Serial.print("Penyebab bangun: ");
  switch (penyebab) {
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("timer"); break;
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("EXT0 (satu pin RTC)"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("EXT1 (beberapa pin RTC)"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("sentuhan (touch)"); break;
    case ESP_SLEEP_WAKEUP_ULP:      Serial.println("program ULP"); break;
    case ESP_SLEEP_WAKEUP_UNDEFINED:
      Serial.println("bukan dari deep sleep (baru menyala/di-reset)");
      break;
    default:
      Serial.printf("lainnya (kode %d)\n", (int)penyebab);
      break;
  }
}
