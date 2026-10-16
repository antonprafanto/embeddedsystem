/*
 * =====================================================================
 *  Pertemuan 05 · Praktikum 4 — Gim Waktu Reaksi
 * =====================================================================
 *  Tujuan   : Memadukan millis(), micros(), dan interupsi dalam sebuah gim.
 *             LED merah menyala setelah jeda acak 2–5 detik; tekan tombol
 *             secepatnya. Waktu reaksi diukur dengan ketelitian mikrodetik.
 *             Menekan sebelum LED menyala (atau < 100 ms) = curang.
 *  Rangkaian: LED merah → resistor 220 Ω → GPIO 23, katoda ke GND
 *             LED hijau → resistor 220 Ω → GPIO 19, katoda ke GND
 *             Tombol    → GPIO 4 dan GND (memakai INPUT_PULLUP)
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (115200 baud).
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan isi diagram.json,
 *                   lalu klik ▶.
 *  Hasil yang diharapkan (angka waktu berbeda-beda):
 *      [RONDE 1] Bersiap... tunggu LED merah menyala.
 *      [RONDE 1] LED MENYALA! Tekan sekarang!
 *      [HASIL] Waktu reaksi: 287 ms (287412 us)
 *      [SKOR] Rekor: 287 ms | rata-rata: 287 ms | ronde sah: 1
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_LED_MERAH = 23;   // rangsangan: tanda untuk menekan
const int PIN_LED_HIJAU = 19;   // menyala sebentar bila hasil sah
const int PIN_TOMBOL    = 4;

const unsigned long JEDA_MIN_MS       = 2000;     // jeda acak sebelum LED menyala
const unsigned long JEDA_MAKS_MS      = 5000;
const unsigned long BATAS_MANUSIA_US  = 100000;   // < 100 ms = menebak, bukan bereaksi
const unsigned long BATAS_LAMBAT_US   = 3000000;  // > 3 detik = ronde hangus
const unsigned long LAMA_ISTIRAHAT_MS = 2000;     // jeda antar-ronde
const unsigned long TOMBOL_LEPAS_MS   = 100;      // tombol harus sudah dilepas

// Tahapan (keadaan) gim — cikal bakal FSM di Pertemuan 11
enum Keadaan { BERSIAP, MENUNGGU_TEKAN, ISTIRAHAT };
Keadaan keadaan = ISTIRAHAT;

// ----- Variabel bersama ISR <-> loop() -----
portMUX_TYPE kunci = portMUX_INITIALIZER_UNLOCKED;
volatile bool adaTekan = false;              // ada tekanan yang belum diproses?
volatile unsigned long waktuTekanUs = 0;     // micros() saat tombol ditekan

// ----- Variabel milik loop() -----
unsigned long tekanUs = 0;                   // salinan waktuTekanUs
unsigned long nomorRonde = 0;
bool rondeDiulang = false;
unsigned long waktuTahapMs = 0;              // kapan tahap saat ini dimulai
unsigned long jedaAcakMs = 0;
unsigned long waktuNyalaUs = 0;              // micros() saat LED merah menyala
unsigned long waktuTerakhirDitekanMs = 0;

unsigned long rekorMs = 0;
unsigned long totalMs = 0;
unsigned long jumlahSah = 0;

// Deklarasi fungsi
bool adaTekanBaru();
void mulaiRonde();
void masukIstirahat(bool ulangiRonde);
void catatHasil(long reaksiUs);

// ISR: cukup catat WAKTU tekanan pertama; pantulan sesudahnya diabaikan
void IRAM_ATTR isrTombol() {
  unsigned long t = micros();
  portENTER_CRITICAL_ISR(&kunci);
  if (!adaTekan) {
    adaTekan = true;
    waktuTekanUs = t;
  }
  portEXIT_CRITICAL_ISR(&kunci);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED_MERAH, OUTPUT);
  pinMode(PIN_LED_HIJAU, OUTPUT);
  pinMode(PIN_TOMBOL, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_TOMBOL), isrTombol, FALLING);

  Serial.println();
  Serial.println("=== Pertemuan 05 · Gim Waktu Reaksi ===");
  Serial.println("Tekan tombol SETELAH LED merah menyala. Jangan menebak!");
  mulaiRonde();
}

void loop() {
  unsigned long sekarang = millis();
  if (digitalRead(PIN_TOMBOL) == LOW) {
    waktuTerakhirDitekanMs = sekarang;       // dipakai untuk memastikan tombol dilepas
  }

  switch (keadaan) {
    case BERSIAP:                            // menunggu jeda acak selesai
      if (adaTekanBaru()) {
        Serial.println("[CURANG] Kamu menekan sebelum LED menyala! Ronde diulang.");
        masukIstirahat(true);
      } else if (sekarang - waktuTahapMs >= jedaAcakMs) {
        digitalWrite(PIN_LED_MERAH, HIGH);
        waktuNyalaUs = micros();             // mulai "stopwatch"
        keadaan = MENUNGGU_TEKAN;
        Serial.printf("[RONDE %lu] LED MENYALA! Tekan sekarang!\n", nomorRonde);
      }
      break;

    case MENUNGGU_TEKAN:                     // LED menyala, menunggu reaksi
      if (adaTekanBaru()) {
        long reaksiUs = (long)(tekanUs - waktuNyalaUs);   // selisih aman luapan
        if (reaksiUs < (long)BATAS_MANUSIA_US) {
          if (reaksiUs < 0) reaksiUs = 0;
          Serial.printf("[CURANG] Reaksi %ld ms terlalu cepat untuk manusia. Ronde diulang.\n",
                        reaksiUs / 1000);
          masukIstirahat(true);
        } else {
          catatHasil(reaksiUs);
          masukIstirahat(false);
        }
      } else if (micros() - waktuNyalaUs > BATAS_LAMBAT_US) {
        Serial.println("[HASIL] Terlalu lama (lebih dari 3 detik). Ronde tidak dihitung.");
        masukIstirahat(false);
      }
      break;

    case ISTIRAHAT:                          // jeda antar-ronde
      if (sekarang - waktuTahapMs >= LAMA_ISTIRAHAT_MS &&
          sekarang - waktuTerakhirDitekanMs >= TOMBOL_LEPAS_MS) {
        mulaiRonde();
      }
      break;
  }
}

// Ambil (lalu hapus) catatan tekanan dari ISR. Waktunya disalin ke tekanUs.
bool adaTekanBaru() {
  portENTER_CRITICAL(&kunci);
  bool ada = adaTekan;
  tekanUs = waktuTekanUs;
  adaTekan = false;
  portEXIT_CRITICAL(&kunci);
  return ada;
}

void mulaiRonde() {
  if (!rondeDiulang) nomorRonde++;
  digitalWrite(PIN_LED_MERAH, LOW);
  digitalWrite(PIN_LED_HIJAU, LOW);
  jedaAcakMs = random(JEDA_MIN_MS, JEDA_MAKS_MS + 1);   // 2000–5000 ms
  waktuTahapMs = millis();
  adaTekanBaru();                            // buang tekanan lama (jika ada)
  keadaan = BERSIAP;
  Serial.printf("\n[RONDE %lu] Bersiap... tunggu LED merah menyala.\n", nomorRonde);
}

void masukIstirahat(bool ulangiRonde) {
  digitalWrite(PIN_LED_MERAH, LOW);
  rondeDiulang = ulangiRonde;
  waktuTahapMs = millis();
  keadaan = ISTIRAHAT;
}

void catatHasil(long reaksiUs) {
  unsigned long reaksiMs = (reaksiUs + 500) / 1000;  // dibulatkan ke ms terdekat
  jumlahSah++;
  totalMs += reaksiMs;
  if (jumlahSah == 1 || reaksiMs < rekorMs) {
    rekorMs = reaksiMs;
  }
  digitalWrite(PIN_LED_HIJAU, HIGH);
  Serial.printf("[HASIL] Waktu reaksi: %lu ms (%ld us)\n", reaksiMs, reaksiUs);
  Serial.printf("[SKOR] Rekor: %lu ms | rata-rata: %lu ms | ronde sah: %lu\n",
                rekorMs, totalMs / jumlahSah, jumlahSah);
}
