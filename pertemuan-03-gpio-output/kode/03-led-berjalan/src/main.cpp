/*
 * =====================================================================
 *  Pertemuan 03 · Praktikum 3 — LED Berjalan (Larik + Perulangan)
 * =====================================================================
 *  Tujuan   : Mengendalikan 6 LED memakai larik (array) dan perulangan
 *             for, lalu membuat empat pola: maju, maju-mundur ala
 *             "Knight Rider", mengisi-mengosongkan, dan genap-ganjil.
 *  Rangkaian: 6 LED di sebelah kiri board, masing-masing:
 *             GPIO → resistor 220 Ω → anoda LED; katode → GND (sisi kiri).
 *             Urutan pin dari atas ke bawah: 32, 33, 25, 26, 27, 14.
 *  Cara menjalankan:
 *    - Board asli : rangkai dulu, lalu Build (✓) → Upload (→) → Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan isi diagram.json, klik ▶.
 *  Hasil yang diharapkan:
 *    LED menyala bergantian membentuk empat pola, berulang terus.
 *    Serial Monitor mencetak nama pola dan keadaan LED ('#' = menyala).
 * =====================================================================
 */
#include <Arduino.h>

// Urutan pin = urutan fisik LED (atas → bawah di sisi kiri board)
const int PIN_LED[] = {32, 33, 25, 26, 27, 14};
const int JUMLAH_LED = sizeof(PIN_LED) / sizeof(PIN_LED[0]);  // = 6
const int JEDA_MS = 150;           // jeda antarlangkah; kecil = cepat

bool nyala[JUMLAH_LED];            // catatan keadaan tiap LED

// Deklarasi fungsi pembantu (isinya ada di bawah loop)
void aturLed(int indeks, bool hidup);
void matikanSemua();
void nyalakanSatu(int indeks);
void tampilkanLaluTunggu(int jedaMs);
void polaMaju(int ulangan);
void polaMajuMundur(int ulangan);
void polaMengisi(int ulangan);
void polaGenapGanjil(int ulangan);

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < JUMLAH_LED; i++) {
    pinMode(PIN_LED[i], OUTPUT);   // satu baris ini mengatur keenam pin
  }
  matikanSemua();
  Serial.print("[INFO] Praktikum 3: LED berjalan, jumlah LED = ");
  Serial.println(JUMLAH_LED);
}

void loop() {
  polaMaju(2);          // titik cahaya maju, 2 kali
  polaMajuMundur(2);    // ala "Knight Rider", 2 kali
  polaMengisi(1);       // nyalakan satu per satu, lalu padamkan
  polaGenapGanjil(3);   // indeks genap dan ganjil bergantian
}

// ---------- Fungsi pembantu ----------

// Nyalakan/padamkan satu LED dan catat keadaannya
void aturLed(int indeks, bool hidup) {
  digitalWrite(PIN_LED[indeks], hidup ? HIGH : LOW);
  nyala[indeks] = hidup;
}

void matikanSemua() {
  for (int i = 0; i < JUMLAH_LED; i++) {
    aturLed(i, false);
  }
}

// Hanya LED nomor 'indeks' yang menyala, sisanya padam
void nyalakanSatu(int indeks) {
  for (int i = 0; i < JUMLAH_LED; i++) {
    aturLed(i, i == indeks);       // true hanya untuk i yang sama dengan indeks
  }
}

// Cetak keadaan semua LED (misalnya "[LED] ..#..."), lalu tunggu
void tampilkanLaluTunggu(int jedaMs) {
  Serial.print("[LED] ");
  for (int i = 0; i < JUMLAH_LED; i++) {
    Serial.print(nyala[i] ? '#' : '.');
  }
  Serial.println();
  delay(jedaMs);
}

// ---------- Pola-pola ----------

// Pola 1: satu titik cahaya berjalan dari LED pertama ke LED terakhir
void polaMaju(int ulangan) {
  Serial.println("[POLA] Maju");
  for (int u = 0; u < ulangan; u++) {
    for (int i = 0; i < JUMLAH_LED; i++) {
      nyalakanSatu(i);
      tampilkanLaluTunggu(JEDA_MS);
    }
  }
}

// Pola 2: maju lalu mundur, seperti lampu mobil KITT di serial "Knight Rider"
void polaMajuMundur(int ulangan) {
  Serial.println("[POLA] Maju-mundur (Knight Rider)");
  for (int u = 0; u < ulangan; u++) {
    for (int i = 0; i < JUMLAH_LED - 1; i++) {   // maju: 0, 1, 2, 3, 4
      nyalakanSatu(i);
      tampilkanLaluTunggu(JEDA_MS);
    }
    for (int i = JUMLAH_LED - 1; i > 0; i--) {   // mundur: 5, 4, 3, 2, 1
      nyalakanSatu(i);
      tampilkanLaluTunggu(JEDA_MS);
    }
  }
}

// Pola 3: LED menyala satu per satu sampai penuh, lalu padam dari ujung
void polaMengisi(int ulangan) {
  Serial.println("[POLA] Mengisi lalu mengosongkan");
  for (int u = 0; u < ulangan; u++) {
    for (int i = 0; i < JUMLAH_LED; i++) {        // isi: 0 → 5
      aturLed(i, true);
      tampilkanLaluTunggu(JEDA_MS);
    }
    for (int i = JUMLAH_LED - 1; i >= 0; i--) {   // kosongkan: 5 → 0
      aturLed(i, false);
      tampilkanLaluTunggu(JEDA_MS);
    }
  }
}

// Pola 4: LED berindeks genap (0, 2, 4) dan ganjil (1, 3, 5) bergantian
void polaGenapGanjil(int ulangan) {
  Serial.println("[POLA] Genap-ganjil bergantian");
  for (int u = 0; u < ulangan; u++) {
    for (int i = 0; i < JUMLAH_LED; i++) {
      aturLed(i, i % 2 == 0);      // menyala jika indeks genap
    }
    tampilkanLaluTunggu(JEDA_MS * 2);
    for (int i = 0; i < JUMLAH_LED; i++) {
      aturLed(i, i % 2 == 1);      // menyala jika indeks ganjil
    }
    tampilkanLaluTunggu(JEDA_MS * 2);
  }
  matikanSemua();
}
