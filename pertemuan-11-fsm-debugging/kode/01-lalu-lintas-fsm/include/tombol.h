#pragma once
// =====================================================================
//  tombol.h — modul tombol penyeberang (sudah bebas pantulan)
// =====================================================================
#include <Arduino.h>

void tombolMulai();      // atur pin tombol sebagai INPUT_PULLUP
bool tombolDitekan();    // true SEKALI untuk setiap tekanan baru; tidak pernah menunggu
