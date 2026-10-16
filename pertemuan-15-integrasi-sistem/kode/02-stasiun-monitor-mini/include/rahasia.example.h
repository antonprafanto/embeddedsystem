// =====================================================================
//  CONTOH berkas rahasia (khusus board ESP32 asli) — sama seperti P13
// =====================================================================
//  1. Salin berkas ini di folder yang sama, beri nama  rahasia.h
//  2. Isi SSID dan sandi hotspot 2,4 GHz milikmu di rahasia.h.
//  3. Build dan Upload seperti biasa.
//  rahasia.h tercantum di .gitignore sehingga TIDAK ikut ter-commit ke
//  GitHub. Jangan menulis sandi asli di berkas contoh ini.
//  Di Wokwi berkas ini tidak diperlukan: program memakai Wokwi-GUEST.
// =====================================================================
#pragma once

#define WIFI_SSID  "NamaHotspotKamu"
#define WIFI_SANDI "SandiHotspotKamu"
#define WIFI_KANAL 0   // 0 = cari otomatis di semua kanal
