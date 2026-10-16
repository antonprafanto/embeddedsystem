/*
 * =====================================================================
 *  Pertemuan 10 · Praktikum 3 — Sensor Parkir: HC-SR04 + Buzzer + 3 LED
 * =====================================================================
 *  Tujuan   : Menggabungkan sensor dan aktuator: makin dekat objek, makin
 *             cepat buzzer berbunyi, dan LED menunjukkan zona jarak. Semua
 *             tugas berjalan bergantian dengan millis() (non-blocking).
 *  Rangkaian:
 *    HC-SR04 : VCC → VIN (5 V) · TRIG → GPIO 17 · ECHO → GPIO 16
 *              (board asli: lewat pembagi tegangan 1 kΩ/2 kΩ) · GND → GND
 *    Buzzer  : (+) → GPIO 18 · (−) → GND (buzzer pasif/piezo)
 *    LED     : merah GPIO 25, kuning GPIO 26, hijau GPIO 27, masing-masing
 *              lewat resistor 220 Ω; katoda ke GND
 *  Zona     : ≥ 100 cm atau tak terdeteksi = AMAN (hijau, buzzer diam)
 *             40–99 cm = WASPADA (kuning, bip makin cepat)
 *             10–39 cm = DEKAT   (merah, bip cepat)
 *             < 10 cm  = BERHENTI (merah, bunyi terus)
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan isi diagram.json,
 *                   klik ▶, lalu klik HC-SR04 dan geser jaraknya.
 *  Hasil yang diharapkan:
 *    LED dan bunyi bip berubah mengikuti jarak; tiap 1 detik Serial Monitor
 *    menampilkan jarak, zona, dan keadaan buzzer.
 * =====================================================================
 */
#include <Arduino.h>

// ---------- Pin ----------
const int PIN_TRIG = 17;
const int PIN_ECHO = 16;
const int PIN_BUZZER = 18;
const int PIN_LED_MERAH = 25;
const int PIN_LED_KUNING = 26;
const int PIN_LED_HIJAU = 27;

// ---------- Pengaturan ----------
const unsigned long INTERVAL_UKUR_MS = 100;    // datasheet: siklus ukur sebaiknya > 60 ms
const unsigned long INTERVAL_LAPOR_MS = 1000;  // laporan ke Serial tiap 1 s
const unsigned long BATAS_TUNGGU_US = 30000;   // ≈ 5 m; lebih lama = tak ada pantulan
const int FREK_BIP_HZ = 2000;                  // nada bip 2 kHz
const unsigned long LAMA_BIP_MS = 60;          // lama satu bunyi bip
const int FAKTOR_PERIODE = 8;                  // periode bip (ms) = jarak (cm) × 8
const int BATAS_AMAN_CM = 100;
const int BATAS_DEKAT_CM = 40;
const int BATAS_BERHENTI_CM = 10;
const int TIDAK_TERDETEKSI = -1;

// ---------- Variabel status ----------
int jarakCm = TIDAK_TERDETEKSI;
unsigned long waktuUkurTerakhir = 0;
unsigned long waktuLaporTerakhir = 0;
unsigned long waktuBipTerakhir = 0;
bool buzzerBunyi = false;

// ---------- Deklarasi fungsi ----------
int ukurJarakCm();
bool zonaAman();
void aturLed();
void aturBuzzer(unsigned long sekarang);
void nyalakanBuzzer(bool bunyi);
const char *namaZona();
void laporkan();

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_LED_MERAH, OUTPUT);
  pinMode(PIN_LED_KUNING, OUTPUT);
  pinMode(PIN_LED_HIJAU, OUTPUT);
  ledcAttach(PIN_BUZZER, FREK_BIP_HZ, 8);      // buzzer pasif digerakkan PWM (LEDC)
  ledcWriteTone(PIN_BUZZER, 0);                // mulai dalam keadaan diam
  Serial.println("[INFO] Sensor parkir siap. Geser jarak HC-SR04 untuk mencoba.");
}

void loop() {
  unsigned long sekarang = millis();

  // Tugas 1: ukur jarak tiap 100 ms, lalu perbarui LED
  if (sekarang - waktuUkurTerakhir >= INTERVAL_UKUR_MS) {
    waktuUkurTerakhir = sekarang;
    jarakCm = ukurJarakCm();
    aturLed();
  }

  // Tugas 2: atur bunyi bip (diperiksa di setiap putaran loop)
  aturBuzzer(sekarang);

  // Tugas 3: laporan ke Serial Monitor tiap 1 detik
  if (sekarang - waktuLaporTerakhir >= INTERVAL_LAPOR_MS) {
    waktuLaporTerakhir = sekarang;
    laporkan();
  }
}

int ukurJarakCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);                // pulsa trigger 10 µs
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  unsigned long durasiUs = pulseIn(PIN_ECHO, HIGH, BATAS_TUNGGU_US);
  if (durasiUs == 0) return TIDAK_TERDETEKSI;  // tidak ada pantulan
  int jarak = (int)lround(durasiUs / 58.0);   // dibulatkan ke cm terdekat
  if (jarak > 400) return TIDAK_TERDETEKSI;    // melebihi jangkauan datasheet
  return jarak;                                // < 2 cm tetap dianggap sangat dekat (lebih aman)
}

bool zonaAman() {
  return jarakCm == TIDAK_TERDETEKSI || jarakCm >= BATAS_AMAN_CM;
}

void aturLed() {
  bool aman = zonaAman();
  bool waspada = !aman && jarakCm >= BATAS_DEKAT_CM;
  bool dekat = !aman && !waspada;
  digitalWrite(PIN_LED_HIJAU, aman);
  digitalWrite(PIN_LED_KUNING, waspada);
  digitalWrite(PIN_LED_MERAH, dekat);
}

void aturBuzzer(unsigned long sekarang) {
  if (zonaAman()) {
    nyalakanBuzzer(false);                     // aman → diam
    return;
  }
  if (jarakCm < BATAS_BERHENTI_CM) {
    nyalakanBuzzer(true);                      // sangat dekat → bunyi terus
    return;
  }
  unsigned long periodeMs = (unsigned long)jarakCm * FAKTOR_PERIODE;  // makin dekat → makin cepat
  if (sekarang - waktuBipTerakhir >= periodeMs) {
    waktuBipTerakhir = sekarang;               // saatnya bip baru
    nyalakanBuzzer(true);
  } else if (sekarang - waktuBipTerakhir >= LAMA_BIP_MS) {
    nyalakanBuzzer(false);                     // bip selesai, diam sampai periode berikutnya
  }
}

void nyalakanBuzzer(bool bunyi) {
  if (bunyi == buzzerBunyi) return;            // keadaan sama → tidak perlu diubah
  buzzerBunyi = bunyi;
  ledcWriteTone(PIN_BUZZER, bunyi ? FREK_BIP_HZ : 0);
}

const char *namaZona() {
  if (zonaAman()) return "AMAN";
  if (jarakCm >= BATAS_DEKAT_CM) return "WASPADA";
  if (jarakCm >= BATAS_BERHENTI_CM) return "DEKAT";
  return "BERHENTI";
}

void laporkan() {
  Serial.print("[DATA] Jarak: ");
  if (jarakCm == TIDAK_TERDETEKSI) {
    Serial.print("tidak terdeteksi");
  } else {
    Serial.print(jarakCm);
    Serial.print(" cm");
  }
  Serial.print(" | Zona: ");
  Serial.print(namaZona());
  Serial.print(" | Buzzer: ");
  if (zonaAman()) {
    Serial.println("diam");
  } else if (jarakCm < BATAS_BERHENTI_CM) {
    Serial.println("bunyi terus");
  } else {
    Serial.print("bip tiap ");
    Serial.print(jarakCm * FAKTOR_PERIODE);
    Serial.println(" ms");
  }
}
