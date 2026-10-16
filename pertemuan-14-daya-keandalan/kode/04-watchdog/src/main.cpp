/*
 * =====================================================================
 *  Pertemuan 14 · Praktikum 4 — Watchdog Penyelamat Program Macet
 * =====================================================================
 *  Tujuan   : Mengawasi loop() dengan Task Watchdog (TWDT) berbatas
 *             3 detik. Saat program sengaja dibuat macet, watchdog
 *             me-reset ESP32 secara otomatis. Setelah boot, penyebab reset
 *             ditampilkan dengan esp_reset_reason() dan dicatat di NVS.
 *  Rangkaian: Tombol tekan antara GPIO 4 dan GND (INPUT_PULLUP) untuk
 *             memicu macet. LED bawaan GPIO 2 berkedip saat program sehat.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor, lalu tekan
 *                   tombol GPIO 4 atau ketik perintah: macet
 *    - Wokwi web  : salin sketch.ino + diagram.json, klik ▶, tekan tombol
 *                   atau ketik "macet" di Serial Monitor.
 *  Hasil yang diharapkan:
 *    Pesan "Bekerja normal" tiap detik. Setelah "macet", ±3 detik kemudian
 *    muncul pesan task_wdt, ESP32 reset sendiri, lalu menampilkan
 *    "Penyebab reset terakhir: TASK_WDT" dan jumlah reset watchdog.
 * =====================================================================
 */
#include <Arduino.h>
#include <esp_task_wdt.h>
#include <Preferences.h>

const int PIN_TOMBOL = 4;                 // tombol "bikin macet", ke GND
const int PIN_LED = 2;                    // LED bawaan: berkedip = program sehat
const uint32_t BATAS_WDT_MS = 3000;       // watchdog "menggigit" setelah 3 detik

unsigned long waktuLaporTerakhir = 0;
unsigned long detikBekerja = 0;
bool ledMenyala = false;
int tombolSebelumnya = HIGH;

const char *namaPenyebabReset(esp_reset_reason_t alasan);
void catatResetWatchdog(esp_reset_reason_t alasan);
void aktifkanWatchdog();
bool sensorSiap();
void simulasikanMacet();

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TOMBOL, INPUT_PULLUP);
  tombolSebelumnya = digitalRead(PIN_TOMBOL);

  esp_reset_reason_t alasan = esp_reset_reason();
  Serial.println();
  Serial.println("=== Praktikum 4: Watchdog ===");
  Serial.printf("[INFO] Penyebab reset terakhir: %s\n", namaPenyebabReset(alasan));
  catatResetWatchdog(alasan);
  aktifkanWatchdog();
  Serial.println("[INFO] Ketik 'macet' atau tekan tombol GPIO 4 untuk membuat program macet.");
}

void loop() {
  esp_task_wdt_reset();                   // "beri makan" watchdog: loop() masih sehat

  if (millis() - waktuLaporTerakhir >= 1000) {
    waktuLaporTerakhir = millis();
    detikBekerja++;
    ledMenyala = !ledMenyala;
    digitalWrite(PIN_LED, ledMenyala ? HIGH : LOW);
    Serial.printf("[INFO] Bekerja normal, detik ke-%lu\n", detikBekerja);
  }

  // Pemicu macet 1: tombol baru saja ditekan (perubahan HIGH -> LOW)
  int tombolSekarang = digitalRead(PIN_TOMBOL);
  bool mintaMacet = (tombolSebelumnya == HIGH && tombolSekarang == LOW);
  tombolSebelumnya = tombolSekarang;

  // Pemicu macet 2: perintah "macet" dari Serial Monitor
  if (Serial.available()) {
    String perintah = Serial.readStringUntil('\n');
    perintah.trim();
    if (perintah == "macet") {
      mintaMacet = true;
    }
  }

  if (mintaMacet) {
    simulasikanMacet();
  }
}

void aktifkanWatchdog() {
  esp_task_wdt_config_t konfigurasi;
  konfigurasi.timeout_ms = BATAS_WDT_MS;
  konfigurasi.idle_core_mask = 1 << 0;    // tetap awasi idle task inti 0 (bawaan)
  konfigurasi.trigger_panic = true;       // waktu habis -> panic -> reset otomatis
  esp_task_wdt_reconfigure(&konfigurasi); // TWDT sudah aktif sejak boot; ubah batasnya
  esp_task_wdt_add(NULL);                 // NULL = task yang sedang berjalan (loopTask)
  Serial.printf("[INFO] Watchdog aktif: batas %lu ms, loop() ikut diawasi.\n",
                (unsigned long)BATAS_WDT_MS);
}

// Pura-pura: sensor rusak dan tidak pernah menjawab
bool sensorSiap() {
  return false;
}

void simulasikanMacet() {
  Serial.println("[PERINGATAN] Menunggu sensor yang tidak pernah menjawab...");
  while (!sensorSiap()) {                 // BUG disengaja: menunggu TANPA batas waktu
    delay(10);                            // watchdog tidak lagi diberi makan
  }
}

// Mencatat berapa kali ESP32 di-reset oleh watchdog (disimpan di NVS)
void catatResetWatchdog(esp_reset_reason_t alasan) {
  Preferences catatan;
  catatan.begin("catatan", false);
  uint32_t jumlah = catatan.getUInt("resetWdt", 0);
  if (alasan == ESP_RST_TASK_WDT || alasan == ESP_RST_INT_WDT || alasan == ESP_RST_WDT) {
    jumlah++;
    catatan.putUInt("resetWdt", jumlah);  // ditulis hanya saat kejadian langka
  }
  catatan.end();
  Serial.printf("[DATA] Jumlah reset oleh watchdog (tersimpan di NVS): %lu\n",
                (unsigned long)jumlah);
}

const char *namaPenyebabReset(esp_reset_reason_t alasan) {
  switch (alasan) {
    case ESP_RST_POWERON:   return "POWERON (baru mendapat catu / tombol EN)";
    case ESP_RST_SW:        return "SW (ESP.restart() dari program)";
    case ESP_RST_PANIC:     return "PANIC (program crash)";
    case ESP_RST_INT_WDT:   return "INT_WDT (interrupt watchdog)";
    case ESP_RST_TASK_WDT:  return "TASK_WDT (task watchdog: program macet)";
    case ESP_RST_WDT:       return "WDT (watchdog lain)";
    case ESP_RST_DEEPSLEEP: return "DEEPSLEEP (bangun dari deep sleep)";
    case ESP_RST_BROWNOUT:  return "BROWNOUT (tegangan catu sempat turun)";
    default:                return "lainnya";
  }
}
