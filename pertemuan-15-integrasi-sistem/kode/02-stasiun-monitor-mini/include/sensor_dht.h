#pragma once
// sensor_dht.h — membaca DHT22 secara berkala tanpa delay()
#include <Arduino.h>

struct DataSensor {
  float suhu;           // °C
  float kelembapan;     // % RH
  bool valid;           // false bila pembacaan terakhir gagal
  int gagalBerturut;    // berapa kali gagal berturut-turut
};

void sensorMulai();
bool sensorPerbarui(DataSensor &data);   // true bila ada pembacaan baru (tiap 2,5 s)
