// =====================================================================
//  tombol.cpp — membaca tombol tanpa delay() (debounce non-blocking)
// =====================================================================
#include "tombol.h"
#include "konfigurasi.h"

// "static" di luar fungsi = hanya terlihat di berkas ini (data pribadi modul).
// Tidak ada berkas lain yang bisa mengacaukannya secara tidak sengaja.
static int levelStabil = HIGH;        // HIGH = tidak ditekan (INPUT_PULLUP)
static int levelTerakhir = HIGH;      // level mentah pada pembacaan sebelumnya
static unsigned long waktuBerubah = 0;

void tombolMulai() {
  pinMode(PIN_TOMBOL, INPUT_PULLUP);
}

bool tombolDitekan() {
  int level = digitalRead(PIN_TOMBOL);
  if (level != levelTerakhir) {       // masih memantul: catat kapan berubah
    levelTerakhir = level;
    waktuBerubah = millis();
  }
  // Level baru dianggap sah bila sudah stabil selama JEDA_DEBOUNCE_MS.
  if (millis() - waktuBerubah >= JEDA_DEBOUNCE_MS && level != levelStabil) {
    levelStabil = level;
    return levelStabil == LOW;        // laporkan hanya peristiwa "baru ditekan"
  }
  return false;
}
