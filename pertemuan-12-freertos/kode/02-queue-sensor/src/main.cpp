/*
 * =====================================================================
 *  Pertemuan 12 · Praktikum 2 — Queue: dari Task Sensor ke Task Tampilan
 * =====================================================================
 *  Tujuan   : Menerapkan pola produsen–konsumen. Task "Baca" (inti 0)
 *             membaca potensiometer tepat tiap 0,5 detik lalu menaruh
 *             datanya di sebuah queue (antrean). Task "Tampil" (inti 1)
 *             menunggu data di antrean dan menampilkannya di Serial Monitor.
 *  Rangkaian: Potensiometer 10 kOhm: GND → GND, SIG (kaki tengah) → GPIO 34,
 *             VCC → 3V3.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200 baud).
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan diagram.json, klik ▶,
 *                   lalu klik potensiometer dan geser nilainya.
 *  Hasil yang diharapkan: satu baris tiap 0,5 detik, angkanya mengikuti
 *  posisi potensiometer, misalnya
 *    [Tampil] #3  t=2003 ms  ADC=2048  .... mV  [#####.....]  isi antrean: 0
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_POT = 34;                  // ADC1: tetap bisa dipakai saat Wi-Fi aktif
const int PANJANG_ANTREAN = 5;           // antrean muat 5 data
const uint32_t PERIODE_BACA_MS = 500;    // produsen membaca tepat tiap 0,5 detik
const uint32_t JEDA_TAMPIL_MS = 0;       // EKSPERIMEN: ubah ke 2000 (konsumen lambat)

// Satu "paket" data yang dikirim lewat antrean. Queue MENYALIN seluruh
// isi struct ini (bukan sekadar alamatnya), jadi aman dipakai lintas task.
struct DataSensor {
  unsigned long nomor;     // nomor urut pembacaan
  unsigned long waktuMs;   // kapan dibaca (millis)
  int adc;                 // nilai mentah 0–4095
  int milivolt;            // tegangan terkalibrasi (mV)
};

QueueHandle_t antreanSensor = NULL;

void taskBaca(void *parameter);
void taskTampil(void *parameter);
void buatBatang(int adc, char *teks, int lebar);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== Pertemuan 12 · Queue Sensor ===");

  // Buat antrean: 5 slot, tiap slot sebesar satu DataSensor
  antreanSensor = xQueueCreate(PANJANG_ANTREAN, sizeof(DataSensor));
  if (antreanSensor == NULL) {
    Serial.println("[GALAT] Antrean gagal dibuat (memori habis?)");
    while (true) {
      delay(1000);
    }
  }
  Serial.printf("[INFO] Antrean dibuat: %d slot x %u byte\n", PANJANG_ANTREAN, (unsigned)sizeof(DataSensor));

  // Produsen di inti 0 (prioritas 2), konsumen di inti 1 (prioritas 1)
  xTaskCreatePinnedToCore(taskBaca, "Baca", 3072, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(taskTampil, "Tampil", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // Semua pekerjaan sudah ditangani dua task di atas.
  // loopTask cukup tidur supaya tidak memakai CPU.
  vTaskDelay(pdMS_TO_TICKS(1000));
}

// PRODUSEN: membaca sensor lalu MENGIRIM ke antrean
void taskBaca(void *parameter) {
  unsigned long nomor = 0;
  TickType_t waktuBangun = xTaskGetTickCount();   // titik awal hitungan periode
  for (;;) {
    DataSensor data;
    data.nomor = ++nomor;
    data.waktuMs = millis();
    data.adc = analogRead(PIN_POT);
    data.milivolt = analogReadMilliVolts(PIN_POT);

    // Timeout 0: kalau antrean penuh, jangan menunggu; laporkan saja.
    if (xQueueSend(antreanSensor, &data, 0) != pdPASS) {
      Serial.printf("[Baca  ] PERINGATAN: antrean penuh, data #%lu dibuang\n", data.nomor);
    }
    // Tidur sampai TEPAT 500 ms sejak bangun sebelumnya. Lama membaca
    // sensor tidak menggeser periode (berbeda dengan vTaskDelay).
    xTaskDelayUntil(&waktuBangun, pdMS_TO_TICKS(PERIODE_BACA_MS));
  }
}

// KONSUMEN: MENUNGGU data di antrean lalu menampilkannya
void taskTampil(void *parameter) {
  DataSensor data;
  char batang[11];   // 10 karakter + penutup '\0'
  for (;;) {
    // portMAX_DELAY: tunggu selamanya (Blocked, tidak memakai CPU)
    if (xQueueReceive(antreanSensor, &data, portMAX_DELAY) == pdPASS) {
      buatBatang(data.adc, batang, 10);
      Serial.printf("[Tampil] #%lu  t=%lu ms  ADC=%4d  %4d mV  [%s]  isi antrean: %u\n",
                    data.nomor, data.waktuMs, data.adc, data.milivolt, batang,
                    (unsigned)uxQueueMessagesWaiting(antreanSensor));
      if (JEDA_TAMPIL_MS > 0) {
        vTaskDelay(pdMS_TO_TICKS(JEDA_TAMPIL_MS));   // pura-pura sibuk (eksperimen)
      }
    }
  }
}

// Mengubah nilai ADC 0–4095 menjadi batang 10 kotak, misalnya "#####....."
void buatBatang(int adc, char *teks, int lebar) {
  int isi = (adc * lebar + 2047) / 4095;   // dibulatkan ke terdekat
  if (isi < 0) isi = 0;
  if (isi > lebar) isi = lebar;
  for (int i = 0; i < lebar; i++) {
    teks[i] = (i < isi) ? '#' : '.';
  }
  teks[lebar] = '\0';
}
