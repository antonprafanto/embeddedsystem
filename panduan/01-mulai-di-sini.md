# Mulai di Sini: Cara Belajar dengan Repositori Ini

> Selamat datang! Halaman ini menjelaskan isi repositori, urutan belajar yang disarankan, dan cara membaca setiap modul. Cukup 5 menit, dan kamu tidak akan bingung lagi di pertemuan berikutnya.

## 1. Isi Repositori

```text
embeddedsystem/
├── README.md                      ← beranda: peta 16 pertemuan + tautan unduh slide
├── RPS.md                         ← rencana pembelajaran semester (capaian, jadwal, penilaian)
├── panduan/                       ← panduan umum (instalasi, Wokwi, pinout, masalah umum, glosarium)
├── pertemuan-01-pengenalan/       ← satu folder untuk setiap pertemuan
│   ├── README.md                  ← modul: materi + praktikum + latihan
│   ├── gambar/                    ← gambar pendukung modul
│   └── kode/                      ← proyek praktikum (satu folder = satu proyek)
│       ├── 01-halo-esp32/
│       │   ├── platformio.ini     ← pengaturan PlatformIO (jangan diubah)
│       │   ├── src/main.cpp       ← kode program
│       │   ├── diagram.json       ← rangkaian untuk simulator Wokwi
│       │   └── wokwi.toml         ← pengaturan Wokwi for VS Code (opsional)
│       └── 02-led-berkedip/
├── pertemuan-02-.../  …  pertemuan-16-uas/
├── proyek-akhir/                  ← ketentuan dan ide proyek akhir
├── slide/
│   ├── pdf/                       ← slide untuk dibaca/diunduh (PDF)
│   └── pptx/                      ← slide versi PowerPoint
└── KREDIT-GAMBAR.md               ← sumber dan lisensi semua gambar dari luar
```

## 2. Urutan Belajar yang Disarankan

1. **Sebelum Pertemuan 1:** ikuti [Panduan Instalasi Alat](02-instalasi-alat.md) sampai daftar periksanya tercentang semua.
2. **Setiap minggu, sebelum kelas:** baca bagian materi pada `README.md` pertemuan tersebut dan lihat slidenya (±30 menit).
3. **Saat kelas/praktikum:** kerjakan bagian **🧪 Praktikum** langkah demi langkah.
4. **Setelah kelas:** kerjakan **🏋️ Latihan & Tantangan** dan jawab **✅ Cek Pemahaman** untuk menguji dirimu sendiri.
5. **Bingung atau galat?** Cek dulu [Panduan Mengatasi Masalah](06-mengatasi-masalah.md) dan [Glosarium](07-glosarium.md).

## 3. Cara Membaca Modul

Setiap modul punya susunan yang sama:

| Bagian | Isinya |
| --- | --- |
| 🎯 Tujuan Pembelajaran | Kemampuan yang akan kamu miliki setelah pertemuan |
| 🧰 Yang Perlu Disiapkan | Perangkat lunak, komponen (jika ada), dan jalur simulasi |
| 1, 2, 3, … | Materi konsep: analogi, gambar, contoh kode kecil |
| 🧪 Praktikum | Langkah demi langkah: rangkaian, kode, cara menjalankan, hasil yang diharapkan |
| 🏋️ Latihan & Tantangan | Soal untuk mengasah pemahaman |
| ✅ Cek Pemahaman | Kuis singkat; jawabannya disembunyikan (klik **Lihat jawaban**) |
| 📌 Rangkuman | Poin penting dalam beberapa baris |
| 📚 Referensi | Sumber bacaan resmi untuk pendalaman |

Kotak berwarna di dalam modul punya arti khusus:

> [!NOTE]
> **Catatan** — informasi tambahan yang berguna.

> [!TIP]
> **Tips** — jalan pintas atau cara yang lebih mudah.

> [!IMPORTANT]
> **Penting** — hal yang wajib kamu pahami sebelum lanjut.

> [!WARNING]
> **Peringatan** — kesalahan yang sering terjadi dan bisa membuat program tidak jalan.

> [!CAUTION]
> **Hati-hati** — tindakan yang bisa **merusak** board atau komponen.

## 4. Dua Jalur Praktikum

Setiap praktikum bisa dikerjakan dengan salah satu jalur berikut. Kodenya **sama persis**.

| | Jalur A — Simulator Wokwi | Jalur B — Board ESP32 asli |
| --- | --- | --- |
| Yang dibutuhkan | Peramban + internet | Laptop dengan VS Code + PlatformIO, board ESP32, kabel USB data, komponen |
| Kelebihan | Gratis, cepat, tanpa risiko merusak komponen | Pengalaman nyata: merangkai, mengukur, menemukan masalah fisik |
| Panduan | [Simulasi Wokwi](03-simulasi-wokwi.md) | [Instalasi Alat](02-instalasi-alat.md) |

> [!IMPORTANT]
> **Ujian praktik dan proyek akhir memakai board asli.** Jadikan Wokwi sebagai tempat berlatih, bukan pengganti merangkai.

## 5. Melihat dan Mengunduh Slide serta Modul

- Di tabel [beranda](../README.md#️-peta-16-pertemuan) dan di bagian atas setiap modul ada tiga jenis tautan:
  - **👁️ Lihat** membuka slide langsung di peramban (PowerPoint Online), tanpa mengunduh.
  - **⬇️ PDF** / **⬇️ PPTX** langsung mengunduh slide ke perangkatmu.
  - **⬇️ PDF** di kolom modul mengunduh versi PDF modul, cocok untuk dibaca luring atau dicetak.
- Semua berkasnya juga ada di *folder* [`slide/pdf`](../slide/pdf), [`slide/pptx`](../slide/pptx), dan [`modul-pdf`](../modul-pdf).

## 6. Etika Belajar

- **Pahami, jangan sekadar menyalin.** Saat praktikum, dosen/asisten bisa meminta kamu mengubah sebagian kode dan menjelaskan alasannya.
- Boleh bertanya ke AI atau teman, tetapi pastikan kamu bisa menjelaskan **setiap baris** kode yang kamu kumpulkan.
- Catat setiap galat yang kamu temui beserta solusinya. Kebiasaan ini sangat berharga di dunia kerja.

---

[🏠 Beranda](../README.md) · [Instalasi Alat ➡️](02-instalasi-alat.md)
