#pragma once
// tombol.h — membaca tombol dengan debounce, tanpa delay()
#include <Arduino.h>

void tombolMulai();      // atur pin sebagai INPUT_PULLUP
bool tombolDitekan();    // true SEKALI setiap kali tombol mulai ditekan
