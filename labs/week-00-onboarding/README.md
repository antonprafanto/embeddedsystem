# 🛠️ MINGGU 0: ONBOARDING & DRIVER CLINIC
### Panduan Persiapan Mandiri Mahasiswa Sebelum Perkuliahan Dimulai

Selamat datang di laboratorium Sistem Tertanam! Sebelum menghadiri pertemuan tatap muka pertama, Anda **wajib** menyelesaikan modul persiapan mandiri ini di laptop masing-masing.

---

## 🎯 TUJUAN MODUL
1. Memastikan laptop Anda mengenali board ESP32 melalui komunikasi USB Serial.
2. Menginstal lingkungan pengembangan resmi: **Visual Studio Code + PlatformIO IDE**.
3. Berhasil meng-upload dan menjalankan program uji coba pertama (*Blink & Serial Hello World*).

---

## 🔌 LANGKAH 1: PERIKSA KABEL USB ANDA (JEBAKAN #1)

> [!WARNING]
> **50% masalah mahasiswa di hari pertama:** Board ESP32 tidak terdeteksi di laptop karena menggunakan **kabel charger murahan (hanya 2 kawat: VCC & GND)**, bukan **kabel data (4 kawat: VCC, GND, D+, D-)**.

### Cara Menguji Kabel USB:
1. Hubungkan ponsel Anda ke laptop menggunakan kabel tersebut.
2. Jika laptop berbunyi dan muncul jendela transfer file (MTP/File Transfer), berarti kabel tersebut adalah **kabel data**.
3. Jika ponsel hanya mengisi daya baterai dan tidak terdeteksi di laptop, **SEGERA GANTI KABEL**. Kabel tersebut tidak akan pernah bisa digunakan untuk memprogram ESP32!

---

## 💻 LANGKAH 2: IDENTIFIKASI CHIP USB-TO-UART & INSTAL DRIVER

Lihat chip kecil berwarna hitam berbentuk persegi di dekat port micro-USB/Type-C pada board ESP32 Anda:

```
[Port USB ESP32] ───> [ Chip USB-UART Converter ] ───> [ Modul ESP32 ]
```

### 1. Chip Silicon Labs CP2102 (Paling Banyak Digunakan)
* Bentuk: Kotak persegi kecil dengan tulisan `SILABS CP2102`.
* **Download Driver Resmi:** [Silicon Labs CP210x Drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
* Jalankan installer `CP210xVCPInstaller_x64.exe` (untuk Windows 64-bit).

### 2. Chip WCH CH340 / CH340G / CH341
* Bentuk: Persegi panjang dengan kaki-kaki di sisinya, bertuliskan `CH340C` atau `CH340G`.
* **Download Driver Resmi:** [WCH CH341SER Driver Windows](https://www.wch-ic.com/downloads/CH341SER_EXE.html)
* Jalankan `CH341SER.EXE` lalu klik tombol **INSTALL**.

### 3. Cara Memeriksa Keberhasilan Driver di Windows:
1. Hubungkan ESP32 ke laptop via kabel data.
2. Buka **Device Manager** (tekan tombol `Windows + X` lalu pilih *Device Manager*).
3. Buka bagian **Ports (COM & LPT)**.
4. Anda harus melihat salah satu dari perangkat berikut:
   * `Silicon Labs CP210x USB to UART Bridge (COM3)` atau
   * `USB-SERIAL CH340 (COM4)`
5. Catat nomor port COM Anda (misalnya `COM3` atau `COM4`).

---

## ⚙️ LANGKAH 3: INSTALASI VISUAL STUDIO CODE & PLATFORMIO

Kita menggunakan **PlatformIO IDE** karena merupakan standar industri modern yang jauh lebih andal dan terstruktur dibandingkan Arduino IDE lama.

1. Download dan instal editor [Visual Studio Code](https://code.visualstudio.com/).
2. Buka VS Code, klik ikon **Extensions** di bilah sisi kiri (atau tekan `Ctrl + Shift + X`).
3. Cari ekstensi: **PlatformIO IDE** (ikon semut/alien oranye).
4. Klik **Install** dan tunggu hingga proses instalasi komponen internal selesai (memerlukan waktu 3–5 menit tergantung kecepatan internet).
5. Setelah selesai, restart VS Code. Akan muncul ikon semut PlatformIO di bilah sisi kiri.

---

## 🚀 LANGKAH 4: PROYEK UJI COBA PERTAMA (SMOKE TEST)

Untuk memastikan seluruh rantai alat (*toolchain*) bekerja sempurna:

1. Buka PlatformIO Home (klik ikon alien di sisi kiri $\to$ pilih **PIO Home** $\to$ **Open**).
2. Klik tombol **+ New Project**.
3. Beri nama: `esp32_smoke_test`.
4. Pilih Board: **Espressif ESP32 Dev Module**.
5. Pilih Framework: **Arduino** (untuk uji coba awal).
6. Klik **Finish** dan tunggu PlatformIO mengunduh paket core ESP32 pertama kali.
7. Buka file `src/main.cpp` di explorer proyek, lalu gantikan kodenya dengan:

```cpp
#include <Arduino.h>

#define LED_PIN 2 // LED biru internal pada ESP32

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("======================================");
  Serial.println("🔥 ESP32 SMOKE TEST SUKSES TERVERIFIKASI!");
  Serial.println("======================================");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("[Status] LED ON  - Sistem Aktif");
  delay(1000);
  
  digitalWrite(LED_PIN, LOW);
  Serial.println("[Status] LED OFF - Sistem Aktif");
  delay(1000);
}
```

8. Hubungkan board ESP32 ke laptop.
9. Di bilah bawah biru VS Code:
   * Klik ikon centang `✓` untuk **Build / Compile**.
   * Klik ikon panah kanan `→` untuk **Upload** ke ESP32.
   * Klik ikon colokan listrik untuk membuka **Serial Monitor** (pastikan baud rate diatur ke `115200`).

---

## ❓ PANDUAN TROUBLESHOOTING (KENDALA UMUM)

### Kasus A: Muncul pesan error *"A fatal error occurred: Failed to connect to ESP32: Timed out waiting for packet header"*
* **Penyebab:** Rangkaian auto-download pada board clone ESP32 murah memiliki kapasitor timing yang lambat.
* **Solusi Praktis:**
  1. Saat terminal menampilkan tulisan `Connecting........_____.....`, **tekan dan tahan tombol BOOT** pada board ESP32 selama 2 detik, lalu lepaskan.
  2. Proses upload akan langsung berjalan.

### Kasus B: Pengguna Linux / Ubuntu (Permission Denied di `/dev/ttyUSB0`)
* **Penyebab:** Pengguna biasa belum memiliki izin mengakses port serial modem.
* **Solusi:** Buka terminal dan masukkan user Anda ke grup `dialout`:
  ```bash
  sudo usermod -a -G dialout $USER
  ```
  Lalu restart laptop Anda.

### Kasus C: Port COM tidak terdeteksi sama sekali di Device Manager
* **Solusi:**
  1. Ganti kabel USB (utamakan kabel bawaan smartphone original).
  2. Pindahkan ke port USB lain di laptop (hindari menggunakan USB Hub pasif tanpa adaptor daya tambahan).
  3. Periksa apakah lampu LED indikator merah (Power LED) pada board menyala saat dicolok.
