#pragma once
// ambang.h — ambang suhu: diatur potensiometer, disimpan di NVS (Preferences)
#include <Arduino.h>

void ambangMulai();                // muat ambang dari NVS (atau pakai nilai bawaan)
float ambangNilai();               // ambang yang berlaku sekarang (°C)
void ambangAturDariPot();          // baca potensiometer → ambang (hanya di halaman ATUR)
bool ambangSimpanBilaBerubah();    // tulis ke NVS HANYA bila berubah; true bila menulis
