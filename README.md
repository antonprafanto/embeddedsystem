<p align="center"><img src="aset/banner.png" alt="Sistem Tertanam (Embedded Systems) — belajar ESP32 dari nol" width="100%"></p>

# Sistem Tertanam (Embedded Systems)

Selamat datang di materi kuliah **Sistem Tertanam** untuk mahasiswa **S1 Teknik Elektro**. Di sini kamu akan belajar memprogram mikrokontroler **ESP32** dari nol: menyalakan LED, membaca tombol dan sensor, berkomunikasi dengan modul lain, menjalankan beberapa tugas sekaligus, sampai mengirim data ke internet. Semua materi ditulis untuk pemula, lengkap dengan gambar, kode siap pakai, simulasi, dan slide yang bisa diunduh.

| | |
| --- | --- |
| 👨‍🏫 **Dosen pengampu** | Anton Prafanto |
| 🔌 **Board** | ESP32 DevKit V1 (30 pin) — board 38 pin juga bisa |
| 🛠️ **Alat** | Visual Studio Code + PlatformIO (Arduino-ESP32 versi 3.3) |
| 🖥️ **Simulator** | [Wokwi](https://wokwi.com) — gratis, di peramban |
| 📅 **Jumlah pertemuan** | 16 (14 materi + UTS + UAS) |

---

## 🚀 Mulai dari Sini

Baru pertama kali? Ikuti urutan ini:

1. 📖 [**Mulai di Sini**](panduan/01-mulai-di-sini.md) — cara memakai repositori ini (5 menit).
2. 🛠️ [**Instalasi Alat**](panduan/02-instalasi-alat.md) — pasang VS Code, PlatformIO, Git, dan *driver* USB. **Kerjakan sebelum Pertemuan 1.**
3. 🖥️ [**Simulasi dengan Wokwi**](panduan/03-simulasi-wokwi.md) — praktik tanpa board.
4. 🧪 [**Pertemuan 1**](pertemuan-01-pengenalan/README.md) — program pertamamu.

Panduan pendukung: [Daftar Komponen](panduan/04-daftar-komponen.md) · [Lembar Saku Pinout ESP32](panduan/05-pinout-esp32.md) · [Mengatasi Masalah](panduan/06-mengatasi-masalah.md) · [Glosarium](panduan/07-glosarium.md) · [Rencana Pembelajaran Semester](RPS.md) · [Proyek Akhir](proyek-akhir/README.md)

---

## 🗺️ Peta 16 Pertemuan

| No | Topik | Modul | Slide |
| :---: | --- | :---: | :---: |
| 01 | Mengenal Sistem Tertanam, ESP32, dan Alat Kerja | [Buka](pertemuan-01-pengenalan/README.md) | [PDF](slide/pdf/P01-Pengenalan-Sistem-Tertanam.pdf) · [PPTX](slide/pptx/P01-Pengenalan-Sistem-Tertanam.pptx) |
| 02 | Dasar Pemrograman C/C++ untuk Mikrokontroler | [Buka](pertemuan-02-dasar-pemrograman/README.md) | [PDF](slide/pdf/P02-Dasar-Pemrograman-C.pdf) · [PPTX](slide/pptx/P02-Dasar-Pemrograman-C.pptx) |
| 03 | GPIO Output: Mengendalikan LED | [Buka](pertemuan-03-gpio-output/README.md) | [PDF](slide/pdf/P03-GPIO-Output.pdf) · [PPTX](slide/pptx/P03-GPIO-Output.pptx) |
| 04 | GPIO Input: Membaca Tombol | [Buka](pertemuan-04-gpio-input/README.md) | [PDF](slide/pdf/P04-GPIO-Input.pdf) · [PPTX](slide/pptx/P04-GPIO-Input.pptx) |
| 05 | Waktu dan Interupsi | [Buka](pertemuan-05-waktu-interupsi/README.md) | [PDF](slide/pdf/P05-Waktu-dan-Interupsi.pdf) · [PPTX](slide/pptx/P05-Waktu-dan-Interupsi.pptx) |
| 06 | Input Analog: ADC | [Buka](pertemuan-06-adc/README.md) | [PDF](slide/pdf/P06-Input-Analog-ADC.pdf) · [PPTX](slide/pptx/P06-Input-Analog-ADC.pptx) |
| 07 | PWM dan Mengendalikan Beban | [Buka](pertemuan-07-pwm-beban/README.md) | [PDF](slide/pdf/P07-PWM-dan-Beban.pdf) · [PPTX](slide/pptx/P07-PWM-dan-Beban.pptx) |
| 08 | **Ujian Tengah Semester (UTS)** | [Buka](pertemuan-08-uts/README.md) | [PDF](slide/pdf/P08-UTS.pdf) · [PPTX](slide/pptx/P08-UTS.pptx) |
| 09 | Komunikasi Serial: UART, I2C, dan SPI | [Buka](pertemuan-09-komunikasi-serial/README.md) | [PDF](slide/pdf/P09-Komunikasi-Serial.pdf) · [PPTX](slide/pptx/P09-Komunikasi-Serial.pptx) |
| 10 | Sensor, Aktuator, *Library*, dan *Datasheet* | [Buka](pertemuan-10-sensor-aktuator/README.md) | [PDF](slide/pdf/P10-Sensor-Aktuator-Datasheet.pdf) · [PPTX](slide/pptx/P10-Sensor-Aktuator-Datasheet.pptx) |
| 11 | Merancang *Firmware* Rapi: FSM dan *Debugging* | [Buka](pertemuan-11-fsm-debugging/README.md) | [PDF](slide/pdf/P11-FSM-dan-Debugging.pdf) · [PPTX](slide/pptx/P11-FSM-dan-Debugging.pptx) |
| 12 | *Multitasking* dengan FreeRTOS | [Buka](pertemuan-12-freertos/README.md) | [PDF](slide/pdf/P12-FreeRTOS.pdf) · [PPTX](slide/pptx/P12-FreeRTOS.pptx) |
| 13 | IoT: Wi-Fi dan MQTT | [Buka](pertemuan-13-wifi-mqtt/README.md) | [PDF](slide/pdf/P13-WiFi-dan-MQTT.pdf) · [PPTX](slide/pptx/P13-WiFi-dan-MQTT.pptx) |
| 14 | Hemat Daya dan Keandalan Sistem | [Buka](pertemuan-14-daya-keandalan/README.md) | [PDF](slide/pdf/P14-Hemat-Daya-Keandalan.pdf) · [PPTX](slide/pptx/P14-Hemat-Daya-Keandalan.pptx) |
| 15 | Integrasi Sistem dan Persiapan Proyek Akhir | [Buka](pertemuan-15-integrasi-sistem/README.md) | [PDF](slide/pdf/P15-Integrasi-Sistem.pdf) · [PPTX](slide/pptx/P15-Integrasi-Sistem.pptx) |
| 16 | **Ujian Akhir Semester (UAS)**: Demo dan Presentasi Proyek | [Buka](pertemuan-16-uas/README.md) | [PDF](slide/pdf/P16-UAS-Proyek-Akhir.pdf) · [PPTX](slide/pptx/P16-UAS-Proyek-Akhir.pptx) |

> [!TIP]
> **Cara mengunduh slide:** klik tautan **PDF**, lalu klik ikon **Download raw file** (panah ke bawah) di kanan atas pratinjau GitHub. Untuk mengunduh **semua materi sekaligus**, klik tombol hijau **Code** → **Download ZIP** di halaman utama repositori ini.

---

## 🧭 Alur Belajar Semester Ini

```mermaid
flowchart LR
    A["Fondasi<br/>P01–P02<br/>ESP32 & bahasa C"] --> B["Input/Output<br/>P03–P07<br/>GPIO, waktu, ADC, PWM"]
    B --> C{{"UTS<br/>P08"}}
    C --> D["Komunikasi<br/>P09–P10<br/>UART, I2C, SPI, sensor"]
    D --> E["Firmware andal<br/>P11–P12<br/>FSM & FreeRTOS"]
    E --> F["IoT & daya<br/>P13–P14<br/>Wi-Fi, MQTT, deep sleep"]
    F --> G["Proyek akhir<br/>P15–P16<br/>integrasi & demo"]
```

---

## 📂 Isi Repositori

```text
├── panduan/            ← instalasi, Wokwi, komponen, pinout, mengatasi masalah, glosarium
├── pertemuan-NN-.../   ← modul (README.md), gambar/, dan kode/ praktikum per pertemuan
├── proyek-akhir/       ← ketentuan, ide, dan rubrik proyek akhir
├── slide/pdf/          ← slide PDF untuk diunduh
├── slide/pptx/         ← slide PowerPoint
├── RPS.md              ← rencana pembelajaran semester
└── KREDIT-GAMBAR.md    ← sumber dan lisensi gambar dari pihak lain
```

Setiap *folder* di dalam `kode/` adalah satu proyek PlatformIO yang bisa langsung dibuka di VS Code, dan juga berisi `diagram.json` untuk simulator Wokwi.

---

## 🤝 Aturan Main

- **Pahami, bukan sekadar menyalin.** Saat praktikum kamu bisa diminta mengubah kode dan menjelaskan alasannya.
- **Catat galat dan solusinya.** Kebiasaan ini membuatmu cepat mahir.
- Menemukan salah ketik atau tautan rusak? Laporkan ke dosen/asisten atau buka *Issue* di repositori ini.

## 🖼️ Kredit

Sebagian gambar memakai karya pihak lain berlisensi terbuka (Espressif, Arduino, Wokwi, dan lainnya). Daftar lengkapnya ada di [KREDIT-GAMBAR.md](KREDIT-GAMBAR.md).
