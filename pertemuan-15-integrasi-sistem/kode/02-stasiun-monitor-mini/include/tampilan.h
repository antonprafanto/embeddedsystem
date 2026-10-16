#pragma once
// tampilan.h — menggambar tiga halaman OLED
#include <Arduino.h>
#include "alarm.h"
#include "jaringan.h"

enum class Halaman : uint8_t { UTAMA, JARINGAN, ATUR_AMBANG };
const int JUMLAH_HALAMAN = 3;

struct IsiLayar {               // semua yang perlu digambar, dikumpulkan oleh main.cpp
  Halaman halaman;
  float suhu;
  float kelembapan;
  float ambang;
  bool sensorValid;
  StatusAlarm status;
  InfoJaringan jaringan;
};

bool tampilanMulai();           // false bila OLED tidak menjawab di alamatnya
void tampilanGambar(const IsiLayar &isi);
