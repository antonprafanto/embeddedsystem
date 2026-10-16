# Panduan Mengatasi Masalah (Troubleshooting)

> Galat (*error*) itu wajar, bahkan bagi insinyur berpengalaman. Kuncinya: **baca pesannya pelan-pelan**, cari kata kuncinya di tabel ini, lalu ubah **satu hal saja** setiap kali mencoba.

**Langkah umum sebelum panik**

1. Baca baris galat **pertama** di terminal (galat berikutnya sering hanya akibat dari galat pertama).
2. Tanyakan: masalahnya di **kode**, **alat (PlatformIO/driver)**, atau **rangkaian**?
3. Kembali ke kondisi terakhir yang berhasil, lalu ubah sedikit demi sedikit.

---

## 1. Saat Instalasi dan Membuka Proyek

| Gejala | Penyebab umum | Solusi |
| --- | --- | --- |
| Tombol PlatformIO (✓, →, colokan) tidak muncul di bilah bawah | *Folder* yang dibuka bukan *folder* proyek | Buka *folder* yang **langsung** berisi `platformio.ini` (misalnya `kode/01-halo-esp32`), bukan *folder* induknya. |
| Pesan *PlatformIO: Initializing…* lama sekali | Unduhan pertama sedang berjalan | Tunggu (bisa 10–30 menit). Pastikan internet stabil. |
| `HTTPClientError` atau `Could not install package` | Internet terputus atau diblokir *proxy* | Coba jaringan lain (misalnya *hotspot* HP), lalu klik **Build** lagi. |
| Python tidak ditemukan | Python portabel gagal dipasang | Pasang Python 3 dari python.org (centang **Add to PATH**), buka ulang VS Code. |

## 2. Saat Build (Kompilasi)

| Pesan galat (contoh) | Arti | Solusi |
| --- | --- | --- |
| `error: expected ';' before '}' token` | Lupa titik koma di akhir baris | Tambahkan `;` pada baris **sebelum** baris yang ditunjuk. |
| `error: 'nilaiSensor' was not declared in this scope` | Nama variabel/fungsi salah ketik atau belum dideklarasikan | Periksa ejaan dan huruf besar-kecil. C/C++ membedakan `nilai` dan `Nilai`. |
| `error: 'bacaSensor' was not declared in this scope` (padahal fungsinya ada) | Fungsi dipanggil sebelum didefinisikan | Pindahkan definisi fungsi ke atas, atau tulis deklarasinya (*prototype*) di bagian atas berkas. |
| `fatal error: Adafruit_SSD1306.h: No such file or directory` | *Library* belum terdaftar | Pastikan `lib_deps` di `platformio.ini` berisi *library* tersebut, lalu **Build** ulang. |
| `error: 'ledcSetup' was not declared in this scope` | Kode dari tutorial lama (Arduino-ESP32 versi 2) | Pakai API versi 3: `ledcAttach(pin, frekuensi, resolusi)` dan `ledcWrite(pin, duty)`. |

## 3. Saat Upload

| Pesan galat (contoh) | Arti | Solusi |
| --- | --- | --- |
| `A fatal error occurred: Failed to connect to ESP32: No serial data received.` | ESP32 tidak masuk mode unduh | Tahan tombol **BOOT** saat terminal menampilkan `Connecting....`, lepas setelah muncul `Writing at ...`. |
| `Could not open COM5, the port is busy or doesn't exist.` | *Port* sedang dipakai aplikasi lain | Tutup Serial Monitor (klik ikon tempat sampah di terminal), aplikasi serial lain, atau tab Wokwi for VS Code. Coba lagi. |
| Tidak ada *port* yang terdeteksi | Kabel charger, *driver* belum terpasang, atau port USB laptop bermasalah | Ganti **kabel data**, pasang *driver* CP210x/CH340, coba port USB lain. Lihat [Panduan Instalasi](02-instalasi-alat.md#8-langkah-6--sambungkan-board-dan-pasang-driver-usb). |
| Upload berhasil, tetapi program lama yang jalan | Board tidak di-*reset* | Tekan tombol **EN** sekali. |

## 4. Saat Program Berjalan dan di Serial Monitor

| Gejala | Penyebab umum | Solusi |
| --- | --- | --- |
| Tulisan acak seperti `⸮⸮⸮⸮` | Kecepatan (*baud rate*) tidak sama | Samakan `Serial.begin(115200)` di kode dengan `monitor_speed = 115200` di `platformio.ini`. |
| Serial Monitor kosong | Pesan sudah terkirim sebelum monitor dibuka | Tekan tombol **EN** setelah Serial Monitor terbuka. |
| `Brownout detector was triggered`, lalu board terus mengulang | Tegangan catu turun (kabel jelek, port USB lemah, servo/motor menarik arus besar) | Ganti kabel/port USB; beri catu terpisah untuk servo/motor dengan GND bersama. |
| `Guru Meditation Error: Core 1 panic'ed (...)` | Program *crash* (misalnya mengakses memori yang salah, membagi dengan nol) | Baca baris `Backtrace`. Dengan `monitor_filters = esp32_exception_decoder`, PlatformIO menunjukkan nama fungsi dan nomor barisnya. Dibahas di Pertemuan 11. |
| Board restart terus dengan pesan `task_wdt` | Ada perulangan yang tidak pernah memberi jeda | Tambahkan `delay()` kecil atau perbaiki logika perulangan. Dibahas di Pertemuan 12 dan 14. |

## 5. Masalah Rangkaian (Perangkat Keras)

| Gejala | Cek ini |
| --- | --- |
| LED tidak menyala | Kaki panjang (anoda) ke arah pin GPIO, kaki pendek ke GND? Resistor terpasang? Nomor pin di kode sama dengan pin yang dipakai? |
| Tombol terbaca acak (kadang 0, kadang 1) | Pin mengambang (*floating*): pakai `INPUT_PULLUP` atau resistor *pull-up/pull-down*. |
| Sensor tidak terbaca | Tegangan sensor (3,3 V atau 5 V)? GND sudah tersambung bersama? Kabel SDA/SCL atau TX/RX tertukar? |
| Komponen tertancap tapi seolah tidak tersambung | Posisi di *breadboard* salah baris, atau *breadboard* longgar. Uji sambungan dengan multimeter mode *continuity*. |
| Board atau komponen terasa **panas** | **Cabut USB segera!** Kemungkinan hubung singkat (3V3/5V langsung ke GND) atau komponen terbalik. |

## 6. Masalah di Wokwi

| Gejala | Solusi |
| --- | --- |
| `Build failed` dengan galat *library* | Tambahkan *library* lewat **Library Manager** sesuai berkas `libraries.txt` praktikum. |
| Rangkaian tidak berubah setelah menempel `diagram.json` | Pastikan seluruh isi lama dihapus dan isi baru utuh (diawali `{` dan diakhiri `}`). |
| Simulasi lambat | Normal untuk program berat. Tutup tab lain di peramban. |
| Tidak bisa menyimpan proyek | Masuk (*login*) dengan akun Wokwi gratis. |

---

## Masih Buntu?

1. Catat **pesan galat lengkap**, **kode**, dan **foto rangkaian**.
2. Tanyakan ke asisten/dosen atau di forum kelas dengan informasi tersebut. Pertanyaan yang jelas akan dijawab jauh lebih cepat.

---

## Referensi

1. Espressif Systems. (2026). *esptool — Troubleshooting*. <https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html>
2. Espressif Systems. (2026). *ESP-IDF Programming Guide — Fatal Errors*. <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/fatal-errors.html>
3. PlatformIO Labs. (2026). *Frequently Asked Questions*. <https://docs.platformio.org/en/latest/faq/index.html>

---

[⬅️ Pinout ESP32](05-pinout-esp32.md) · [🏠 Beranda](../README.md) · [Glosarium ➡️](07-glosarium.md)
