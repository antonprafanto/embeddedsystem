# ⚡ EE-304: Sistem Tertanam (Embedded Systems)
### Program Studi Sarjana (S1) Teknik Elektro

Selamat datang di repositori resmi perkuliahan **Sistem Tertanam (Embedded Systems)** berbasis **SoC ESP32 (Xtensa Dual-Core 32-bit)**. Repositori ini berfungsi sebagai pusat distribusi materi, panduan praktikum (jobsheet), starter code, serta dokumentasi teknis perkuliahan.

---

## 📌 Navigasi Cepat
* 📖 **[Silabus & Rencana Pembelajaran Semester (RPS) Lengkap](SILABUS_EMBEDDED_SYSTEM_ESP32.md)**
* 🛠️ **Platform:** ESP32-WROOM-32D / ESP32-S3
* 💻 **Toolchain:** Visual Studio Code + PlatformIO (C/C++ & ESP-IDF Native)
* 🎯 **Pendekatan:** *Hardware-Software Co-Design* & *Scaffolding* (Ramah Pemula $\to$ Standar Industri)

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

## 📂 Rencana Struktur Direktori Repositori

```text
├── docs/                               # Materi kuliah, slide, & lembar saku pin
│   └── esp32_pin_gotchas.md            # Panduan pin aman & pin terlarang
├── labs/                               # Panduan praktikum terpandu mingguan
│   ├── week-01-bitwise-c/              # Starter code & jobsheet W01
│   ├── week-02-gpio-debugging/         # Starter code & jobsheet W02
│   └── ...
├── projects/                           # Template & referensi Capstone Project
├── SILABUS_EMBEDDED_SYSTEM_ESP32.md    # Dokumen resmi RPS kurikulum
└── README.md                           # Beranda portal perkuliahan
```

---

## ⚠️ Pedoman Penting Mahasiswa (Golden Rules)
1. **Aturan Pin Analog:** Sensor analog **HANYA boleh dihubungkan ke ADC1 (GPIO 32–39)**. Sirkuit ADC2 nonaktif saat Wi-Fi menyala.
2. **Pin Terlarang:** GPIO 6 sampai 11 terhubung internal ke chip Flash SPI. Jangan dihubungkan ke kabel apa pun.
3. **Hardware Sanity Check:** Selalu ukur rel tegangan 3.3V dengan multimeter digital sebelum menyalahkan kode program.

---

## 👨‍🏫 Dosen Pengampu & Pengelola
* **Dosen Pengampu:** Anton Prafanto
* **Repositori Resmi:** [github.com/antonprafanto/embeddedsystem](https://github.com/antonprafanto/embeddedsystem)
