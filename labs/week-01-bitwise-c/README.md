# 📝 JOBSHEET PRAKTIKUM MINGGU 1
## Topik: Fondasi Bahasa C untuk Sistem Tertanam & Manipulasi Bitwise

* **Mata Kuliah:** Sistem Tertanam (*Embedded Systems*) - EE-304
* **Alokasi Waktu:** 170 Menit (Sesi Lab Terpandu)
* **Target Board:** ESP32 Development Board

---

## 🎯 1. TUJUAN PRAKTIKUM
Setelah menyelesaikan modul praktikum ini, mahasiswa diharapkan mampu:
1. Mengaplikasikan operator bitwise (`&`, `|`, `^`, `~`, `<<`, `>>`) untuk memanipulasi bit register secara mandiri.
2. Memahami alasan mengapa fungsi `delay()` terlarang dalam sistem tertanam industri dan mampu menggantikannya dengan arsitektur *non-blocking state machine*.
3. Menggunakan fungsi `Serial` monitor untuk men-debug status register dalam format biner dan heksadesimal.

---

## 📚 2. DASAR TEORI SINGKAT

Dalam sistem tertanam, memori dan efisiensi waktu eksekusi adalah batasan utama. Mikrokontroler mengontrol periferal fisik (GPIO, Timer, ADC) melalui **Special Function Registers (SFR)**. Satu byte register (8-bit) sering kali memuat 8 status berbeda sekaligus.

### 4 Rumus Emas Bit-Masking:
1. **Menyalakan Bit ke-n (Set to 1):**  
   $$\text{Reg} = \text{Reg} \mid (1 \ll n)$$  
   *Contoh:* `reg |= (1 << 3);` $\rightarrow$ Menyalakan bit ke-3 tanpa mengganggu bit lain.

2. **Mematikan Bit ke-n (Clear to 0):**  
   $$\text{Reg} = \text{Reg} \ \& \ \sim(1 \ll n)$$  
   *Contoh:* `reg &= ~(1 << 3);` $\rightarrow$ Mematikan bit ke-3 tanpa mengganggu bit lain.

3. **Membalik Nilai Bit ke-n (Toggle):**  
   $$\text{Reg} = \text{Reg} \oplus (1 \ll n)$$  
   *Contoh:* `reg ^= (1 << 3);` $\rightarrow$ Nilai 0 jadi 1, nilai 1 jadi 0.

4. **Memeriksa Status Bit ke-n (Check Bit):**  
   $$\text{Status} = (\text{Reg} \ \& \ (1 \ll n)) \neq 0$$  
   *Contoh:* `if (reg & (1 << 3)) { ... }` $\rightarrow$ True jika bit ke-3 bernilai 1.

---

## 🛠️ 3. TUGAS PRAKTIKUM BERJENJANG (TIERED ASSIGNMENT)

### 🟢 Level 1: Penguasaan Operator Bitwise (Wajib - Skor: 70)
1. Buka folder proyek `labs/week-01-bitwise-c/` di VS Code / PlatformIO.
2. Buka file `src/main.cpp`.
3. Teliti implementasi pada fungsi:
   * `set_register_bit()`
   * `clear_register_bit()`
   * `toggle_register_bit()`
   * `check_register_bit()`
4. Lakukan kompilasi (*Build*) dan unggah (*Upload*) ke ESP32.
5. Buka Serial Monitor (baud rate: `115200`), catat perubahan status biner sebelum dan sesudah operasi bitwise dipanggil.

### 🟡 Level 2: Analisis Loop Non-Blocking (Wajib - Skor: 85)
1. Perhatikan fungsi `run_non_blocking_led_state_machine()`.
2. Jelaskan dalam laporan Anda: **Mengapa LED bisa berkedip teratur tanpa menggunakan instruksi `delay(500)`?**
3. Ubah nilai `BLINK_INTERVAL_MS` menjadi `200` ms, lakukan upload ulang, dan amati perubahan ritme pada LED dan register `BIT_COMM_OK`.

### 🔴 Level 3: Tantangan Biner (Tantangan Ekstra - Skor: 100)
1. Buat sebuah fungsi baru bernama `run_4bit_binary_counter()`.
2. Fungsi ini menaikkan nilai penghitung dari `0b0000` hingga `0b1111` (0 sampai 15 desimal) setiap 1 detik.
3. Cetak nilai penghitung tersebut ke Serial Monitor dalam format biner dan desimal.
4. Pastikan sistem tetap berjalan secara *non-blocking*!

---

## 📋 4. METODE ASESMEN DI LAB (LIVE CODE MUTATION)
Saat mendemonstrasikan hasil praktikum kepada asisten lab / dosen:
* Asisten akan meminta Anda **mengubah bit target atau interval waktu secara mendadak**.
* Contoh pertanyaan uji lisan: *"Coba ubah bit ke-5 menjadi 1 tanpa menggunakan fungsi pembantu, bagaimana sintaks satu barisnya?"*
* Mahasiswa yang mampu menjawab dan mengeksekusi dalam waktu < 2 menit mendapatkan nilai sempurna.

---

## 📤 5. PANDUAN PENGUMPULAN
1. Pastikan kode Anda rapi dan memiliki komentar penjelasan.
2. Commit perubahan kode Anda ke repository lokal:
   ```bash
   git add .
   git commit -m "feat(week-01): selesai tugas praktikum level 1 dan 2"
   ```
3. Push ke repository GitHub masing-masing mahasiswa.
