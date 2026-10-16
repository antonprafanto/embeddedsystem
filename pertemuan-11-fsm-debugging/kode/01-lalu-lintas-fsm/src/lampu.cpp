// =====================================================================
//  lampu.cpp — "dapur" modul lampu: cara menyalakan LED lalu lintas
// =====================================================================
#include "lampu.h"
#include "konfigurasi.h"

void lampuMulai() {
  pinMode(PIN_MERAH, OUTPUT);
  pinMode(PIN_KUNING, OUTPUT);
  pinMode(PIN_HIJAU, OUTPUT);
  lampuTampilkan(Warna::PADAM);
}

void lampuTampilkan(Warna w) {
  // Ketiga pin selalu ditulis bersamaan agar tidak pernah ada dua lampu
  // yang menyala berbarengan (mis. merah dan hijau) karena lupa memadamkan.
  digitalWrite(PIN_MERAH,  w == Warna::MERAH  ? HIGH : LOW);
  digitalWrite(PIN_KUNING, w == Warna::KUNING ? HIGH : LOW);
  digitalWrite(PIN_HIJAU,  w == Warna::HIJAU  ? HIGH : LOW);
}
