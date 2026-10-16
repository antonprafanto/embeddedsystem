/*
 * =====================================================================
 *  Pertemuan 05 · Praktikum 3 — Timer Perangkat Keras
 * =====================================================================
 *  Tujuan   : Memakai timer perangkat keras ESP32 untuk memanggil ISR
 *             tepat setiap 500 ms. ISR membalik LED hijau dan mencatat
 *             waktu; loop() yang mencetak laporannya.
 *  Rangkaian: LED hijau → resistor 220 Ω → GPIO 19, katoda ke GND
 *             (LED merah dan tombol boleh tetap terpasang, tidak dipakai)
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200 baud).
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan isi diagram.json,
 *                   lalu klik ▶.
 *  Hasil yang diharapkan:
 *    LED hijau berganti setiap 0,5 detik dan Serial Monitor menampilkan:
 *      [TIMER] Tik ke-2 | LED hijau: PADAM | selang 500000 us
 *    Selang bisa meleset sedikit (jitter); itu wajar.
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_LED_HIJAU = 19;

const uint32_t FREKUENSI_TIMER = 1000000;  // 1 MHz → 1 tik = 1 mikrodetik
const uint64_t ALARM_TIK       = 500000;   // 500.000 tik × 1 us = 500 ms

// Ubah ke 1700 untuk meniru loop() yang sibuk (lihat Eksperimen kecil)
const unsigned long BEBAN_LOOP_MS = 0;

hw_timer_t *timerKu = NULL;                // pegangan (handle) timer
portMUX_TYPE kunci = portMUX_INITIALIZER_UNLOCKED;

// Variabel bersama ISR <-> loop()
volatile unsigned long jumlahTik  = 0;     // sudah berapa kali alarm berbunyi
volatile unsigned long waktuTikUs = 0;     // micros() saat tik terakhir
volatile unsigned long selangUs   = 0;     // jarak antara dua tik terakhir
volatile bool ledNyala = false;

unsigned long tikSebelumnya = 0;           // hanya dipakai loop()

// ISR timer: dipanggil perangkat keras setiap 500.000 tik
void IRAM_ATTR isrTimer() {
  unsigned long sekarangUs = micros();
  portENTER_CRITICAL_ISR(&kunci);
  ledNyala = !ledNyala;
  selangUs = sekarangUs - waktuTikUs;
  waktuTikUs = sekarangUs;
  jumlahTik = jumlahTik + 1;
  portEXIT_CRITICAL_ISR(&kunci);
  digitalWrite(PIN_LED_HIJAU, ledNyala ? HIGH : LOW);  // boleh di ISR: cepat
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED_HIJAU, OUTPUT);

  Serial.println();
  Serial.println("=== Pertemuan 05 · Timer Perangkat Keras ===");
  Serial.println("Timer 1 MHz, alarm tiap 500000 tik (500 ms). LED hijau dibalik oleh ISR.");

  waktuTikUs = micros();
  timerKu = timerBegin(FREKUENSI_TIMER);        // 1) buat timer & mulai menghitung
  if (timerKu == NULL) {
    Serial.println("[PERINGATAN] Timer gagal dibuat!");
    return;
  }
  timerAttachInterrupt(timerKu, &isrTimer);     // 2) pasang ISR
  timerAlarm(timerKu, ALARM_TIK, true, 0);      // 3) alarm 500.000 tik, ulangi terus
}

void loop() {
  // Salin data bersama di dalam bagian kritis
  portENTER_CRITICAL(&kunci);
  unsigned long tik    = jumlahTik;
  unsigned long selang = selangUs;
  bool led             = ledNyala;
  portEXIT_CRITICAL(&kunci);

  if (tik != tikSebelumnya) {                   // ada tik baru sejak laporan terakhir
    tikSebelumnya = tik;
    if (tik == 1) {
      Serial.printf("[TIMER] Tik ke-%lu | LED hijau: %s | (tik pertama)\n",
                    tik, led ? "NYALA" : "PADAM");
    } else {
      Serial.printf("[TIMER] Tik ke-%lu | LED hijau: %s | selang %lu us\n",
                    tik, led ? "NYALA" : "PADAM", selang);
    }
    delay(BEBAN_LOOP_MS);                       // 0 = loop() tidak sibuk
  }
}
