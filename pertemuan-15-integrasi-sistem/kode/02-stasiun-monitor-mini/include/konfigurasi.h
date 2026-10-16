#pragma once
// =====================================================================
//  konfigurasi.h — SEMUA pin dan angka penting di satu tempat.
//  Ingin mengganti pin, ambang, atau Wi-Fi? Cukup ubah berkas ini.
// =====================================================================
#include <Arduino.h>

// ---------- Identitas (WAJIB diganti) ----------
// NIM dipakai di topik MQTT dan ID klien agar datamu tidak bercampur
// dengan data teman sekelas di broker publik.
#define NIM "GANTI_NIM"     // ← GANTI dengan NIM-mu (angka/huruf saja)

// ---------- Peta pin (peta pin standar kursus) ----------
const int PIN_DHT        = 14;    // data DHT22
const int PIN_POT        = 34;    // potensiometer → ADC1 (aman saat Wi-Fi aktif)
const int PIN_TOMBOL     = 4;     // tombol ke GND, INPUT_PULLUP
const int PIN_LED_MERAH  = 25;    // status: melewati ambang
const int PIN_LED_KUNING = 26;    // status: mendekati ambang / sensor galat
const int PIN_LED_HIJAU  = 27;    // status: normal
const int PIN_LED_BIRU   = 23;    // status jaringan (MQTT)
const int PIN_LED_DETAK  = 2;     // LED bawaan: detak loop()
const int PIN_BUZZER     = 18;    // buzzer pasif (PWM)
const int PIN_SDA        = 21;    // bus I2C (OLED)
const int PIN_SCL        = 22;
const uint8_t ALAMAT_OLED = 0x3C;

// ---------- Waktu (milidetik) ----------
const unsigned long INTERVAL_BACA_SENSOR_MS = 2500;   // DHT22: minimal 2 s antarpembacaan
const unsigned long INTERVAL_BACA_POT_MS    = 100;
const unsigned long INTERVAL_LAYAR_MS       = 250;    // OLED digambar ulang 4× per detik
const unsigned long INTERVAL_KIRIM_MS       = 10000;  // data MQTT rutin tiap 10 s
const unsigned long JEDA_DEBOUNCE_MS        = 30;

// ---------- Ambang dan alarm (°C) ----------
const float AMBANG_BAWAAN    = 30.0;  // dipakai bila NVS masih kosong
const float AMBANG_MIN       = 20.0;  // rentang yang bisa diatur potensiometer
const float AMBANG_MAKS      = 45.0;
const float JARAK_WASPADA    = 2.0;   // MENDEKATI bila suhu ≥ ambang − 2,0
const float HISTERESIS       = 0.5;   // cegah status berganti-ganti di sekitar batas
const int   BATAS_GAGAL_BACA = 3;     // gagal 3× berturut-turut → SENSOR_GALAT
const int   FREKUENSI_BUZZER = 2000;  // Hz

// ---------- Jaringan (pola yang sama dengan Pertemuan 13) ----------
//  - Board asli: salin include/rahasia.example.h menjadi include/rahasia.h,
//    lalu isi SSID & sandi hotspot 2,4 GHz-mu (rahasia.h tidak di-commit).
//  - Wokwi: rahasia.h tidak ada, jadi otomatis memakai Wokwi-GUEST.
#if __has_include("rahasia.h")
#include "rahasia.h"
#define SUMBER_WIFI "include/rahasia.h"
#else
#define WIFI_SSID "Wokwi-GUEST"     // Wi-Fi virtual Wokwi: terbuka, tanpa sandi
#define WIFI_SANDI ""
#define WIFI_KANAL 6                // Wokwi-GUEST ada di kanal 6 (sambung lebih cepat)
#define SUMBER_WIFI "bawaan Wokwi"
#endif
#ifndef WIFI_KANAL
#define WIFI_KANAL 0                // 0 = cari jaringan di semua kanal
#endif
const char MQTT_BROKER[] = "broker.emqx.io";
const int  MQTT_PORT     = 1883;
#define TOPIK_DASAR "kuliah-st/" NIM "/stasiun/"

const unsigned long BATAS_TUNGGU_WIFI_MS = 15000;  // lewat dari ini → ulangi WiFi.begin()
const unsigned long JEDA_COBA_AWAL_MS    = 2000;   // jeda coba ulang MQTT: 4, 8, 16, 30 s ...
const unsigned long JEDA_COBA_MAKS_MS    = 30000;
