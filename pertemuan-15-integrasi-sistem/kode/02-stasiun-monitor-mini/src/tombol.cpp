#include "tombol.h"
#include "konfigurasi.h"

// "static" = variabel milik modul ini saja, tidak terlihat dari berkas lain
static bool keadaanStabil = HIGH;        // HIGH = dilepas (INPUT_PULLUP)
static bool bacaanTerakhir = HIGH;
static unsigned long waktuBerubah = 0;

void tombolMulai() {
  pinMode(PIN_TOMBOL, INPUT_PULLUP);
}

bool tombolDitekan() {
  bool bacaan = digitalRead(PIN_TOMBOL);
  if (bacaan != bacaanTerakhir) {        // ada perubahan (bisa jadi pantulan)
    bacaanTerakhir = bacaan;
    waktuBerubah = millis();
  }
  if (millis() - waktuBerubah >= JEDA_DEBOUNCE_MS && bacaan != keadaanStabil) {
    keadaanStabil = bacaan;              // sudah stabil cukup lama → sah
    return keadaanStabil == LOW;         // laporkan hanya saat mulai ditekan
  }
  return false;
}
