# Panduan Simulasi dengan Wokwi

> Wokwi adalah simulator elektronika di peramban (*browser*). Kamu bisa "merangkai" ESP32, LED, tombol, sensor, bahkan Wi-Fi, lalu menjalankan kode yang sama dengan board asli — **tanpa membeli komponen dan tanpa instalasi**.

**Daftar isi**

1. [Kapan Memakai Wokwi?](#1-kapan-memakai-wokwi)
2. [Jalur Utama: Wokwi di Peramban (Gratis)](#2-jalur-utama-wokwi-di-peramban-gratis)
3. [Berinteraksi dengan Rangkaian Simulasi](#3-berinteraksi-dengan-rangkaian-simulasi)
4. [Memakai Library di Wokwi](#4-memakai-library-di-wokwi)
5. [Menyimpan dan Membagikan Proyek](#5-menyimpan-dan-membagikan-proyek)
6. [Jalur Opsional: Wokwi for VS Code](#6-jalur-opsional-wokwi-for-vs-code)
7. [Keterbatasan Simulator](#7-keterbatasan-simulator)

---

## 1. Kapan Memakai Wokwi?

- Kamu **belum punya** board atau komponen tertentu.
- Kamu ingin **mencoba ide dulu** sebelum merangkai di *breadboard* (lebih aman, tidak ada komponen terbakar).
- Kamu belajar di rumah dan kitnya ada di laboratorium.

Setiap *folder* praktikum di repositori ini sudah berisi dua berkas yang dibutuhkan Wokwi:

| Berkas di repositori | Dipakai di Wokwi sebagai | Isinya |
| --- | --- | --- |
| `src/main.cpp` | tab **sketch.ino** | Kode program |
| `diagram.json` | tab **diagram.json** | Daftar komponen dan kabelnya |
| `libraries.txt` (jika ada) | tab **Library Manager** / berkas `libraries.txt` | Daftar *library* tambahan |

## 2. Jalur Utama: Wokwi di Peramban (Gratis)

![Langkah menjalankan kode ESP32 di simulator Wokwi pada peramban](gambar/wokwi-langkah-simulasi.png)

*Gambar W.1 — Enam langkah menjalankan praktikum di Wokwi. Sumber: ilustrasi dibuat untuk materi ini.*

1. Buka <https://wokwi.com/projects/new/esp32> memakai Chrome, Edge, atau Firefox. Halaman ini berisi proyek ESP32 kosong.
2. Klik tab **sketch.ino**. Hapus semua isinya (`Ctrl+A` lalu `Delete`), kemudian **tempel** seluruh isi berkas `src/main.cpp` dari *folder* praktikum.
3. Klik tab **diagram.json**. Hapus semua isinya, lalu **tempel** seluruh isi berkas `diagram.json` dari *folder* praktikum yang sama. Rangkaian di panel kanan akan langsung berubah.
4. Jika *folder* praktikum punya berkas `libraries.txt`, tambahkan *library* yang tertulis di dalamnya (lihat [bagian 4](#4-memakai-library-di-wokwi)).
5. Klik tombol hijau **▶** (*Start the simulation*). Wokwi akan mengompilasi kode di servernya; tunggu beberapa detik.
6. Pesan dari `Serial.println()` muncul di **Serial Monitor** pada panel bawah.

> [!TIP]
> **Cara cepat menyalin isi berkas dari GitHub:** buka berkasnya di GitHub (misalnya `src/main.cpp`), lalu klik ikon **Copy raw file** (dua kotak bertumpuk) di kanan atas tampilan kode.

> [!NOTE]
> **Kenapa kode dari PlatformIO bisa langsung dipakai di Wokwi?** Semua kode di materi ini sengaja ditulis dengan `#include <Arduino.h>` dan fungsi yang dideklarasikan sebelum dipakai. Dengan begitu, kode sah sebagai berkas `.cpp` di PlatformIO **dan** sebagai berkas `.ino` di Wokwi.

## 3. Berinteraksi dengan Rangkaian Simulasi

Selama simulasi berjalan, kamu bisa:

- **Menekan tombol (*push button*)**: klik dan tahan tombol dengan *mouse*.
- **Memutar potensiometer**: klik potensiometer, lalu geser *slider* yang muncul.
- **Mengubah nilai sensor** (suhu, kelembapan, cahaya, jarak): klik sensornya, lalu atur *slider* nilainya.
- **Mengirim teks ke ESP32**: ketik di kotak input Serial Monitor, lalu tekan Enter.
- **Menghentikan simulasi**: klik tombol ■ (*Stop*).

## 4. Memakai Library di Wokwi

Sebagian praktikum memakai *library* (pustaka kode siap pakai), misalnya untuk layar OLED atau sensor DHT22. Di PlatformIO, *library* ditulis di `platformio.ini` bagian `lib_deps` dan diunduh otomatis. Di Wokwi caranya:

1. Klik tab **Library Manager** di atas editor kode.
2. Klik tombol **+**, ketik nama *library* (misalnya `Adafruit SSD1306`), lalu klik namanya untuk menambahkan.
3. Ulangi untuk setiap baris di berkas `libraries.txt` praktikum.

Wokwi lalu membuat berkas `libraries.txt` di proyekmu secara otomatis.

## 5. Menyimpan dan Membagikan Proyek

- Tanpa akun, kamu tetap bisa menjalankan simulasi, tetapi **tidak bisa menyimpan**.
- Dengan akun gratis (masuk lewat Google/GitHub), klik **Save** untuk menyimpan. Proyek gratis bersifat **publik**: siapa pun yang punya tautannya bisa melihat.
- Untuk mengumpulkan tugas, kamu bisa menyalin tautan proyek Wokwi-mu (tombol **Share**) bila dosen memintanya.

> [!WARNING]
> **Jangan menyimpan kata sandi, token, atau data pribadi** di proyek Wokwi publik.

## 6. Jalur Opsional: Wokwi for VS Code

Wokwi juga tersedia sebagai ekstensi VS Code yang menjalankan simulasi langsung dari hasil *build* PlatformIO. Ekstensi ini **produk komersial** yang memerlukan lisensi setelah masa uji coba, jadi **tidak wajib**. Jika kampusmu menyediakan lisensi:

1. Pasang ekstensi **Wokwi Simulator** dari tab **Extensions** VS Code.
2. Tekan `F1`, pilih **Wokwi: Request a New License**, lalu ikuti petunjuk di peramban.
3. Buka *folder* praktikum (sudah berisi `wokwi.toml` dan `diagram.json`), klik **Build** di PlatformIO.
4. Tekan `F1` → **Wokwi: Start Simulator**.

## 7. Keterbatasan Simulator

Simulator sangat membantu, tetapi **bukan pengganti penuh perangkat asli**:

- Bluetooth belum disimulasikan; Wi-Fi tersedia lewat jaringan virtual **Wokwi-GUEST** (tanpa kata sandi).
- Simulasi bisa berjalan lebih lambat daripada waktu nyata pada laptop yang berat. Waktu di dalam simulasi (misalnya `millis()`) tetap konsisten.
- Masalah fisik seperti kabel kendur, *breadboard* rusak, catu daya drop, atau derau listrik **tidak muncul** di simulator. Pengalaman merangkai di dunia nyata tetap penting.

---

## Referensi

1. CodeMagic LTD. (2026). *Wokwi Docs — ESP32 Simulation*. <https://docs.wokwi.com/guides/esp32>
2. CodeMagic LTD. (2026). *Wokwi Docs — Arduino Libraries*. <https://docs.wokwi.com/guides/libraries>
3. CodeMagic LTD. (2026). *Wokwi Docs — ESP32 WiFi Networking*. <https://docs.wokwi.com/guides/esp32-wifi>
4. CodeMagic LTD. (2026). *Wokwi for VS Code — Getting Started*. <https://docs.wokwi.com/vscode/getting-started>

---

[⬅️ Instalasi Alat](02-instalasi-alat.md) · [🏠 Beranda](../README.md) · [Daftar Komponen ➡️](04-daftar-komponen.md)
