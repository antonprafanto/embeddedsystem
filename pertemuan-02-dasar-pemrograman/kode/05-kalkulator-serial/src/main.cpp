/*
 * =====================================================================
 *  Pertemuan 02 · Praktikum 5 — Kalkulator Serial
 * =====================================================================
 *  Tujuan   : Membaca satu baris soal hitungan dari Serial Monitor
 *             (misalnya 12 + 5), menghitungnya, lalu mengirim hasilnya.
 *             Program menolak pembagian dengan nol dan masukan yang tidak
 *             valid. Semua materi Pertemuan 2 dipakai di sini: variabel,
 *             percabangan, perulangan, fungsi, string, dan Serial.
 *  Rangkaian: Tidak perlu komponen.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (ikon colokan).
 *                   Ketik soal di terminal, lalu tekan Enter.
 *    - Wokwi web  : salin seluruh isi berkas ini ke sketch.ino, klik ▶,
 *                   ketik soal di kotak input Serial Monitor, lalu Enter.
 *  Hasil yang diharapkan (Serial Monitor, 115200 baud), antara lain:
 *    12 + 5 = 17.00
 *    9 / 0 -> [GALAT] tidak bisa membagi dengan nol
 *    halo -> [GALAT] operator tidak ditemukan
 * =====================================================================
 */
#include <Arduino.h>

// ---------------------------------------------------------------------
// Prototipe fungsi (isinya ada di bawah loop)
// ---------------------------------------------------------------------
void cetakPetunjuk();
void prosesSoal(String soal);
int cariOperator(String soal);
bool apakahAngka(String teks);

void setup() {
  Serial.begin(115200);
  // Beri waktu mengetik hingga 20 detik per huruf (bawaannya hanya 1 detik).
  // Di PlatformIO setiap huruf langsung dikirim begitu diketik.
  Serial.setTimeout(20000);
  delay(1000);
  cetakPetunjuk();
}

void loop() {
  if (Serial.available() > 0) {                    // ada huruf yang masuk?
    String soal = Serial.readStringUntil('\n');    // baca sampai Enter (LF)
    soal.trim();                                   // buang spasi, CR, LF di kedua ujung
    if (soal.length() > 0) {
      prosesSoal(soal);
    }
  }
}

// Menampilkan judul dan cara pakai
void cetakPetunjuk() {
  Serial.println();
  Serial.println("=== Praktikum 5: Kalkulator Serial ===");
  Serial.println("Ketik soal lalu tekan Enter, contoh: 12 + 5");
  Serial.println("Operator: +  -  * (atau x)  / (atau :)");
  Serial.println("Pecahan memakai titik, contoh: 2.5");
  Serial.println();
}

// Memecah soal menjadi angka kiri, operator, dan angka kanan, lalu menghitung
void prosesSoal(String soal) {
  int posisi = cariOperator(soal);
  if (posisi < 0) {
    Serial.printf("%s -> [GALAT] operator tidak ditemukan\n", soal.c_str());
    return;                                        // keluar dari fungsi
  }

  // Potong teks: sebelum operator = angka kiri, sesudah operator = angka kanan
  String teksKiri = soal.substring(0, posisi);
  String teksKanan = soal.substring(posisi + 1);
  teksKiri.trim();
  teksKanan.trim();

  if (!apakahAngka(teksKiri) || !apakahAngka(teksKanan)) {
    Serial.printf("%s -> [GALAT] angka tidak valid\n", soal.c_str());
    return;
  }

  float angkaKiri = teksKiri.toFloat();            // teks "12" -> angka 12.0
  float angkaKanan = teksKanan.toFloat();
  char operasi = soal.charAt(posisi);
  float hasil = 0.0;

  switch (operasi) {
    case '+':
      hasil = angkaKiri + angkaKanan;
      break;
    case '-':
      hasil = angkaKiri - angkaKanan;
      break;
    case '*':
    case 'x':                                      // tiga simbol perkalian
    case 'X':                                      // berbagi satu blok kode
      hasil = angkaKiri * angkaKanan;
      break;
    case '/':
    case ':':
      // == aman di sini: teks "0" atau "0.0" selalu diubah menjadi 0.0 persis
      if (angkaKanan == 0.0) {
        Serial.printf("%s -> [GALAT] tidak bisa membagi dengan nol\n", soal.c_str());
        return;
      }
      hasil = angkaKiri / angkaKanan;
      break;
  }

  // c_str() mengubah String menjadi teks C (char[]) yang dipahami %s
  Serial.printf("%s = %.2f\n", soal.c_str(), hasil);
}

// Mencari posisi operator di dalam soal; -1 jika tidak ada.
// Pencarian dimulai dari indeks 1 supaya tanda minus pada angka pertama
// (misalnya "-3 x 4") tidak dianggap sebagai operator.
int cariOperator(String soal) {
  int panjang = soal.length();
  for (int i = 1; i < panjang; i++) {
    char c = soal.charAt(i);
    if (c == '+' || c == '-' || c == '*' || c == 'x' || c == 'X' || c == '/' || c == ':') {
      return i;
    }
  }
  return -1;
}

// true jika teks berupa angka yang sah, misalnya "12", "-3", atau "2.5"
bool apakahAngka(String teks) {
  int panjang = teks.length();
  int jumlahDigit = 0;
  int jumlahTitik = 0;
  for (int i = 0; i < panjang; i++) {
    char c = teks.charAt(i);
    if (c >= '0' && c <= '9') {
      jumlahDigit++;
    } else if (c == '.') {
      jumlahTitik++;
    } else if (c == '-' && i == 0) {
      // tanda minus boleh, asal di paling depan
    } else {
      return false;                                // karakter lain: bukan angka
    }
  }
  return jumlahDigit > 0 && jumlahTitik <= 1;
}
