#include "sensor_dht.h"
#include "konfigurasi.h"
#include <DHT.h>

static DHT dht(PIN_DHT, DHT22);
static unsigned long waktuBacaTerakhir = 0;
static int gagalBerturut = 0;

void sensorMulai() {
  dht.begin();
  waktuBacaTerakhir = millis();          // bacaan pertama 2,5 s setelah menyala
}

bool sensorPerbarui(DataSensor &data) {
  if (millis() - waktuBacaTerakhir < INTERVAL_BACA_SENSOR_MS) return false;   // belum waktunya
  waktuBacaTerakhir = millis();

  float kelembapan = dht.readHumidity();
  float suhu = dht.readTemperature();
  if (isnan(suhu) || isnan(kelembapan)) {   // NaN = komunikasi dengan sensor gagal
    gagalBerturut++;
    data.valid = false;                     // nilai lama dibiarkan, tetapi ditandai tidak sah
  } else {
    gagalBerturut = 0;
    data.valid = true;
    data.suhu = suhu;
    data.kelembapan = kelembapan;
  }
  data.gagalBerturut = gagalBerturut;
  return true;
}
