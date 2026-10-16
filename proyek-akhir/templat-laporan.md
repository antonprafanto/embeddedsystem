# Laporan Akhir Proyek — [Judul Proyek]

> **Cara memakai templat ini:** salin berkas ini ke repositori kelompokmu (misalnya `dokumen/laporan-akhir.md`), isi semua bagian, lalu ekspor ke PDF (`dokumen/laporan-akhir.pdf`). Panjang ±15–25 halaman di luar lampiran *(sesuaikan)*. Ganti teks dalam tanda kurung siku `[…]` dan hapus kotak petunjuk seperti ini. Laporan boleh juga diketik ulang di pengolah kata dengan susunan bab yang sama. Rubrik penilaiannya ada di halaman [Proyek Akhir](README.md#-rubrik-penilaian).

| | |
| --- | --- |
| **Mata kuliah** | Sistem Tertanam |
| **Dosen pengampu** | [Nama dosen] |
| **Kelompok** | [NN] |
| **Tautan repositori** | [https://github.com/…] (versi final: *tag* `v1.0`) |
| **Tautan video demo** | [https://…] |
| **Tanggal** | [tanggal bulan tahun] |

| No | Nama | NIM | Peran |
| :---: | --- | --- | --- |
| 1 | [ ] | [ ] | [ ] |
| 2 | [ ] | [ ] | [ ] |
| 3 | [ ] | [ ] | [ ] |

## Ringkasan

> **Petunjuk:** maksimal 150 kata: masalah, solusi, hasil utama dalam angka, dan satu keterbatasan terpenting.

[Tulis di sini.]

## 1. Pendahuluan

### 1.1 Latar Belakang

[Siapa penggunanya dan masalah apa yang diselesaikan.]

### 1.2 Tujuan

[Tujuan yang bisa diukur keberhasilannya.]

### 1.3 Ruang Lingkup dan Batasan

[Apa yang dikerjakan dan apa yang sengaja tidak dikerjakan.]

## 2. Perancangan Sistem

### 2.1 Kebutuhan Sistem

> **Petunjuk:** salin tabel kebutuhan dari proposal, lalu tambahkan status dan bukti pengujiannya. Jelaskan bila ada kebutuhan yang berubah.

| ID | Kebutuhan | Status | Bukti (ID uji) |
| --- | --- | :---: | --- |
| F-1 | [ ] | [Terpenuhi/Sebagian/Belum] | [U-01] |
| NF-1 | [ ] | [ ] | [ ] |

### 2.2 Diagram Blok

[Sisipkan gambar diagram blok, lalu jelaskan alurnya dalam 3–5 kalimat.]

### 2.3 Perangkat Keras

**Daftar komponen**

| No | Komponen | Jumlah | Keterangan |
| :---: | --- | :---: | --- |
| 1 | ESP32 DevKit V1 | 1 | [ ] |
| 2 | [ ] | [ ] | [ ] |

**Rangkaian.** [Sisipkan skematik atau gambar rangkaian (misalnya tangkapan layar proyek Wokwi kelompokmu) beserta penjelasannya.]

**Alokasi pin**

| Komponen | Pin komponen | GPIO ESP32 | Jenis sinyal | Alasan pemilihan pin |
| --- | --- | :---: | --- | --- |
| [ ] | [ ] | [ ] | [ ] | [ ] |

**Anggaran daya**

| Komponen | Tegangan | Arus tipikal | Arus puncak | Sumber angka |
| --- | :---: | :---: | :---: | --- |
| [ ] | [ ] | [ ] | [ ] | [Lembar data atau pengukuran] |
| **Total** | | [ ] | [ ] | |

[Jelaskan pilihan catu daya. Bila memakai baterai, tuliskan perhitungan umur baterainya.]

### 2.4 Perangkat Lunak

**Arsitektur.** [Sisipkan diagram FSM atau diagram *task* FreeRTOS, lalu jelaskan.]

| Keadaan atau *task* | Tugas | Periode atau pemicu | Komunikasi |
| --- | --- | --- | --- |
| [ ] | [ ] | [ ] | [ ] |

**Pustaka yang dipakai**

| Pustaka | Versi | Lisensi | Dipakai untuk |
| --- | --- | --- | --- |
| [Contoh: Adafruit SSD1306] | [ ] | [BSD] | [Menampilkan data di OLED] |

**Komunikasi** (bila ada): [daftar topik MQTT, arah (kirim/terima), QoS, *retained*, dan contoh isi JSON.]

## 3. Implementasi

### 3.1 Foto Alat

[Sisipkan foto alat dari atas dan dari samping, dengan keterangan setiap bagian.]

### 3.2 Potongan Kode Penting

> **Petunjuk:** pilih 2–3 potongan yang paling menunjukkan pemahamanmu, misalnya transisi FSM, ISR, atau komunikasi antar-*task*. Setiap potongan maksimal ±20 baris, diikuti penjelasan mengapa kode itu ditulis seperti itu.

```cpp
// [Tempel potongan kode di sini]
```

[Penjelasan.]

### 3.3 Perubahan dari Proposal

| Rencana di proposal | Yang dikerjakan | Alasan perubahan |
| --- | --- | --- |
| [ ] | [ ] | [ ] |

## 4. Pengujian

### 4.1 Uji Fungsional

| ID uji | Kebutuhan | Langkah | Hasil yang diharapkan | Hasil nyata | Status |
| --- | --- | --- | --- | --- | :---: |
| U-01 | F-1 | [Contoh: picu sensor gerak saat SIAGA, 10 kali] | [Alarm berbunyi ≤ 1 detik] | [Rata-rata 0,4 detik; maksimum 0,7 detik] | [Lulus] |
| U-02 | [ ] | [ ] | [ ] | [ ] | [ ] |

### 4.2 Data Pengukuran

> **Petunjuk:** tampilkan data dalam tabel atau grafik, misalnya pembacaan sensor dibandingkan dengan alat ukur lain, waktu respons, atau konsumsi arus. Sebutkan alat ukur yang dipakai.

[Tulis di sini.]

### 4.3 Uji Ketahanan dan Gangguan

| Skenario | Yang diamati | Hasil |
| --- | --- | --- |
| Kabel sensor dicabut saat sistem berjalan | Pesan galat muncul dan sistem tidak macet | [ ] |
| Wi-Fi diputus 5 menit, lalu dinyalakan lagi | Sistem tetap bekerja luring, lalu tersambung kembali | [ ] |
| Sistem berjalan 2 jam tanpa henti | Tidak ada *reset* atau macet | [ ] |

## 5. Pembahasan

> **Petunjuk:** jelaskan **mengapa** hasilnya seperti itu, bukan sekadar mengulang isi tabel.

**Analisis hasil.** [Tulis di sini.]

**Kendala dan solusi**

| Kendala | Penyebab | Solusi |
| --- | --- | --- |
| [ ] | [ ] | [ ] |

**Keterbatasan.** [Tulis di sini.]

## 6. Kesimpulan dan Saran

**Kesimpulan:** [Jawab tujuan di bagian 1.2 dengan bukti dari bagian 4.]

**Saran pengembangan:** [Tulis di sini.]

## 7. Pembagian Kerja

| Anggota | Pekerjaan utama | Kontribusi (%) | Bukti (*commit*, berkas) |
| --- | --- | :---: | --- |
| [ ] | [ ] | [ ] | [ ] |

## 8. Pernyataan Penggunaan AI dan Sumber Kode

**Penggunaan AI** (tulis "Tidak ada" bila kelompokmu tidak memakai AI)

| Alat | Dipakai untuk | Bagian yang terdampak | Cara kami memverifikasi |
| --- | --- | --- | --- |
| [Nama alat AI] | [ ] | [Berkas dan fungsi] | [ ] |

**Kode dari sumber lain**

| Sumber (URL) | Bagian yang diadaptasi | Lisensi |
| --- | --- | --- |
| [ ] | [ ] | [ ] |

## 9. Pernyataan Keaslian

Kami menyatakan bahwa proyek dan laporan ini adalah hasil kerja kelompok kami sendiri. Semua bantuan AI dan sumber dari pihak lain sudah kami cantumkan di bagian 8. Setiap anggota memahami dan mampu menjelaskan seluruh kode yang kami kumpulkan.

| Nama | NIM | Tanda tangan |
| --- | --- | --- |
| [ ] | [ ] | |
| [ ] | [ ] | |
| [ ] | [ ] | |

## Referensi

> **Petunjuk:** minimal lembar data setiap komponen utama dan dokumentasi pustaka yang dipakai. Format: Penulis/Lembaga. (Tahun). *Judul*. Penerbit/Situs. URL

1. [Contoh: Espressif Systems. (2025). *ESP32 Series Datasheet*. https://documentation.espressif.com/esp32_datasheet_en.pdf]
2. [ ]

## Lampiran

- A. Tautan repositori dan *tag* versi final
- B. Tautan lembar data komponen
- C. Log pengujian lengkap
- D. Foto tambahan
