#include "ambang.h"
#include "konfigurasi.h"
#include <Preferences.h>

static Preferences penyimpanan;                 // akses NVS (Non-Volatile Storage) di flash
static const char NAMA_RUANG[] = "stasiun";     // namespace NVS (maks. 15 karakter)
static const char KUNCI_AMBANG[] = "ambang";    // nama kunci (maks. 15 karakter)
static float ambang = AMBANG_BAWAAN;
static float ambangTersimpan = AMBANG_BAWAAN;
static unsigned long waktuBacaPot = 0;

void ambangMulai() {
  penyimpanan.begin(NAMA_RUANG, false);         // false = boleh baca dan tulis
  if (penyimpanan.isKey(KUNCI_AMBANG)) {
    float nilai = penyimpanan.getFloat(KUNCI_AMBANG, AMBANG_BAWAAN);
    if (nilai >= AMBANG_MIN && nilai <= AMBANG_MAKS) ambang = nilai;   // tolak nilai aneh
    Serial.printf("[INFO] Ambang %.1f °C dimuat dari NVS.\n", ambang);
  } else {
    Serial.printf("[INFO] NVS masih kosong. Pakai ambang bawaan %.1f °C.\n", ambang);
  }
  ambangTersimpan = ambang;
}

float ambangNilai() {
  return ambang;
}

void ambangAturDariPot() {
  if (millis() - waktuBacaPot < INTERVAL_BACA_POT_MS) return;
  waktuBacaPot = millis();
  long jumlah = 0;
  for (int i = 0; i < 8; i++) jumlah += analogRead(PIN_POT);   // rata-rata 8 sampel → redam derau
  float rasio = (jumlah / 8) / 4095.0f;                         // 0,0 … 1,0
  float nilai = AMBANG_MIN + rasio * (AMBANG_MAKS - AMBANG_MIN);
  ambang = roundf(nilai * 2.0f) / 2.0f;                         // bulatkan ke kelipatan 0,5 °C
}

bool ambangSimpanBilaBerubah() {
  if (fabsf(ambang - ambangTersimpan) < 0.01f) return false;    // sama → jangan tulis flash
  penyimpanan.putFloat(KUNCI_AMBANG, ambang);
  ambangTersimpan = ambang;
  Serial.printf("[INFO] Ambang %.1f °C disimpan ke NVS.\n", ambang);
  return true;
}
