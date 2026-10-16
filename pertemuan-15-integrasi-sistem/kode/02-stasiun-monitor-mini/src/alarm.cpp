#include "alarm.h"
#include "konfigurasi.h"

static StatusAlarm status = StatusAlarm::MENUNGGU;   // keadaan awal: belum ada data
static unsigned long waktuMasukStatus = 0;           // kapan status sekarang dimulai
static bool bunyiSebelumnya = false;

void alarmMulai() {
  pinMode(PIN_LED_MERAH, OUTPUT);
  pinMode(PIN_LED_KUNING, OUTPUT);
  pinMode(PIN_LED_HIJAU, OUTPUT);
  ledcAttach(PIN_BUZZER, FREKUENSI_BUZZER, 8);       // PWM 2 kHz, resolusi 8 bit
  ledcWrite(PIN_BUZZER, 0);
  waktuMasukStatus = millis();
}

// Batas naik dan batas turun sengaja dibuat berbeda (histeresis) supaya status
// tidak berganti-ganti ketika suhu bergoyang tipis di sekitar batas.
static StatusAlarm hitungStatus(float suhu, float ambang) {
  bool sedangBahaya = (status == StatusAlarm::BAHAYA);
  bool sedangWaspada = sedangBahaya || (status == StatusAlarm::MENDEKATI);
  float batasBahaya = sedangBahaya ? ambang - HISTERESIS : ambang;
  float batasWaspada = sedangWaspada ? ambang - JARAK_WASPADA - HISTERESIS
                                     : ambang - JARAK_WASPADA;
  if (suhu >= batasBahaya) return StatusAlarm::BAHAYA;
  if (suhu >= batasWaspada) return StatusAlarm::MENDEKATI;
  return StatusAlarm::NORMAL;
}

bool alarmPerbarui(bool sensorValid, int gagalBerturut, float suhu, float ambang) {
  StatusAlarm baru = status;
  if (!sensorValid) {
    // Sekali gagal belum tentu rusak; baru dianggap galat setelah 3× berturut-turut
    if (gagalBerturut >= BATAS_GAGAL_BACA) baru = StatusAlarm::SENSOR_GALAT;
  } else {
    baru = hitungStatus(suhu, ambang);
  }
  if (baru == status) return false;
  status = baru;
  waktuMasukStatus = millis();
  return true;
}

StatusAlarm alarmStatus() {
  return status;
}

const char *alarmNamaStatus(StatusAlarm s) {
  switch (s) {
    case StatusAlarm::MENUNGGU:     return "MENUNGGU";
    case StatusAlarm::NORMAL:       return "NORMAL";
    case StatusAlarm::MENDEKATI:    return "MENDEKATI";
    case StatusAlarm::BAHAYA:       return "BAHAYA";
    case StatusAlarm::SENSOR_GALAT: return "SENSOR_GALAT";
  }
  return "?";
}

void alarmJalankanKeluaran() {
  unsigned long lama = millis() - waktuMasukStatus;   // sudah berapa lama di status ini
  bool merah = false, kuning = false, hijau = false, bunyi = false;
  switch (status) {
    case StatusAlarm::MENUNGGU:     break;                                // semua padam
    case StatusAlarm::NORMAL:       hijau = true; break;
    case StatusAlarm::MENDEKATI:    kuning = true; break;
    case StatusAlarm::BAHAYA:       merah = true; bunyi = (lama % 1000) < 200; break;  // bip 0,2 s tiap 1 s
    case StatusAlarm::SENSOR_GALAT: kuning = (lama % 500) < 250; break;   // kuning berkedip 2 Hz
  }
  digitalWrite(PIN_LED_MERAH, merah);
  digitalWrite(PIN_LED_KUNING, kuning);
  digitalWrite(PIN_LED_HIJAU, hijau);
  if (bunyi != bunyiSebelumnya) {                     // tulis PWM hanya saat berubah
    ledcWrite(PIN_BUZZER, bunyi ? 128 : 0);           // 128/255 ≈ 50% → nada 2 kHz
    bunyiSebelumnya = bunyi;
  }
}
