# ⚡ Sistem Tertanam (Embedded Systems)
### Program Studi Sarjana (S1) Teknik Elektro

Selamat datang di repositori resmi perkuliahan **Sistem Tertanam (Embedded Systems)** berbasis **SoC ESP32 (Xtensa Dual-Core 32-bit)**. Repositori ini berfungsi sebagai portal distribusi materi, panduan praktikum (jobsheet), starter code, serta dokumentasi teknis perkuliahan.

---

## 📌 Navigasi Cepat
* 📖 **[Silabus & Rencana Pembelajaran Semester (RPS) Lengkap](SILABUS_EMBEDDED_SYSTEM_ESP32.md)**
* 📌 **[Lembar Saku Pinout & Hardware Gotchas ESP32](docs/esp32_pin_gotchas.md)**
* 🛠️ **[Minggu 0: Panduan Onboarding & Driver Clinic](labs/week-00-onboarding/README.md)**
* 📝 **[Minggu 1: Fondasi Bahasa C & Manipulasi Bitwise](labs/week-01-bitwise-c/README.md)**
* 💻 **Platform:** ESP32-WROOM-32D / ESP32-S3
* ⚙️ **Toolchain:** Visual Studio Code + PlatformIO (Hybrid: Arduino Core → Native FreeRTOS/ESP-IDF)

---

## 🗺️ Peta Jalur Pembelajaran (16 Minggu)

```mermaid
flowchart TD
    subgraph Fase1 ["Fase 1: Fondasi, Elektrikal & Diagnostik (W1 - W3)"]
        W1[W01: Onboarding, C Refresher & Bitwise] --> W2[W02: GPIO, Interupsi & Crash Debugging]
        W2 --> W3[W03: Transistor Driver & ADC1 Precision]
    end

    subgraph Fase2 ["Fase 2: Memori & Komunikasi Instrumen (W4 - W6)"]
        W3 --> W4[W04: NVS Storage & Baca Datasheet]
        W4 --> W5[W05: Bus Serial UART/I2C/SPI & Logic Analyzer]
        W5 --> W6[W06: Bus Industri CAN/TWAI & RS-485 Modbus]
    end

    subgraph Fase3 ["Fase 3: FreeRTOS & Sistem Waktu Nyata (W7 - W10)"]
        W6 --> W7[W07: FreeRTOS Task & Task Watchdog Timer]
        W7 --> W8[W08: Evaluasi Tengah Semester - UTS]
        W8 --> W9[W09: Sinkronisasi IPC - Queue, Mutex, Semaphore]
        W9 --> W10[W10: Dual-Core Task Pinning & Deferred ISR]
    end

    subgraph Fase4 ["Fase 4: Daya Rendah, IoT & Keamanan (W11 - W13)"]
        W10 --> W11[W11: Low-Power Design & Deep Sleep]
        W11 --> W12[W12: Wi-Fi Stack & MQTT Auto-Reconnect]
        W12 --> W13[W13: TLS Security & OTA Firmware Rollback]
    end

    subgraph Fase5 ["Fase 5: Komputasi Tepi & Capstone (W14 - W16)"]
        W13 --> W14[W14: BLE & Pengantar TinyML]
        W14 --> W15[W15: Skematik PCB, Power Spikes & EMC/EMI]
        W15 --> W16[W16: Evaluasi Akhir Semester - UAS Capstone Demo]
    end
```

---

## 📂 Struktur Direktori Repositori

```text
├── docs/                                   # Dokumentasi pendukung & lembar saku
│   └── esp32_pin_gotchas.md                # Panduan pin aman & pin terlarang ESP32
├── labs/                                   # Panduan praktikum terpandu mingguan
│   ├── week-00-onboarding/                 # Panduan instalasi toolchain & driver USB
│   ├── week-01-bitwise-c/                  # Jobsheet & Starter code praktikum W01
│   │   ├── platformio.ini                  # Konfigurasi project PlatformIO
│   │   ├── README.md                       # Jobsheet modul lab
│   │   └── src/main.cpp                    # Template kode berjenjang (Level 1-3)
│   └── ...
├── projects/                               # Template & spesifikasi Capstone Project
├── SILABUS_EMBEDDED_SYSTEM_ESP32.md        # Dokumen resmi RPS kurikulum (OBE)
└── README.md                               # Beranda portal perkuliahan
```

---

## ⚠️ Pedoman Penting Mahasiswa (Golden Rules)
1. **Aturan Pin Analog:** Sensor analog **HANYA boleh dihubungkan ke ADC1 (GPIO 32–39)**. Sirkuit ADC2 nonaktif saat radio Wi-Fi menyala.
2. **Pin Terlarang:** GPIO 6 sampai 11 terhubung internal ke chip Flash SPI. Jangan pernah dihubungkan ke kabel apa pun!
3. **Hardware Sanity Check:** Selalu ukur rel tegangan 3.3V dengan multimeter digital sebelum menyalahkan kode program Anda.

---

## 👨‍🏫 Dosen Pengampu & Pengelola
* **Dosen Pengampu:** Anton Prafanto
* **Repositori Resmi:** [github.com/antonprafanto/embeddedsystem](https://github.com/antonprafanto/embeddedsystem)
