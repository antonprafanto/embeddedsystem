#pragma once
// alarm.h — FSM status suhu + keluaran LED dan buzzer
#include <Arduino.h>

enum class StatusAlarm : uint8_t { MENUNGGU, NORMAL, MENDEKATI, BAHAYA, SENSOR_GALAT };

void alarmMulai();
// Hitung status dari data terbaru; true bila status BERUBAH
bool alarmPerbarui(bool sensorValid, int gagalBerturut, float suhu, float ambang);
StatusAlarm alarmStatus();
const char *alarmNamaStatus(StatusAlarm s);
void alarmJalankanKeluaran();      // panggil di SETIAP putaran loop(): LED + pola bunyi
