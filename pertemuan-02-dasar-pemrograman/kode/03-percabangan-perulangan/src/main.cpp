/*
 * =====================================================================
 *  Pertemuan 02 · Praktikum 3 — Percabangan dan Perulangan
 * =====================================================================
 *  Tujuan   : Berlatih if / else if / else, switch-case, for, while,
 *             do-while, break, dan continue lewat dua contoh:
 *             (1) mengelompokkan data suhu sensor (simulasi) dan
 *             (2) mencetak tabel perkalian dengan for bersarang.
 *  Rangkaian: Tidak perlu komponen. Data sensor disimulasikan dalam larik.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (ikon colokan).
 *    - Wokwi web  : salin seluruh isi berkas ini ke sketch.ino, lalu klik ▶.
 *  Hasil yang diharapkan (Serial Monitor, 115200 baud), antara lain:
 *    Data ke-0: 24.5 C -> SEJUK, kipas MATI
 *    Data ke-3: [LEWATI] data rusak (-127.0)
 *    Ringkasan: sejuk 2, nyaman 2, panas 2, sangat panas 1, rusak 1
 *    (Keluaran lengkap ada di README.md pertemuan ini.)
 * =====================================================================
 */
#include <Arduino.h>

// Data suhu (°C) hasil simulasi sensor. Nilai -127.0 menandakan sensor
// gagal dibaca (beberapa sensor suhu digital memakai kode ini).
const float DATA_SUHU[] = {24.5, 27.0, 30.0, -127.0, 33.4, 35.9, 29.9, 22.8};
const int JUMLAH_DATA = 8;            // banyaknya isi larik (cara otomatis: Praktikum 4)
const float BATAS_BAWAH_SAH = -50.0;  // suhu ruangan di bawah ini = data rusak
const int UKURAN_TABEL = 5;           // tabel perkalian 1..5

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== Praktikum 3: Percabangan dan Perulangan ===");

  // -------------------------------------------------------------------
  // [1] for + if/else if/else + switch-case + continue
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[1] Klasifikasi 8 data suhu:");

  int jumlahSejuk = 0, jumlahNyaman = 0, jumlahPanas = 0;
  int jumlahSangatPanas = 0, jumlahRusak = 0;

  for (int i = 0; i < JUMLAH_DATA; i++) {
    float suhu = DATA_SUHU[i];

    if (suhu < BATAS_BAWAH_SAH) {   // pecahan tidak dibandingkan dengan ==
      Serial.printf("  Data ke-%d: [LEWATI] data rusak (%.1f)\n", i, suhu);
      jumlahRusak++;
      continue;               // langsung lompat ke data berikutnya
    }

    // Percabangan bertingkat: diperiksa dari atas, yang pertama benar dipakai
    int level;                // 0 = sejuk, 1 = nyaman, 2 = panas, 3 = sangat panas
    if (suhu < 25.0) {
      level = 0;
      jumlahSejuk++;
    } else if (suhu < 30.0) {
      level = 1;
      jumlahNyaman++;
    } else if (suhu < 35.0) {
      level = 2;
      jumlahPanas++;
    } else {
      level = 3;
      jumlahSangatPanas++;
    }

    Serial.printf("  Data ke-%d: %.1f C -> ", i, suhu);

    // switch-case: memilih satu dari beberapa kemungkinan nilai level
    switch (level) {
      case 0:
        Serial.println("SEJUK, kipas MATI");
        break;
      case 1:
        Serial.println("NYAMAN, kipas PELAN");
        break;
      case 2:
        Serial.println("PANAS, kipas SEDANG");
        break;
      default:
        Serial.println("SANGAT PANAS, kipas KENCANG");
        break;
    }
  }

  Serial.printf("  Ringkasan: sejuk %d, nyaman %d, panas %d, sangat panas %d, rusak %d\n",
                jumlahSejuk, jumlahNyaman, jumlahPanas, jumlahSangatPanas, jumlahRusak);

  // -------------------------------------------------------------------
  // [2] break: hentikan perulangan begitu yang dicari sudah ketemu
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[2] Cari suhu pertama yang >= 35.0 C:");
  int indeksKetemu = -1;      // -1 artinya "belum ketemu"
  for (int i = 0; i < JUMLAH_DATA; i++) {
    if (DATA_SUHU[i] >= 35.0) {
      indeksKetemu = i;
      break;                  // keluar dari for, sisa data tidak diperiksa
    }
  }
  if (indeksKetemu >= 0) {
    Serial.printf("  Ketemu di data ke-%d (%.1f C)\n", indeksKetemu, DATA_SUHU[indeksKetemu]);
  } else {
    Serial.println("  Tidak ada.");
  }

  // -------------------------------------------------------------------
  // [3] while: ulangi SELAMA syaratnya benar
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[3] Hitung mundur dengan while:");
  int hitung = 3;
  Serial.print("  ");
  while (hitung > 0) {
    Serial.printf("%d... ", hitung);
    hitung--;                 // sama dengan hitung = hitung - 1
  }
  Serial.println("Mulai!");

  // -------------------------------------------------------------------
  // [4] while vs do-while saat syaratnya sudah salah sejak awal
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[4] while vs do-while (syarat: n < 3, padahal n = 5):");
  int n = 5;
  while (n < 3) {
    Serial.println("  while: badan perulangan dijalankan");   // tidak pernah tercetak
    n++;
  }
  n = 5;
  do {
    Serial.println("  do-while: badan perulangan dijalankan sekali");
    n++;
  } while (n < 3);

  // -------------------------------------------------------------------
  // [5] for bersarang: tabel perkalian
  // -------------------------------------------------------------------
  Serial.println();
  Serial.printf("[5] Tabel perkalian 1-%d:\n", UKURAN_TABEL);
  Serial.print("   x |");
  for (int kolom = 1; kolom <= UKURAN_TABEL; kolom++) {
    Serial.printf("%4d", kolom);
  }
  Serial.println();
  Serial.print("  ---+");
  for (int kolom = 1; kolom <= UKURAN_TABEL; kolom++) {
    Serial.print("----");
  }
  Serial.println();
  for (int baris = 1; baris <= UKURAN_TABEL; baris++) {
    Serial.printf("  %2d |", baris);
    for (int kolom = 1; kolom <= UKURAN_TABEL; kolom++) {
      Serial.printf("%4d", baris * kolom);   // %4d = lebar 4 karakter, rata kanan
    }
    Serial.println();
  }
}

void loop() {
  // Semua contoh sudah dijalankan sekali di setup().
}
