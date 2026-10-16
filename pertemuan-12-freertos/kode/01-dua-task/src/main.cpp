/*
 * =====================================================================
 *  Pertemuan 12 · Praktikum 1 — Dua Task di Dua Inti
 * =====================================================================
 *  Tujuan   : Membuat dua task FreeRTOS yang berjalan bersamaan:
 *             - task "Kedip" (inti 0) mengedipkan LED tiap 0,5 detik,
 *             - task "Lapor" (inti 1) mencetak laporan tiap 2 detik,
 *             lalu melihat nomor inti, prioritas, dan sisa stack-nya.
 *  Rangkaian: LED merah + resistor 220 ohm di GPIO 23
 *             (GPIO 23 → resistor → anoda LED; katoda LED → GND).
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200 baud).
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan diagram.json, klik ▶.
 *  Hasil yang diharapkan:
 *    LED berkedip (0,5 detik nyala, 0,5 detik padam) tanpa terganggu
 *    oleh pencetakan Serial. Serial Monitor menampilkan laporan tiap 2 detik:
 *      [Lapor] #1 | inti 1 | LED sudah menyala 1 kali | sisa stack: Kedip 1500 B, Lapor 3536 B
 *      [Lapor] #2 | inti 1 | LED sudah menyala 3 kali | sisa stack: Kedip 1500 B, Lapor 2628 B
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_LED = 23;                // LED eksternal di GPIO 23
const uint32_t JEDA_KEDIP_MS = 500;    // lama nyala = lama padam
const uint32_t JEDA_LAPOR_MS = 2000;   // laporan tiap 2 detik

// "Pegangan" (handle) untuk menunjuk task tertentu, misalnya saat
// menanyakan sisa stack-nya.
TaskHandle_t handleKedip = NULL;
TaskHandle_t handleLapor = NULL;

// Ditulis oleh task Kedip, dibaca oleh task Lapor. Aman tanpa mutex
// karena hanya SATU task yang menulis dan variabel 32-bit dibaca/ditulis
// dalam satu langkah oleh CPU ESP32.
volatile unsigned long jumlahNyala = 0;

// Deklarasi fungsi task (definisinya ada di bawah)
void taskKedip(void *parameter);
void taskLapor(void *parameter);

void setup() {
  Serial.begin(115200);
  delay(1000);                         // beri waktu Serial Monitor siap
  pinMode(PIN_LED, OUTPUT);

  Serial.println();
  Serial.println("=== Pertemuan 12 · Dua Task di Dua Inti ===");
  // setup() dan loop() sendiri dijalankan oleh task bernama "loopTask"
  Serial.printf("[INFO] setup() berjalan di task \"%s\", inti %d, prioritas %u\n",
                pcTaskGetName(NULL), xPortGetCoreID(), uxTaskPriorityGet(NULL));

  // xTaskCreatePinnedToCore(fungsi, nama, stack (BYTE), parameter,
  //                         prioritas, &handle, inti)
  BaseType_t hasil1 = xTaskCreatePinnedToCore(taskKedip, "Kedip", 2048, NULL, 1, &handleKedip, 0);
  BaseType_t hasil2 = xTaskCreatePinnedToCore(taskLapor, "Lapor", 4096, NULL, 1, &handleLapor, 1);
  if (hasil1 != pdPASS || hasil2 != pdPASS) {
    Serial.println("[GALAT] Task gagal dibuat: memori tidak cukup untuk stack.");
    return;
  }
  Serial.println("[INFO] Task Kedip (inti 0) dan task Lapor (inti 1) sudah dibuat.");
}

void loop() {
  // loopTask tetap hidup di inti 1. Ia tidur 10 detik, lalu menyapa.
  vTaskDelay(pdMS_TO_TICKS(10000));
  Serial.printf("[loop ] loopTask masih hidup di inti %d, sisa stack %u B\n",
                xPortGetCoreID(), uxTaskGetStackHighWaterMark(NULL));
}

// Task 1: mengedipkan LED. Pola wajib sebuah task: perulangan tanpa
// akhir yang selalu memberi jeda (vTaskDelay) dan tidak pernah return.
void taskKedip(void *parameter) {
  for (;;) {
    digitalWrite(PIN_LED, HIGH);
    jumlahNyala = jumlahNyala + 1;               // hanya task ini yang menulis
    vTaskDelay(pdMS_TO_TICKS(JEDA_KEDIP_MS));    // Blocked: CPU bebas dipakai task lain
    digitalWrite(PIN_LED, LOW);
    vTaskDelay(pdMS_TO_TICKS(JEDA_KEDIP_MS));
  }
}

// Task 2: mencetak laporan. Jeda awal 0,5 detik membuat laporan tidak
// muncul tepat bersamaan dengan saat LED menyala.
void taskLapor(void *parameter) {
  unsigned long nomor = 0;
  vTaskDelay(pdMS_TO_TICKS(500));
  for (;;) {
    nomor++;
    // Sisa stack paling sedikit sejak task dibuat (high water mark), dalam BYTE
    UBaseType_t sisaKedip = uxTaskGetStackHighWaterMark(handleKedip);
    UBaseType_t sisaLapor = uxTaskGetStackHighWaterMark(NULL);   // NULL = task ini
    Serial.printf("[Lapor] #%lu | inti %d | LED sudah menyala %lu kali | sisa stack: Kedip %u B, Lapor %u B\n",
                  nomor, xPortGetCoreID(), jumlahNyala, sisaKedip, sisaLapor);
    vTaskDelay(pdMS_TO_TICKS(JEDA_LAPOR_MS));
  }
}
