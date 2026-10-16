#pragma once
// =====================================================================
//  lampu.h — "menu" modul lampu: APA yang bisa dilakukan, bukan BAGAIMANA
// =====================================================================
#include <Arduino.h>

// Warna yang boleh ditampilkan. enum class membuat namanya "terkurung":
// Warna::HIJAU tidak akan tertukar dengan Keadaan::HIJAU di main.cpp.
enum class Warna { MERAH, KUNING, HIJAU, PADAM };

void lampuMulai();               // atur ketiga pin lampu sebagai OUTPUT, semua padam
void lampuTampilkan(Warna w);    // nyalakan SATU warna, padamkan yang lain
