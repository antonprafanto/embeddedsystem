# Proposal Proyek Akhir — [Judul Proyek]

> **Cara memakai templat ini:** salin berkas ini ke repositori kelompokmu (misalnya `dokumen/proposal.md`), ganti semua teks dalam tanda kurung siku `[…]`, lalu hapus kotak petunjuk seperti ini. Kumpulkan dalam bentuk PDF pada **Pertemuan 14**. Panjang ideal 4–8 halaman. Ketentuan lengkap ada di halaman [Proyek Akhir](README.md).

| | |
| --- | --- |
| **Mata kuliah** | Sistem Tertanam |
| **Kelompok** | [NN] |
| **Judul proyek** | [Judul singkat dan jelas, misalnya "Alarm Keamanan Ruangan Berbasis ESP32"] |
| **Tautan repositori** | [https://github.com/…] |
| **Tanggal** | [tanggal bulan tahun] |

## Anggota dan Peran

| No | Nama | NIM | Peran utama |
| :---: | --- | --- | --- |
| 1 | [Nama lengkap] | [NIM] | [Perangkat keras] |
| 2 | [Nama lengkap] | [NIM] | [*Firmware*] |
| 3 | [Nama lengkap] | [NIM] | [Integrasi dan dokumentasi] |

## 1. Latar Belakang dan Masalah

> **Petunjuk:** 1–2 paragraf. Siapa penggunanya, masalah apa yang mereka hadapi, dan mengapa sistem tertanam cocok untuk menyelesaikannya.

[Tulis di sini.]

## 2. Tujuan dan Ruang Lingkup

**Tujuan:** [Satu kalimat, misalnya "Membangun alarm ruangan yang mendeteksi gerakan dan mengirim notifikasi ke HP dalam waktu kurang dari 3 detik."]

**Termasuk dalam ruang lingkup:**

- [Fitur 1]
- [Fitur 2]

**Di luar ruang lingkup** (sengaja tidak dikerjakan):

- [Misalnya aplikasi Android khusus; cukup memakai MQTTX]

## 3. Kebutuhan Sistem

> **Petunjuk:** setiap kebutuhan harus bisa diuji. Hindari kata "cepat" atau "akurat" tanpa angka. Kebutuhan fungsional menjawab "sistem melakukan apa"; kebutuhan nonfungsional menjawab "seberapa baik".

### 3.1 Kebutuhan Fungsional

| ID | Kebutuhan | Cara menguji |
| --- | --- | --- |
| F-1 | [Contoh: Saat mode SIAGA, sistem membunyikan alarm paling lambat 1 detik setelah gerakan terdeteksi.] | [Picu sensor 10 kali, lalu catat waktu respons dari log `millis()`.] |
| F-2 | [ ] | [ ] |
| F-3 | [ ] | [ ] |

### 3.2 Kebutuhan Nonfungsional

| ID | Kebutuhan | Cara menguji |
| --- | --- | --- |
| NF-1 | [Contoh: Sistem berjalan 2 jam tanpa *reset* atau macet.] | [Uji ketahanan 2 jam sambil mencatat log Serial.] |
| NF-2 | [Contoh: Arus total maksimal 500 mA sehingga cukup dengan adaptor 5 V 1 A.] | [Ukur dengan multimeter saat semua aktuator aktif.] |

## 4. Diagram Blok

> **Petunjuk:** tunjukkan sensor, aktuator, tampilan atau komunikasi, catu daya, serta jenis antarmuka (digital, ADC, interupsi, PWM, I2C, Wi-Fi) dan nomor pinnya. Lihat contohnya di bagian "Contoh Diagram Blok" halaman [Proyek Akhir](README.md#-contoh-diagram-blok).

[Sisipkan gambar diagram blok di sini, misalnya `![Diagram blok](../gambar/diagram-blok.png)`.]

[Jelaskan alur sistem dalam 3–5 kalimat.]

## 5. Daftar Komponen

| No | Komponen | Jumlah | Sumber (kit/beli) | Perkiraan harga | Ada di Wokwi? |
| :---: | --- | :---: | --- | --- | :---: |
| 1 | ESP32 DevKit V1 | 1 | Kit | – | Ya |
| 2 | [ ] | [ ] | [ ] | [ ] | [ ] |
| 3 | [ ] | [ ] | [ ] | [ ] | [ ] |

## 6. Rencana Alokasi Pin

> **Petunjuk:** hindari GPIO 6–11; pasang sensor analog di ADC1 (GPIO 32–39) bila memakai Wi-Fi; hati-hati dengan pin *strapping* 0, 2, 5, 12, dan 15. Lihat [lembar saku pinout](../panduan/05-pinout-esp32.md).

| Komponen | Pin komponen | GPIO ESP32 | Jenis sinyal | Catatan |
| --- | --- | :---: | --- | --- |
| [Contoh: DHT22] | [DATA] | [14] | [Digital, satu kabel data] | [Modul sudah berisi resistor *pull-up*] |
| [ ] | [ ] | [ ] | [ ] | [ ] |

## 7. Anggaran Daya (Perkiraan)

| Komponen | Tegangan | Arus tipikal | Arus puncak | Sumber angka |
| --- | :---: | :---: | :---: | --- |
| [Contoh: modul ESP32-WROOM-32E saat Wi-Fi mengirim] | 3,3 V | ±239 mA (rata-rata) | 379 mA | Lembar data modul |
| [ ] | [ ] | [ ] | [ ] | [ ] |
| **Total** | | [ ] | [ ] | |

**Catu daya yang dipilih:** [misalnya adaptor 5 V 2 A ke pin VIN], karena [alasan berdasarkan total arus puncak].

## 8. Rancangan Perangkat Lunak

> **Petunjuk:** pilih FSM, FreeRTOS, atau keduanya, lalu sertakan diagramnya.

### 8.1 Mesin Keadaan (FSM)

| Keadaan | Yang dilakukan | Pemicu → keadaan berikutnya |
| --- | --- | --- |
| [Contoh: NONAKTIF] | [LED hijau menyala, sensor diabaikan] | [PIN benar → SIAGA] |
| [ ] | [ ] | [ ] |

### 8.2 *Task* FreeRTOS

| *Task* | Tugas | Periode | Prioritas | *Stack* (byte) | Komunikasi |
| --- | --- | :---: | :---: | :---: | --- |
| [Contoh: `taskSensor`] | [Membaca DHT22] | [2 detik] | [2] | [4096] | [Mengirim ke antrean `antreanData`] |
| [ ] | [ ] | [ ] | [ ] | [ ] | [ ] |

## 9. Topik Kuliah yang Dipakai

Centang minimal tiga, lalu jelaskan singkat pemakaiannya.

- [ ] Interupsi dan *timer* (Pertemuan 5): [ ]
- [ ] ADC (Pertemuan 6): [ ]
- [ ] PWM, servo, *buzzer*, atau *relay* (Pertemuan 7): [ ]
- [ ] UART, I2C, atau SPI (Pertemuan 9): [ ]
- [ ] Sensor, aktuator, dan pustaka (Pertemuan 10): [ ]
- [ ] FSM (Pertemuan 11): [ ]
- [ ] FreeRTOS (Pertemuan 12): [ ]
- [ ] Wi-Fi dan MQTT (Pertemuan 13): [ ]
- [ ] *Deep sleep*, NVS, atau *watchdog* (Pertemuan 14): [ ]

## 10. Pembagian Tugas

| Pekerjaan | Penanggung jawab | Pendukung | Target selesai |
| --- | --- | --- | --- |
| [Contoh: rangkaian dan uji perangkat keras] | [Nama] | [Nama] | [Pertemuan 15] |
| [ ] | [ ] | [ ] | [ ] |

## 11. Jadwal Kerja

| Pekan | Target | Bukti selesai |
| --- | --- | --- |
| [Pertemuan 14] | [Proposal, membeli komponen, uji per modul] | [Proposal terkumpul, hasil program uji] |
| [Pertemuan 15] | [Integrasi tahap 1–3] | [Laporan kemajuan, *commit* per tahap] |
| [Pertemuan 16] | [Integrasi penuh, pengujian, video, laporan] | [Demo UAS] |

## 12. Risiko dan Rencana Cadangan

| Risiko | Kemungkinan | Dampak | Rencana cadangan |
| --- | :---: | :---: | --- |
| [Contoh: sensor rusak menjelang demo] | [Sedang] | [Tinggi] | [Beli dua unit; siapkan versi Wokwi] |
| [Contoh: Wi-Fi kampus menolak ESP32] | [Tinggi] | [Sedang] | [Pakai *hotspot* HP] |
| [ ] | [ ] | [ ] | [ ] |

## 13. Rencana Penggunaan AI

> **Petunjuk:** tuliskan alat AI yang akan dipakai dan untuk apa. Pemakaian yang sebenarnya dicatat lengkap di laporan akhir.

[Contoh: Kami akan memakai asisten AI untuk memahami pesan galat dan membuat draf fungsi pembacaan sensor. Semua kode hasil AI akan kami uji dan kami jelaskan di laporan.]

## Persetujuan Dosen

| | |
| --- | --- |
| **Keputusan** | ☐ Disetujui ☐ Disetujui dengan perbaikan ☐ Perlu diajukan ulang |
| **Catatan** | |
| **Tanggal dan paraf** | |
