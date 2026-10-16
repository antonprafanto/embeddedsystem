/*
 * =====================================================================
 *  Pertemuan 09 · Praktikum 1 — Perintah Teks lewat Serial (UART)
 * =====================================================================
 *  Tujuan   : Mengendalikan LED dengan mengetik perintah teks di Serial
 *             Monitor. Kamu belajar menerima data UART per baris, mengurai
 *             (parsing) teks menjadi perintah + argumen, memeriksa
 *             (validasi) argumen, lalu membalas dengan pesan yang ramah.
 *  Rangkaian: Tidak perlu komponen tambahan. LED bawaan board ESP32
 *             DevKit V1 terhubung ke GPIO 2. Serial (UART0: TX = GPIO 1,
 *             RX = GPIO 3) tersambung ke laptop lewat cip USB-UART di board.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor (ikon colokan).
 *                   Ketik perintah, lalu tekan Enter.
 *    - Wokwi web  : salin isi berkas ini ke sketch.ino, klik ▶, klik panel
 *                   Serial Monitor, ketik perintah, lalu tekan Enter.
 *  Perintah : BANTUAN, LED ON, LED OFF, KEDIP n (n = 1–10), STATUS
 *             (huruf besar/kecil sama saja)
 *  Hasil yang diharapkan (Serial Monitor, 115200 baud), contoh:
 *    > LED ON
 *    [OK] LED menyala.
 *    > KEDIP 20
 *    [GALAT] Jumlah kedip harus 1 sampai 10. Contoh: KEDIP 3
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_LED = 2;                       // LED bawaan board ESP32 DevKit V1
const int KEDIP_MIN = 1;                     // batas bawah argumen KEDIP
const int KEDIP_MAKS = 10;                   // batas atas: agar program tidak sibuk lama
const int JEDA_KEDIP_MS = 200;               // lama satu fase nyala/padam saat berkedip
const unsigned long BATAS_TUNGGU_MS = 5000;  // jeda maksimum antarhuruf saat mengetik

bool ledMenyala = false;                     // keadaan LED saat ini
unsigned long jumlahBerhasil = 0;            // banyaknya perintah yang berhasil
unsigned long jumlahGalat = 0;               // banyaknya perintah yang ditolak

// Deklarasi fungsi (prototipe) supaya urutan definisi di bawah bebas
void jalankanPerintah(const String &baris);
bool semuaAngka(const String &teks);
void kirimGalat(const String &pesan);
void aturLed(bool nyala);
void kedipkan(int kali);
void tampilkanBantuan();
void tampilkanStatus();

void setup() {
  Serial.begin(115200);                      // UART0 → cip USB-UART → laptop
  // Serial Monitor PlatformIO mengirim tiap huruf begitu diketik. Batas tunggu
  // yang longgar membuat readStringUntil() tidak memotong ketikan yang lambat.
  Serial.setTimeout(BATAS_TUNGGU_MS);
  pinMode(PIN_LED, OUTPUT);
  aturLed(false);
  delay(500);                                // beri waktu Serial Monitor siap

  Serial.println();
  Serial.println("=========================================");
  Serial.println("  Pengendali LED lewat Serial (UART)");
  Serial.println("=========================================");
  Serial.println("Ketik BANTUAN lalu tekan Enter.");
}

void loop() {
  if (Serial.available() > 0) {                    // ada byte masuk dari laptop?
    String baris = Serial.readStringUntil('\n');   // kumpulkan sampai Enter ('\n')
    baris.trim();                            // buang spasi dan '\r' di awal/akhir
    if (baris.length() == 0) {
      return;                                // baris kosong: abaikan
    }
    baris.toUpperCase();                     // "led on" dianggap sama dengan "LED ON"
    Serial.print("> ");                      // gemakan perintah yang diterima ESP32
    Serial.println(baris);
    jalankanPerintah(baris);
  }
}

// Mengurai satu baris: kata pertama = perintah, sisanya = argumen.
// Contoh: "KEDIP 3" → perintah = "KEDIP", argumen = "3".
void jalankanPerintah(const String &baris) {
  String perintah = baris;
  String argumen = "";
  int posisiSpasi = baris.indexOf(' ');            // -1 jika tidak ada spasi
  if (posisiSpasi >= 0) {
    perintah = baris.substring(0, posisiSpasi);    // sebelum spasi
    argumen = baris.substring(posisiSpasi + 1);    // sesudah spasi
    argumen.trim();
  }

  if (perintah == "LED" && argumen == "ON") {
    jumlahBerhasil++;
    aturLed(true);
    Serial.println("[OK] LED menyala.");
  } else if (perintah == "LED" && argumen == "OFF") {
    jumlahBerhasil++;
    aturLed(false);
    Serial.println("[OK] LED padam.");
  } else if (perintah == "LED") {
    kirimGalat("Pilihan untuk LED hanya ON atau OFF. Contoh: LED ON");
  } else if (perintah == "KEDIP") {
    // Validasi dua lapis: (1) harus angka, (2) harus berada dalam rentang wajar.
    if (!semuaAngka(argumen)) {
      kirimGalat("KEDIP butuh angka bulat. Contoh: KEDIP 3");
      return;
    }
    int kali = argumen.toInt();
    if (kali < KEDIP_MIN || kali > KEDIP_MAKS) {
      kirimGalat("Jumlah kedip harus " + String(KEDIP_MIN) + " sampai " +
                 String(KEDIP_MAKS) + ". Contoh: KEDIP 3");
      return;
    }
    jumlahBerhasil++;
    kedipkan(kali);
    Serial.printf("[OK] LED berkedip %d kali.\n", kali);
  } else if (perintah == "STATUS" && argumen.length() == 0) {
    jumlahBerhasil++;
    tampilkanStatus();
  } else if (perintah == "BANTUAN" && argumen.length() == 0) {
    jumlahBerhasil++;
    tampilkanBantuan();
  } else {
    kirimGalat("Perintah \"" + baris + "\" tidak dikenal. Ketik BANTUAN.");
  }
}

// true bila teks tidak kosong dan semua karakternya angka 0–9.
// Perlu, karena toInt() diam-diam mengubah teks bukan angka ("TIGA") menjadi 0.
bool semuaAngka(const String &teks) {
  if (teks.length() == 0) {
    return false;
  }
  for (unsigned int i = 0; i < teks.length(); i++) {
    if (!isDigit(teks[i])) {
      return false;
    }
  }
  return true;
}

// Semua pesan galat lewat satu pintu: formatnya seragam dan mudah dihitung.
void kirimGalat(const String &pesan) {
  jumlahGalat++;
  Serial.print("[GALAT] ");
  Serial.println(pesan);
}

void aturLed(bool nyala) {
  ledMenyala = nyala;
  digitalWrite(PIN_LED, nyala ? HIGH : LOW);
}

// Berkedip lalu kembali ke keadaan semula (nyala tetap nyala, padam tetap padam).
void kedipkan(int kali) {
  for (int i = 0; i < kali; i++) {
    digitalWrite(PIN_LED, ledMenyala ? LOW : HIGH);   // kebalikan keadaan sekarang
    delay(JEDA_KEDIP_MS);
    digitalWrite(PIN_LED, ledMenyala ? HIGH : LOW);   // kembali seperti semula
    delay(JEDA_KEDIP_MS);
  }
}

void tampilkanBantuan() {
  Serial.println("Daftar perintah:");
  Serial.println("  LED ON   - menyalakan LED bawaan (GPIO 2)");
  Serial.println("  LED OFF  - memadamkan LED");
  Serial.println("  KEDIP n  - mengedipkan LED n kali (n = 1 sampai 10)");
  Serial.println("  STATUS   - menampilkan keadaan LED dan jumlah perintah");
  Serial.println("  BANTUAN  - menampilkan daftar ini");
}

void tampilkanStatus() {
  Serial.printf("[STATUS] LED: %s | berhasil: %lu | galat: %lu\n",
                ledMenyala ? "NYALA" : "PADAM", jumlahBerhasil, jumlahGalat);
}
