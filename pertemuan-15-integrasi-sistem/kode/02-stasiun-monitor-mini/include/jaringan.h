#pragma once
// jaringan.h — Wi-Fi + MQTT di tugas FreeRTOS terpisah (inti 0)
#include <Arduino.h>
#include "alarm.h"

enum class KeadaanJaringan : uint8_t { WIFI_MULAI, WIFI_MENUNGGU, MQTT_MENYAMBUNG, TERSAMBUNG };

struct PaketData {              // loop() → tugas jaringan
  float suhu;
  float kelembapan;
  float ambang;
  bool sensorValid;
  StatusAlarm status;
};

struct InfoJaringan {           // tugas jaringan → loop()
  KeadaanJaringan keadaan;
  uint32_t jumlahKirim;         // berapa kali paket data terkirim
  char ip[16];                  // mis. "10.10.0.2"
};

void jaringanMulai();                         // buat kotak surat + tugas jaringan
void jaringanKirim(const PaketData &paket);   // titipkan data TERBARU (tidak pernah menunggu)
InfoJaringan jaringanInfo();                  // intip status jaringan terbaru (tidak menunggu)
const char *jaringanNamaKeadaan(KeadaanJaringan k);
UBaseType_t jaringanSisaStack();              // sisa stack minimum tugas jaringan (byte)
