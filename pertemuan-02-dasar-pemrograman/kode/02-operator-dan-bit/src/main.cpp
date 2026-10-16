/*
 * =====================================================================
 *  Pertemuan 02 · Praktikum 2 — Operator dan Bit
 * =====================================================================
 *  Tujuan   : Mencoba operator aritmetika, penugasan majemuk (+=, -=, ...),
 *             ++ dan --, perbandingan, serta logika (&&, ||, !). Lalu
 *             memakai operator bit (&, |, ^, ~, <<, >>) untuk menyalakan,
 *             memadamkan, dan membalik 8 "lampu" yang disimpan sebagai
 *             bit di dalam satu bita, mirip cara kerja register
 *             mikrokontroler.
 *  Rangkaian: Tidak perlu komponen. Lampu hanya disimulasikan sebagai bit.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (ikon colokan).
 *    - Wokwi web  : salin seluruh isi berkas ini ke sketch.ino, lalu klik ▶.
 *  Hasil yang diharapkan (Serial Monitor, 115200 baud), antara lain:
 *    skor /= 5   -> 4
 *    Nyalakan lampu 3  : 00001000  (0x08 =   8)
 *    5 << 1  = 10  (sama dengan 5 x 2)
 *    (Keluaran lengkap ada di README.md pertemuan ini.)
 * =====================================================================
 */
#include <Arduino.h>

// Prototipe fungsi (isinya ada di bawah loop)
void tampilkanBit(const char *keterangan, uint8_t nilai);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== Praktikum 2: Operator dan Bit ===");

  // -------------------------------------------------------------------
  // [1] Aritmetika dan penugasan majemuk: "skor += 5" = "skor = skor + 5"
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[1] Aritmetika dan penugasan majemuk:");
  int skor = 10;
  Serial.printf("  skor = 10   -> %d\n", skor);
  skor += 5;
  Serial.printf("  skor += 5   -> %d\n", skor);
  skor -= 3;
  Serial.printf("  skor -= 3   -> %d\n", skor);
  skor *= 2;
  Serial.printf("  skor *= 2   -> %d\n", skor);
  skor /= 5;                  // 24 / 5 = 4 (pembagian bulat, sisanya dibuang)
  Serial.printf("  skor /= 5   -> %d\n", skor);
  skor %= 3;                  // sisa 4 dibagi 3 = 1
  Serial.printf("  skor %%= 3   -> %d\n", skor);
  skor++;                     // tambah 1
  Serial.printf("  skor++      -> %d\n", skor);
  skor--;                     // kurangi 1
  Serial.printf("  skor--      -> %d\n", skor);

  // -------------------------------------------------------------------
  // [2] Perbandingan menghasilkan benar (1) atau salah (0).
  //     Logika: && (DAN), || (ATAU), ! (BUKAN).
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[2] Perbandingan dan logika (1 = benar, 0 = salah):");
  int suhu = 31;
  int lembap = 75;
  Serial.printf("  suhu = %d, lembap = %d\n", suhu, lembap);
  Serial.printf("  suhu > 30                 -> %d\n", suhu > 30);
  Serial.printf("  lembap >= 80              -> %d\n", lembap >= 80);
  Serial.printf("  suhu > 30 && lembap >= 80 -> %d  (DAN: keduanya harus benar)\n", suhu > 30 && lembap >= 80);
  Serial.printf("  suhu > 30 || lembap >= 80 -> %d  (ATAU: cukup salah satu)\n", suhu > 30 || lembap >= 80);
  Serial.printf("  !(suhu > 30)              -> %d  (BUKAN: membalik)\n", !(suhu > 30));
  Serial.printf("  suhu == 31                -> %d  (== membandingkan, = mengisi)\n", suhu == 31);

  // -------------------------------------------------------------------
  // [3] Operator bit. Satu bita = 8 bit = 8 "sakelar lampu" (bit 7 ... bit 0).
  //     (1 << n) membuat pola yang hanya bit ke-n-nya bernilai 1.
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[3] Operator bit: 8 lampu dalam satu bita (bit 7 ... bit 0):");
  uint8_t lampu = 0b00000000;         // awalan 0b = bilangan biner
  tampilkanBit("Awal", lampu);
  lampu |= (1 << 3);                  // OR : nyalakan bit 3, bit lain tetap
  tampilkanBit("Nyalakan lampu 3", lampu);
  lampu |= (1 << 0);
  tampilkanBit("Nyalakan lampu 0", lampu);
  lampu |= (1 << 7);
  tampilkanBit("Nyalakan lampu 7", lampu);
  lampu &= ~(1 << 3);                 // AND NOT: padamkan bit 3 saja
  tampilkanBit("Padamkan lampu 3", lampu);
  lampu ^= (1 << 1);                  // XOR: balik bit 1 (0 jadi 1)
  tampilkanBit("Balik lampu 1", lampu);
  lampu ^= (1 << 1);                  // dibalik lagi (1 jadi 0)
  tampilkanBit("Balik lampu 1 lagi", lampu);
  uint8_t kebalikan = ~lampu;         // NOT: balik semua bit sekaligus
  tampilkanBit("~lampu (semua)", kebalikan);
  // Membaca satu bit: geser bit ke-n ke posisi paling kanan, lalu saring dengan & 1
  Serial.printf("  Lampu 7 menyala? %d\n", (lampu >> 7) & 1);
  Serial.printf("  Lampu 3 menyala? %d\n", (lampu >> 3) & 1);

  // -------------------------------------------------------------------
  // [4] Geser bit: geser kiri 1 langkah = kali 2, geser kanan 1 langkah = bagi 2
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[4] Geser bit (shift):");
  Serial.printf("  5 << 1  = %d  (sama dengan 5 x 2)\n", 5 << 1);
  Serial.printf("  5 << 3  = %d  (sama dengan 5 x 8)\n", 5 << 3);
  Serial.printf("  40 >> 2 = %d  (sama dengan 40 / 4)\n", 40 >> 2);
}

void loop() {
  // Semua contoh sudah dijalankan sekali di setup().
}

// Mencetak keterangan, 8 bit (bit 7 di kiri), lalu nilai heksadesimal dan desimal
void tampilkanBit(const char *keterangan, uint8_t nilai) {
  Serial.printf("  %-18s: ", keterangan);           // %-18s = teks rata kiri, lebar 18
  for (int bit = 7; bit >= 0; bit--) {
    Serial.print((nilai >> bit) & 1);                // cetak bit ke-'bit' (0 atau 1)
  }
  Serial.printf("  (0x%02X = %3d)\n", nilai, nilai);  // %02X = heksadesimal 2 digit
}
