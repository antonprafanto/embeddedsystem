/*
 * =====================================================================
 *  Pertemuan 11 · Praktikum 3 — Membaca Crash
 * =====================================================================
 *  Tujuan   : Sengaja membuat ESP32 crash dengan perintah dari Serial
 *             Monitor, lalu belajar membaca pesan "Guru Meditation Error",
 *             register PC/EXCVADDR, dan backtrace.
 *  Rangkaian: Tidak perlu komponen tambahan.
 *  Perintah : bagi0 · null · baca · rekursi · macet · assert · bantuan
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor. Filter
 *                   esp32_exception_decoder di platformio.ini menerjemahkan
 *                   alamat backtrace menjadi nama fungsi dan nomor baris.
 *    - Wokwi web  : salin berkas ini ke sketch.ino, klik ▶, lalu ketik
 *                   perintah di kotak input Serial Monitor.
 *  Hasil yang diharapkan:
 *    Setiap perintah crash memunculkan pesan galat beserta backtrace,
 *    lalu ESP32 di-reset otomatis ("Rebooting...") dan program mulai lagi.
 * =====================================================================
 */
#include <Arduino.h>
#include <assert.h>

struct Sensor {
  int id;      // offset 0
  int nilai;   // offset 4 → membaca ini lewat pointer null = alamat 0x00000004
};

Sensor *volatile sensorAktif = nullptr;   // sengaja "lupa" diisi

// ---- Deklarasi fungsi ----
void tampilkanBantuan();
void jalankanPerintah(const String &perintah);
const char *namaPenyebabReset(esp_reset_reason_t alasan);
void crashBagiNol();
void crashTulisNull();
void crashBacaNull();
int crashRekursi(int kedalaman);
void crashMacet();
void cekSuhu(int suhu);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("=== Praktikum 3: Membaca Crash ===");
  // Petunjuk pertama setelah crash: MENGAPA board tadi di-reset?
  Serial.printf("[INFO] Penyebab reset terakhir: %s\n", namaPenyebabReset(esp_reset_reason()));

  // Makro log: yang tampil ditentukan CORE_DEBUG_LEVEL di platformio.ini
  log_e("contoh log_e (level 1: ERROR)");
  log_w("contoh log_w (level 2: WARN)");
  log_i("contoh log_i (level 3: INFO)");
  log_d("contoh log_d (level 4: DEBUG)");

  enableLoopWDT();   // loop() ikut diawasi Task Watchdog (batas 5 detik)
  tampilkanBantuan();
}

void loop() {
  if (Serial.available() > 0) {
    String perintah = Serial.readStringUntil('\n');
    perintah.trim();
    if (perintah.length() > 0) {
      jalankanPerintah(perintah);
    }
  }
}

void tampilkanBantuan() {
  Serial.println("Ketik perintah lalu Enter:");
  Serial.println("  bagi0   : bagi bilangan bulat dengan nol");
  Serial.println("  null    : tulis ke pointer null");
  Serial.println("  baca    : baca lewat pointer null");
  Serial.println("  rekursi : rekursi tanpa henti (stack overflow)");
  Serial.println("  macet   : perulangan tanpa jeda (Task Watchdog)");
  Serial.println("  assert  : assert() yang gagal");
}

void jalankanPerintah(const String &perintah) {
  Serial.printf("[INFO] Perintah: %s\n", perintah.c_str());
  if (perintah == "bagi0") {
    crashBagiNol();
  } else if (perintah == "null") {
    crashTulisNull();
  } else if (perintah == "baca") {
    crashBacaNull();
  } else if (perintah == "rekursi") {
    crashRekursi(1);
  } else if (perintah == "macet") {
    crashMacet();
  } else if (perintah == "assert") {
    cekSuhu(500);
  } else if (perintah == "bantuan") {
    tampilkanBantuan();
  } else {
    Serial.println("[PERINGATAN] Perintah tidak dikenal. Ketik: bantuan");
  }
}

void crashBagiNol() {
  volatile int pembagi = 0;   // volatile: paksa pembagian benar-benar dikerjakan CPU
  int hasil = 100 / pembagi;
  Serial.printf("Hasil: %d\n", hasil);   // tidak akan pernah tercetak
}

void crashTulisNull() {
  int *volatile penunjuk = nullptr;   // pointer belum menunjuk ke mana-mana
  *penunjuk = 42;                     // menulis ke alamat 0x00000000
}

void crashBacaNull() {
  int nilai = sensorAktif->nilai;     // membaca alamat 0 + 4 = 0x00000004
  Serial.printf("Nilai sensor: %d\n", nilai);
}

int crashRekursi(int kedalaman) {
  volatile char penampung[128];       // tiap panggilan memakan >128 byte stack
  penampung[0] = (char)kedalaman;
  if (kedalaman % 10 == 0) {
    Serial.printf("[INFO] kedalaman %d, sisa stack %u byte\n", kedalaman,
                  uxTaskGetStackHighWaterMark(NULL));
  }
  if (kedalaman > 100000) {           // syarat berhenti yang terlalu jauh
    return 0;
  }
  return crashRekursi(kedalaman + 1) + penampung[0];
}

void crashMacet() {
  Serial.println("[INFO] Masuk perulangan tanpa jeda. Tunggu sekitar 5 detik...");
  volatile unsigned long hitungan = 0;
  while (true) {
    hitungan = hitungan + 1;   // sibuk terus: loop() tak pernah selesai, watchdog tak diberi makan
  }
}

void cekSuhu(int suhu) {
  // Sensor hanya mampu -40..125 °C. Nilai di luar itu PASTI bug, bukan kondisi wajar.
  assert(suhu >= -40 && suhu <= 125);
  Serial.printf("Suhu %d C valid\n", suhu);
}

const char *namaPenyebabReset(esp_reset_reason_t alasan) {
  switch (alasan) {
    case ESP_RST_POWERON:  return "POWERON (baru dinyalakan)";
    case ESP_RST_SW:       return "SW (esp_restart dipanggil)";
    case ESP_RST_PANIC:    return "PANIC (crash/exception)";
    case ESP_RST_INT_WDT:  return "INT_WDT (watchdog interupsi)";
    case ESP_RST_TASK_WDT: return "TASK_WDT (watchdog tugas)";
    case ESP_RST_WDT:      return "WDT (watchdog lain)";
    case ESP_RST_BROWNOUT: return "BROWNOUT (tegangan catu turun)";
    default:               return "lainnya";
  }
}
