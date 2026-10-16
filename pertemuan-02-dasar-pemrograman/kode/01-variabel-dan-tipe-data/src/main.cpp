/*
 * =====================================================================
 *  Pertemuan 02 · Praktikum 1 — Variabel dan Tipe Data
 * =====================================================================
 *  Tujuan   : Mengenal variabel, konstanta (const dan #define), ukuran
 *             (bita/byte) dan rentang nilai tipe data di ESP32, lalu
 *             membuktikan jebakan pemula: nilai meluap (overflow),
 *             pembagian bilangan bulat, konversi tipe, dan membandingkan
 *             pecahan dengan ==.
 *  Rangkaian: Tidak perlu komponen. Cukup board ESP32 (atau Wokwi).
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (ikon colokan).
 *    - Wokwi web  : salin seluruh isi berkas ini ke sketch.ino, lalu klik ▶.
 *  Hasil yang diharapkan (Serial Monitor, 115200 baud), antara lain:
 *    int       : 4 bita
 *    uint8_t 255 + 1 = 0
 *    7 / 2        = 3
 *    suhu == 36.6         -> 0
 *    (Keluaran lengkap ada di README.md pertemuan ini.)
 * =====================================================================
 */
#include <Arduino.h>

// ---------------------------------------------------------------------
// Dua cara membuat nilai tetap (konstanta):
//  - #define : pengganti teks sebelum kompilasi, tidak punya tipe data.
//  - const   : variabel bertipe yang dikunci. Cara ini lebih dianjurkan
//              karena kompilator ikut memeriksa tipenya.
// ---------------------------------------------------------------------
#define JUMLAH_SENSOR 4
const float BATAS_PANAS = 35.0;

// ---------------------------------------------------------------------
// Variabel global: dibuat di luar fungsi, sehingga dikenal di setup()
// maupun loop(). Format: tipe nama = nilaiAwal;
// ---------------------------------------------------------------------
float suhuRuangan = 28.75;    // bilangan pecahan       -> float (4 bita)
int jumlahPembacaan = 120;    // bilangan bulat         -> int   (4 bita di ESP32)
bool lampuMenyala = true;     // benar/salah            -> bool  (1 bita)
char kodeRuang = 'B';         // satu karakter          -> char  (1 bita)
uint8_t kecerahan = 200;      // bulat 0..255 saja      -> uint8_t (1 bita)

void setup() {
  Serial.begin(115200);
  delay(1000);                // beri waktu Serial Monitor siap

  Serial.println();
  Serial.println("=== Praktikum 1: Variabel dan Tipe Data ===");

  // -------------------------------------------------------------------
  // [1] Menampilkan isi variabel dan konstanta.
  //     %d = bilangan bulat, %.2f = pecahan 2 angka di belakang titik,
  //     %c = satu karakter, \n = pindah baris.
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[1] Isi variabel dan konstanta:");
  Serial.printf("  suhuRuangan     = %.2f\n", suhuRuangan);
  Serial.printf("  jumlahPembacaan = %d\n", jumlahPembacaan);
  Serial.printf("  lampuMenyala    = %d (1 = true, 0 = false)\n", lampuMenyala);
  Serial.printf("  kodeRuang       = %c (disimpan sebagai angka %d)\n", kodeRuang, kodeRuang);
  Serial.printf("  kecerahan       = %d\n", kecerahan);
  Serial.printf("  BATAS_PANAS     = %.1f (const)\n", BATAS_PANAS);
  Serial.printf("  JUMLAH_SENSOR   = %d (#define)\n", JUMLAH_SENSOR);

  // -------------------------------------------------------------------
  // [2] Ukuran tipe data. sizeof(tipe) = berapa bita (byte) memori yang dipakai.
  //     Hasil sizeof bertipe size_t (bulat tanpa tanda), dicetak dengan %u.
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[2] Ukuran tipe data di ESP32 (sizeof):");
  Serial.printf("  bool      : %u bita\n", sizeof(bool));
  Serial.printf("  char      : %u bita\n", sizeof(char));
  Serial.printf("  uint8_t   : %u bita\n", sizeof(uint8_t));
  Serial.printf("  int16_t   : %u bita\n", sizeof(int16_t));
  Serial.printf("  int       : %u bita\n", sizeof(int));
  Serial.printf("  uint32_t  : %u bita\n", sizeof(uint32_t));
  Serial.printf("  long      : %u bita\n", sizeof(long));
  Serial.printf("  long long : %u bita\n", sizeof(long long));
  Serial.printf("  float     : %u bita\n", sizeof(float));
  Serial.printf("  double    : %u bita\n", sizeof(double));

  // -------------------------------------------------------------------
  // [3] Rentang nilai tipe berukuran pasti (dari pustaka <stdint.h>).
  //     Di ESP32, int32_t dan uint32_t bertipe long dan unsigned long,
  //     sehingga dicetak dengan %ld dan %lu.
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[3] Rentang nilai tipe berukuran pasti:");
  Serial.printf("  uint8_t  : %d sampai %d\n", 0, UINT8_MAX);
  Serial.printf("  int16_t  : %d sampai %d\n", INT16_MIN, INT16_MAX);
  Serial.printf("  uint32_t : %lu sampai %lu\n", 0UL, UINT32_MAX);
  Serial.printf("  int32_t  : %ld sampai %ld\n", INT32_MIN, INT32_MAX);

  // -------------------------------------------------------------------
  // [4] Overflow: nilai melewati batas, lalu "berputar" seperti
  //     odometer motor. uint8_t hanya muat 0..255.
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[4] Overflow (nilai meluap):");
  uint8_t angka = 255;
  angka = angka + 1;          // 256 tidak muat di 8 bit -> kembali ke 0
  Serial.printf("  uint8_t 255 + 1 = %d\n", angka);
  angka = 0;
  angka = angka - 1;          // di bawah 0 -> berputar ke 255
  Serial.printf("  uint8_t   0 - 1 = %d\n", angka);

  // -------------------------------------------------------------------
  // [5] Pembagian dan konversi tipe. Bulat dibagi bulat = bulat
  //     (pecahannya DIBUANG). Supaya hasilnya pecahan, salah satu angka
  //     harus bertipe pecahan. Di dalam printf, %% mencetak tanda persen.
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[5] Pembagian dan konversi tipe:");
  int a = 7;
  int b = 2;
  Serial.printf("  7 / 2        = %d    <- bulat: pecahannya dibuang\n", a / b);
  Serial.printf("  7 %% 2        = %d    <- sisa bagi (modulo)\n", a % b);
  Serial.printf("  7 / 2.0      = %.2f <- pecahan\n", a / 2.0);
  Serial.printf("  (float)7 / 2 = %.2f <- a diubah dulu ke float\n", (float)a / b);
  float nilaiPecahan = 3.99;
  int nilaiBulat = (int)nilaiPecahan;   // pecahan -> bulat: angka di belakang titik DIPOTONG
  Serial.printf("  (int)3.99    = %d    <- dipotong, bukan dibulatkan\n", nilaiBulat);

  // -------------------------------------------------------------------
  // [6] Membandingkan pecahan. Banyak pecahan (misalnya 36.6) tidak bisa
  //     disimpan persis dalam biner, jadi jangan dibandingkan dengan ==.
  //     Bandingkan selisihnya dengan toleransi kecil. fabs() = nilai mutlak.
  // -------------------------------------------------------------------
  Serial.println();
  Serial.println("[6] Membandingkan pecahan:");
  float suhu = 36.6;
  Serial.printf("  36.6 tersimpan sebagai %.10f\n", suhu);
  Serial.printf("  suhu == 36.6         -> %d  <- 0 artinya salah\n", suhu == 36.6);
  Serial.printf("  |suhu - 36.6| < 0.01 -> %d  <- 1 artinya benar\n", fabs(suhu - 36.6) < 0.01);

  Serial.println();
  Serial.println("Selesai. Ubah nilai variabel di kode, lalu jalankan ulang!");
}

void loop() {
  // Semua contoh sudah dijalankan sekali di setup(). Tidak ada yang diulang.
}
