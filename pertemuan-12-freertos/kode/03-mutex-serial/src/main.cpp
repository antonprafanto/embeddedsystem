/*
 * =====================================================================
 *  Pertemuan 12 · Praktikum 3 — Mutex untuk Serial Bersama
 * =====================================================================
 *  Tujuan   : Melihat race condition secara langsung. Dua task mencetak
 *             baris panjang ke Serial yang sama:
 *             - PAKAI_MUTEX = false → huruf kedua task bercampur aduk,
 *             - PAKAI_MUTEX = true  → setiap baris tercetak utuh.
 *  Rangkaian: Tidak perlu komponen tambahan.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200 baud).
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan diagram.json, klik ▶.
 *    Jalankan sekali dengan PAKAI_MUTEX = false, lalu ubah ke true dan
 *    jalankan lagi. Bandingkan hasilnya.
 *  Hasil yang diharapkan:
 *    false → huruf dari task A dan task B bercampur dalam satu baris,
 *    true  → setiap baris utuh (urutan baris A dan B bisa bertukar), misalnya
 *            "[A01] Task A melapor: suhu ruang 27 C, normal."
 * =====================================================================
 */
#include <Arduino.h>

const bool PAKAI_MUTEX = false;   // ← ubah ke true, lalu bandingkan hasilnya
const int JUMLAH_BARIS = 3;       // tiap task mencetak 3 baris

SemaphoreHandle_t kunciSerial = NULL;   // mutex = "kunci" Serial Monitor

// Data untuk tiap task: kode huruf dan isi pesannya
struct InfoPencetak {
  char kode;
  const char *pesan;
};

InfoPencetak infoA = {'A', "Task A melapor: suhu ruang 27 C, normal."};
InfoPencetak infoB = {'B', "TASK B MELAPOR: PINTU GARASI TERKUNCI."};

void taskPencetak(void *parameter);
void tampilkan(const char *teks);
void cetakPelan(const char *teks);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== Pertemuan 12 · Mutex Serial ===");
  if (PAKAI_MUTEX) {
    Serial.println("[INFO] Mode: DENGAN mutex (setiap baris utuh)");
  } else {
    Serial.println("[INFO] Mode: TANPA mutex (baris bisa bercampur)");
  }

  kunciSerial = xSemaphoreCreateMutex();   // buat kunci SEBELUM task dibuat
  if (kunciSerial == NULL) {
    Serial.println("[GALAT] Mutex gagal dibuat (memori habis?)");
    return;
  }

  // Fungsi task yang SAMA dipakai dua kali, dibedakan lewat parameter
  xTaskCreatePinnedToCore(taskPencetak, "PencetakA", 3072, &infoA, 1, NULL, 0);
  xTaskCreatePinnedToCore(taskPencetak, "PencetakB", 3072, &infoB, 1, NULL, 1);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));   // loopTask tidak punya pekerjaan
}

void taskPencetak(void *parameter) {
  InfoPencetak *info = (InfoPencetak *)parameter;   // ambil data milik task ini
  char baris[64];
  for (int nomor = 1; nomor <= JUMLAH_BARIS; nomor++) {
    snprintf(baris, sizeof(baris), "[%c%02d] %s", info->kode, nomor, info->pesan);
    tampilkan(baris);
    vTaskDelay(pdMS_TO_TICKS(300));
  }
  snprintf(baris, sizeof(baris), "[INFO] Task %c selesai.", info->kode);
  tampilkan(baris);
  vTaskDelete(NULL);   // task boleh berakhir, asal menghapus dirinya sendiri
}

// Semua akses ke Serial lewat fungsi ini. Bila PAKAI_MUTEX = true,
// hanya task pemegang kunci yang boleh mencetak.
void tampilkan(const char *teks) {
  if (PAKAI_MUTEX) {
    xSemaphoreTake(kunciSerial, portMAX_DELAY);   // ambil kunci (tunggu bila dipakai)
    cetakPelan(teks);                             // bagian kritis: hanya satu task
    xSemaphoreGive(kunciSerial);                  // kembalikan kunci!
  } else {
    cetakPelan(teks);                             // tanpa kunci: bisa bertabrakan
  }
}

// Mencetak huruf demi huruf dengan jeda 1 ms, meniru perangkat keluaran
// yang lambat (misalnya printer atau layar). Satu baris = banyak panggilan
// Serial.write(), sehingga task lain bisa "menyela" di tengah baris.
void cetakPelan(const char *teks) {
  for (int i = 0; teks[i] != '\0'; i++) {
    Serial.write(teks[i]);
    vTaskDelay(pdMS_TO_TICKS(1));
  }
  Serial.write('\n');
}
