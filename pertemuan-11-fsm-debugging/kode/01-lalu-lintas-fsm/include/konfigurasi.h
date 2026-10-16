#pragma once
// =====================================================================
//  konfigurasi.h — semua "angka penting" proyek dikumpulkan di SATU tempat
//  Mengapa? Bila pin atau durasi berubah, kamu cukup mengedit berkas ini
//  tanpa membongkar logika FSM di main.cpp.
// =====================================================================
#include <Arduino.h>

// ---- Pin (cocokkan dengan diagram.json) ----
constexpr uint8_t PIN_MERAH  = 25;
constexpr uint8_t PIN_KUNING = 26;
constexpr uint8_t PIN_HIJAU  = 27;
constexpr uint8_t PIN_TOMBOL = 4;    // tombol penyeberang ke GND, pakai INPUT_PULLUP

// ---- Lama tiap keadaan (milidetik) ----
// Sengaja singkat agar mudah diamati; lampu sungguhan jauh lebih lama.
constexpr unsigned long LAMA_HIJAU_MS     = 8000;  // hijau normal
constexpr unsigned long LAMA_HIJAU_MIN_MS = 3000;  // hijau minimal walau ada penyeberang
constexpr unsigned long LAMA_KUNING_MS    = 2000;
constexpr unsigned long LAMA_MERAH_MS     = 5000;  // waktu untuk menyeberang

// ---- Tombol ----
constexpr unsigned long JEDA_DEBOUNCE_MS  = 50;    // pantulan (bouncing) < 50 ms diabaikan
