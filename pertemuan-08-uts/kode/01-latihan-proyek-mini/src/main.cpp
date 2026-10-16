/*
 * =====================================================================
 *  Pertemuan 08 · Latihan Proyek Mini — Lampu Penyeberangan Pintar
 * =====================================================================
 *  Tujuan   : Latihan menjelang UTS yang memadukan materi P03–P07 dalam
 *             satu program TANPA delay(): GPIO output, tombol dengan
 *             interupsi + debounce, pewaktuan millis(), ADC, dan nada PWM.
 *  Cerita   : Lampu kendaraan tetap HIJAU sampai pejalan kaki menekan
 *             tombol. Hijau dipertahankan minimal T_hijau (5–20 detik,
 *             diatur potensiometer). Lalu KUNING 3 detik, MERAH 10 detik
 *             sambil buzzer berbunyi "bip" berirama (lebih cepat pada
 *             3 detik terakhir), JEDA AMAN 2 detik, lalu kembali HIJAU.
 *  Rangkaian:
 *    - LED merah, kuning, hijau : GPIO 25, 26, 27 → resistor 220 Ω → anoda;
 *                                 katode ke GND
 *    - Tombol penyeberang       : GPIO 4 ↔ GND (INPUT_PULLUP, ditekan = LOW)
 *    - Buzzer pasif             : (+) GPIO 18, (−) GND
 *    - Potensiometer 10 kΩ      : SIG → GPIO 34, VCC → 3V3 (bukan 5 V!), GND → GND
 *    - LED biru bawaan GPIO 2   : menyala selama permintaan menunggu dilayani
 *  Cara menjalankan:
 *    - Board asli : rangkai sesuai gambar → Build (✓) → Upload (→) → Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan diagram.json ke tab
 *                   diagram.json, lalu klik ▶. Klik tombol hijau untuk menyeberang.
 *  Hasil yang diharapkan:
 *    Lampu hijau menyala. Tekan tombol → LED biru menyala. Setelah T_hijau:
 *    kuning 3 s → merah 10 s + bunyi bip → merah 2 s → hijau lagi.
 *    Serial Monitor mencetak nilai T_hijau dan setiap pergantian tahap.
 * =====================================================================
 */
#include <Arduino.h>

// ----- Pin (mengikuti peta pin standar kursus) -----
const int PIN_MERAH  = 25;
const int PIN_KUNING = 26;
const int PIN_HIJAU  = 27;
const int PIN_TOMBOL = 4;
const int PIN_BUZZER = 18;
const int PIN_POT    = 34;   // ADC1, pin input saja
const int PIN_TANDA  = 2;    // LED biru bawaan: "permintaan tercatat, tunggu"

// ----- Pengaturan waktu (milidetik) -----
const unsigned long HIJAU_MIN_MS     = 5000;    // kenop diputar habis ke kiri
const unsigned long HIJAU_MAKS_MS    = 20000;   // kenop diputar habis ke kanan
const unsigned long KUNING_MS        = 3000;
const unsigned long MENYEBERANG_MS   = 10000;
const unsigned long PERINGATAN_MS    = 3000;    // 3 detik terakhir: bip lebih cepat
const unsigned long JEDA_AMAN_MS     = 2000;    // semua merah sebelum hijau lagi
const unsigned long DEBOUNCE_MS      = 50;      // tombol dianggap tenang setelah 50 ms
const unsigned long JEDA_BACA_POT_MS = 200;     // baca potensiometer 5 kali per detik

// ----- Buzzer dan potensiometer -----
const uint32_t NADA_BIP_HZ               = 1000;
const unsigned long LAMA_BIP_MS          = 100;   // lama satu bunyi "bip"
const unsigned long PERIODE_BIP_MS       = 500;   // 2 bip per detik
const unsigned long PERIODE_BIP_CEPAT_MS = 200;   // 5 bip per detik
const int AMBANG_UBAH_POT = 40;   // abaikan perubahan < 40 (±1%) agar derau tidak membanjiri Serial

enum Tahap { TAHAP_HIJAU, TAHAP_KUNING, TAHAP_MENYEBERANG, TAHAP_JEDA_AMAN };

// ----- Variabel bersama ISR dan loop(): wajib volatile -----
volatile bool adaTepiTombol = false;         // ISR: "ada tepi turun"
volatile unsigned long waktuTepiTombol = 0;  // kapan tepi terakhir terjadi (ms)
portMUX_TYPE kunci = portMUX_INITIALIZER_UNLOCKED;

// ----- Keadaan program -----
Tahap tahap = TAHAP_HIJAU;
unsigned long mulaiTahap = 0;               // kapan tahap sekarang dimulai
unsigned long durasiHijauMs = HIJAU_MIN_MS; // T_hijau dari potensiometer
unsigned long waktuBacaPot = 0;
int nilaiPotTerakhir = -1000;               // nilai mustahil → cetakan pertama pasti muncul
bool permintaanMenyeberang = false;
bool peringatanDiumumkan = false;
bool buzzerBunyi = false;

// ----- Prototipe fungsi -----
void isrTombol();                     // IRAM_ATTR cukup ditulis pada definisinya
bool ambilTekananTombol();
void tanganiTombol();
void bacaPotensiometer(unsigned long sekarang);
void perbaruiDurasiHijau();
void masukTahap(Tahap baru, unsigned long sekarang);
void aturLampu(bool merah, bool kuning, bool hijau);
void aturIramaBuzzer(unsigned long lamaTahap);
void bunyikanBuzzer(bool bunyi);

// ISR sesingkat mungkin: hanya mencatat "ada tepi turun" dan kapan terjadinya.
// Setiap pantulan memperbarui waktunya, sehingga loop() bisa menunggu sampai tenang.
void IRAM_ATTR isrTombol() {
  unsigned long sekarang = millis();
  portENTER_CRITICAL_ISR(&kunci);
  adaTepiTombol = true;
  waktuTepiTombol = sekarang;
  portEXIT_CRITICAL_ISR(&kunci);
}

// Mengembalikan true SEKALI untuk setiap tekanan yang sah (debounce berbasis waktu).
bool ambilTekananTombol() {
  portENTER_CRITICAL(&kunci);
  bool ada = adaTepiTombol;
  unsigned long waktuTepi = waktuTepiTombol;
  portEXIT_CRITICAL(&kunci);

  // millis() dibaca SETELAH menyalin waktuTepi, jadi selisihnya tidak pernah "negatif".
  if (!ada || millis() - waktuTepi < DEBOUNCE_MS) {
    return false;                       // belum ada tepi, atau pantulan belum reda
  }
  portENTER_CRITICAL(&kunci);
  adaTepiTombol = false;                // tepi ini sudah diperiksa
  portEXIT_CRITICAL(&kunci);

  // Sah hanya jika tombol MASIH ditekan setelah pantulan reda.
  // Tepi turun palsu saat tombol DILEPAS akan gagal di pemeriksaan ini.
  return digitalRead(PIN_TOMBOL) == LOW;
}

void tanganiTombol() {
  if (tahap != TAHAP_HIJAU) {
    Serial.println("[TOMBOL] Diabaikan: siklus penyeberangan sedang berjalan.");
  } else if (permintaanMenyeberang) {
    Serial.println("[TOMBOL] Permintaan sudah tercatat, harap tunggu.");
  } else {
    permintaanMenyeberang = true;
    digitalWrite(PIN_TANDA, HIGH);      // tanda "tunggu" untuk pejalan kaki
    Serial.println("[TOMBOL] Permintaan menyeberang tercatat.");
  }
}

void bacaPotensiometer(unsigned long sekarang) {
  if (sekarang - waktuBacaPot < JEDA_BACA_POT_MS) {
    return;                             // belum waktunya membaca lagi
  }
  waktuBacaPot = sekarang;
  perbaruiDurasiHijau();
}

void perbaruiDurasiHijau() {
  int nilai = analogRead(PIN_POT);      // 0–4095 (12 bit)
  if (abs(nilai - nilaiPotTerakhir) < AMBANG_UBAH_POT) {
    return;                             // perubahan kecil dianggap derau
  }
  nilaiPotTerakhir = nilai;
  durasiHijauMs = map(nilai, 0, 4095, HIJAU_MIN_MS, HIJAU_MAKS_MS);
  Serial.printf("[INFO] Durasi hijau minimum: %.1f detik\n", durasiHijauMs / 1000.0);
}

void aturLampu(bool merah, bool kuning, bool hijau) {
  digitalWrite(PIN_MERAH, merah ? HIGH : LOW);
  digitalWrite(PIN_KUNING, kuning ? HIGH : LOW);
  digitalWrite(PIN_HIJAU, hijau ? HIGH : LOW);
}

// Semua aksi "sekali saat masuk tahap" dikumpulkan di sini.
void masukTahap(Tahap baru, unsigned long sekarang) {
  tahap = baru;
  mulaiTahap = sekarang;
  switch (baru) {
    case TAHAP_HIJAU:
      aturLampu(false, false, true);
      Serial.println("[TAHAP] HIJAU       : kendaraan jalan; tekan tombol untuk menyeberang");
      break;
    case TAHAP_KUNING:
      aturLampu(false, true, false);
      Serial.println("[TAHAP] KUNING      : kendaraan bersiap berhenti");
      break;
    case TAHAP_MENYEBERANG:
      aturLampu(true, false, false);
      permintaanMenyeberang = false;    // permintaan sedang dilayani
      digitalWrite(PIN_TANDA, LOW);
      peringatanDiumumkan = false;
      Serial.println("[TAHAP] MENYEBERANG : kendaraan berhenti, pejalan kaki menyeberang");
      break;
    case TAHAP_JEDA_AMAN:
      aturLampu(true, false, false);
      bunyikanBuzzer(false);
      Serial.println("[TAHAP] JEDA AMAN   : semua merah, penyeberangan dikosongkan");
      break;
  }
}

void aturIramaBuzzer(unsigned long lamaTahap) {
  unsigned long periode = PERIODE_BIP_MS;
  if (lamaTahap >= MENYEBERANG_MS - PERINGATAN_MS) {
    periode = PERIODE_BIP_CEPAT_MS;     // tanda waktu menyeberang hampir habis
  }
  // Sisa bagi (%) membuat pola berulang: berbunyi di awal setiap periode selama LAMA_BIP_MS.
  bunyikanBuzzer(lamaTahap % periode < LAMA_BIP_MS);
}

void bunyikanBuzzer(bool bunyi) {
  if (bunyi == buzzerBunyi) {
    return;                             // ubah hanya saat perlu: ledcWriteTone() mengatur ulang timer
  }
  buzzerBunyi = bunyi;
  ledcWriteTone(PIN_BUZZER, bunyi ? NADA_BIP_HZ : 0);   // frekuensi 0 = diam
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_MERAH, OUTPUT);
  pinMode(PIN_KUNING, OUTPUT);
  pinMode(PIN_HIJAU, OUTPUT);
  pinMode(PIN_TANDA, OUTPUT);
  pinMode(PIN_TOMBOL, INPUT_PULLUP);
  analogReadResolution(12);

  if (!ledcAttach(PIN_BUZZER, NADA_BIP_HZ, 10)) {
    Serial.println("[PERINGATAN] Buzzer gagal disiapkan (ledcAttach).");
  }
  ledcWriteTone(PIN_BUZZER, 0);         // mulai dalam keadaan diam
  attachInterrupt(digitalPinToInterrupt(PIN_TOMBOL), isrTombol, FALLING);

  Serial.println("[INFO] Lampu penyeberangan pintar siap.");
  perbaruiDurasiHijau();
  masukTahap(TAHAP_HIJAU, millis());
}

void loop() {
  unsigned long sekarang = millis();
  bacaPotensiometer(sekarang);
  if (ambilTekananTombol()) {
    tanganiTombol();
  }

  unsigned long lamaTahap = sekarang - mulaiTahap;   // aman walau millis() meluap
  switch (tahap) {
    case TAHAP_HIJAU:
      if (permintaanMenyeberang && lamaTahap >= durasiHijauMs) {
        masukTahap(TAHAP_KUNING, sekarang);
      }
      break;
    case TAHAP_KUNING:
      if (lamaTahap >= KUNING_MS) {
        masukTahap(TAHAP_MENYEBERANG, sekarang);
      }
      break;
    case TAHAP_MENYEBERANG:
      if (lamaTahap >= MENYEBERANG_MS) {
        masukTahap(TAHAP_JEDA_AMAN, sekarang);
        break;
      }
      aturIramaBuzzer(lamaTahap);
      if (!peringatanDiumumkan && lamaTahap >= MENYEBERANG_MS - PERINGATAN_MS) {
        peringatanDiumumkan = true;
        Serial.println("[INFO] Sisa 3 detik: bip dipercepat");
      }
      break;
    case TAHAP_JEDA_AMAN:
      if (lamaTahap >= JEDA_AMAN_MS) {
        masukTahap(TAHAP_HIJAU, sekarang);
      }
      break;
  }
}
