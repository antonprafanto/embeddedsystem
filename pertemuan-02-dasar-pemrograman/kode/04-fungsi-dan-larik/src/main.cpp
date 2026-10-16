/*
 * =====================================================================
 *  Pertemuan 02 · Praktikum 4 — Fungsi dan Larik
 * =====================================================================
 *  Tujuan   : Membuat fungsi sendiri (dengan parameter dan nilai balik)
 *             untuk mengolah larik (array) berisi pembacaan sensor suhu:
 *             konversi °C ke °F, rata-rata, dan nilai maksimum. Lalu
 *             melihat beda variabel lokal biasa dengan static, serta dua
 *             cara menyimpan teks: larik char dan String.
 *  Rangkaian: Tidak perlu komponen. Data sensor disimulasikan dalam larik.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (ikon colokan).
 *    - Wokwi web  : salin seluruh isi berkas ini ke sketch.ino, lalu klik ▶.
 *  Hasil yang diharapkan (Serial Monitor, 115200 baud), antara lain:
 *    [0] 26.5 C = 79.7 F
 *    Rata-rata : 28.92 C
 *    Panggilan ke-3: lokal = 1, static = 3
 *    (Keluaran lengkap ada di README.md pertemuan ini.)
 * =====================================================================
 */
#include <Arduino.h>

// ---------------------------------------------------------------------
// Larik (array) berisi 6 pembacaan sensor suhu dalam °C.
// Indeks larik dimulai dari 0: pembacaanSuhu[0] ... pembacaanSuhu[5].
// ---------------------------------------------------------------------
float pembacaanSuhu[] = {26.5, 27.8, 29.4, 31.6, 30.2, 28.0};

// Banyaknya isi larik dihitung otomatis:
// ukuran seluruh larik (byte) dibagi ukuran satu elemen (byte).
const int JUMLAH_PEMBACAAN = sizeof(pembacaanSuhu) / sizeof(pembacaanSuhu[0]);

// ---------------------------------------------------------------------
// Prototipe (deklarasi) fungsi. Ditulis di atas supaya fungsi boleh
// dipanggil di setup() walaupun isinya baru ditulis di bagian bawah.
// Kata const pada parameter larik = fungsi berjanji tidak mengubah isinya.
// ---------------------------------------------------------------------
float celsiusKeFahrenheit(float celsius);
float rataRata(const float data[], int jumlah);
float nilaiMaksimum(const float data[], int jumlah);
int indeksMaksimum(const float data[], int jumlah);
void cetakGaris(int panjang);
void hitungPanggilan();

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== Praktikum 4: Fungsi dan Larik ===");

  // [1] Memanggil fungsi untuk setiap isi larik
  Serial.println();
  Serial.printf("[1] Data suhu dari sensor (%d pembacaan):\n", JUMLAH_PEMBACAAN);
  for (int i = 0; i < JUMLAH_PEMBACAAN; i++) {
    float fahrenheit = celsiusKeFahrenheit(pembacaanSuhu[i]);   // panggil fungsi
    Serial.printf("  [%d] %.1f C = %.1f F\n", i, pembacaanSuhu[i], fahrenheit);
  }

  // [2] Fungsi yang menerima seluruh larik sebagai parameter
  Serial.println();
  Serial.println("[2] Olahan data:");
  cetakGaris(32);
  Serial.printf("  Rata-rata : %.2f C\n", rataRata(pembacaanSuhu, JUMLAH_PEMBACAAN));
  Serial.printf("  Maksimum  : %.2f C (pembacaan ke-%d)\n",
                nilaiMaksimum(pembacaanSuhu, JUMLAH_PEMBACAAN),
                indeksMaksimum(pembacaanSuhu, JUMLAH_PEMBACAAN));
  Serial.printf("  Air mendidih: 100 C = %.1f F\n", celsiusKeFahrenheit(100.0));
  cetakGaris(32);

  // [3] Fungsi yang sama dipanggil tiga kali
  Serial.println();
  Serial.println("[3] Variabel lokal biasa vs static:");
  for (int i = 0; i < 3; i++) {
    hitungPanggilan();
  }

  // [4] Dua cara menyimpan teks
  Serial.println();
  Serial.println("[4] Teks: larik char vs String:");
  char namaSensor[] = "DHT22";      // larik char: 5 huruf + 1 penutup '\0' = 6 bita
  String lokasi = "Lab";            // String: teks yang panjangnya bisa berubah
  lokasi += " Elektro";             // menyambung teks
  Serial.printf("  namaSensor = %s (%u huruf, %u bita)\n",
                namaSensor, strlen(namaSensor), sizeof(namaSensor));
  // c_str() mengubah String menjadi teks C (larik char) yang dipahami %s
  Serial.printf("  lokasi     = %s (%u huruf)\n", lokasi.c_str(), lokasi.length());
}

void loop() {
  // Semua contoh sudah dijalankan sekali di setup().
}

// ---------------------------------------------------------------------
// Definisi (isi) fungsi
// ---------------------------------------------------------------------

// Mengubah suhu Celsius menjadi Fahrenheit: F = C × 9/5 + 32
float celsiusKeFahrenheit(float celsius) {
  return celsius * 9.0 / 5.0 + 32.0;
}

// Menghitung rata-rata isi larik: jumlahkan semua, lalu bagi banyaknya data
float rataRata(const float data[], int jumlah) {
  float total = 0.0;
  for (int i = 0; i < jumlah; i++) {
    total += data[i];               // sama dengan total = total + data[i]
  }
  return total / jumlah;
}

// Mencari nilai terbesar di dalam larik
float nilaiMaksimum(const float data[], int jumlah) {
  float terbesar = data[0];         // anggap dulu data pertama yang terbesar
  for (int i = 1; i < jumlah; i++) {
    if (data[i] > terbesar) {
      terbesar = data[i];           // ketemu yang lebih besar, ganti
    }
  }
  return terbesar;
}

// Mencari POSISI (indeks) nilai terbesar di dalam larik
int indeksMaksimum(const float data[], int jumlah) {
  int indeks = 0;
  for (int i = 1; i < jumlah; i++) {
    if (data[i] > data[indeks]) {
      indeks = i;
    }
  }
  return indeks;
}

// Fungsi void: tidak mengembalikan nilai, hanya mencetak garis pemisah
void cetakGaris(int panjang) {
  for (int i = 0; i < panjang; i++) {
    Serial.print('-');
  }
  Serial.println();
}

// Membandingkan variabel lokal biasa dengan variabel lokal static
void hitungPanggilan() {
  int lokal = 0;                    // dibuat ulang dari 0 setiap kali fungsi dipanggil
  static int total = 0;             // static: dibuat sekali, nilainya diingat antarpanggilan
  lokal++;
  total++;
  Serial.printf("  Panggilan ke-%d: lokal = %d, static = %d\n", total, lokal, total);
}
