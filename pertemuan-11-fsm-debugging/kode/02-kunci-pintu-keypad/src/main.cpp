/*
 * =====================================================================
 *  Pertemuan 11 · Praktikum 2 — Kunci Pintu dengan Keypad
 * =====================================================================
 *  Tujuan   : Membuat kunci pintu ber-PIN sebagai FSM: TERKUNCI →
 *             MEMASUKKAN_PIN → TERBUKA / SALAH → BLOKIR (setelah 3 kali
 *             salah, dengan hitung mundur). Semua tanpa delay().
 *  Rangkaian: Keypad membran 4×4 — baris R1–R4 ke GPIO 13, 14, 27, 26;
 *             kolom C1–C4 ke GPIO 25, 33, 32, 23. Servo (kunci): sinyal
 *             GPIO 19, V+ ke VIN (5 V), GND ke GND. LED bawaan GPIO 2 =
 *             indikator salah/blokir.
 *  Library  : Keypad (Chris--A) dan ESP32Servo — lihat platformio.ini.
 *  Cara menjalankan:
 *    - Board asli : Build (✓) → Upload (→) → Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino, salin diagram.json,
 *                   tambahkan library Keypad dan ESP32Servo, lalu klik ▶.
 *  Hasil yang diharapkan:
 *    PIN 1234 lalu # → servo berputar 90° (terbuka) selama 5 detik.
 *    Salah 3 kali → keypad diblokir 10 detik, LED berkedip, hitung mundur.
 * =====================================================================
 */
#include <Arduino.h>
#include <Keypad.h>
#include <ESP32Servo.h>

// ---- Pin ----
const int PIN_SERVO = 19;   // servo berperan sebagai "kunci" pintu
const int PIN_LED   = 2;    // LED bawaan board: tanda salah/blokir

// Keypad 4x4. Library Keypad memasang INPUT_PULLUP di pin BARIS dan memakai
// pin KOLOM sebagai OUTPUT bergantian. Jadi semua pin harus GPIO dua arah
// yang punya pull-up internal (bukan GPIO 34–39) dan sebaiknya bukan pin
// strapping (0, 2, 5, 12, 15) agar tombol yang tertekan tidak mengganggu boot.
const byte JUMLAH_BARIS = 4;
const byte JUMLAH_KOLOM = 4;
byte pinBaris[JUMLAH_BARIS] = {13, 14, 27, 26};   // R1, R2, R3, R4
byte pinKolom[JUMLAH_KOLOM] = {25, 33, 32, 23};   // C1, C2, C3, C4
char petaTombol[JUMLAH_BARIS][JUMLAH_KOLOM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// ---- Aturan kunci (konstanta bernama, bukan angka ajaib) ----
const char SANDI_BENAR[] = "1234";            // PIN contoh: ganti sesukamu
const byte PANJANG_SANDI_MAKS = 8;
const byte BATAS_SALAH = 3;                    // salah 3 kali → blokir
const unsigned long BATAS_DIAM_MS   = 10000;   // batal bila 10 s tanpa tombol
const unsigned long LAMA_TERBUKA_MS = 5000;    // terbuka 5 s lalu terkunci lagi
const unsigned long LAMA_SALAH_MS   = 1500;    // tanda salah tampil 1,5 s
const unsigned long LAMA_BLOKIR_MS  = 10000;   // blokir 10 s (sistem nyata: lebih lama)
const unsigned long KEDIP_BLOKIR_MS = 250;     // LED berkedip tiap 250 ms saat blokir
const int SUDUT_TERKUNCI = 0;
const int SUDUT_TERBUKA  = 90;

// ---- Keadaan FSM ----
enum class Keadaan { TERKUNCI, MEMASUKKAN_PIN, TERBUKA, SALAH, BLOKIR };

Keypad keypad = Keypad(makeKeymap(petaTombol), pinBaris, pinKolom, JUMLAH_BARIS, JUMLAH_KOLOM);
Servo servoKunci;

Keadaan keadaan = Keadaan::TERKUNCI;
unsigned long waktuMasuk = 0;            // millis() saat masuk keadaan sekarang
unsigned long waktuTombolTerakhir = 0;   // untuk batas diam di MEMASUKKAN_PIN
char sandi[PANJANG_SANDI_MAKS + 1];      // digit yang diketik (+1 untuk '\0')
byte panjangSandi = 0;
byte jumlahSalah = 0;
unsigned long sisaDetikTerakhir = 0;     // agar hitung mundur dicetak sekali per detik

// ---- Deklarasi fungsi ----
void masukKeadaan(Keadaan baru, const char *alasan);
void tambahDigit(char digit);
bool sandiCocok();
const char *namaKeadaan(Keadaan k);

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  servoKunci.attach(PIN_SERVO);
  Serial.println("[INFO] Kunci pintu siap. Ketik PIN lalu tekan #, tombol * untuk batal.");
  masukKeadaan(Keadaan::TERKUNCI, "mulai");
}

void loop() {
  unsigned long sekarang = millis();
  unsigned long lama = sekarang - waktuMasuk;   // lama berada di keadaan ini
  char tombol = keypad.getKey();                // tidak menunggu: NO_KEY bila kosong

  switch (keadaan) {
    case Keadaan::TERKUNCI:
      if (tombol >= '0' && tombol <= '9') {
        masukKeadaan(Keadaan::MEMASUKKAN_PIN, "tombol angka");
        tambahDigit(tombol);
      }
      break;

    case Keadaan::MEMASUKKAN_PIN:
      if (tombol >= '0' && tombol <= '9') {
        tambahDigit(tombol);
      } else if (tombol == '*') {
        masukKeadaan(Keadaan::TERKUNCI, "dibatalkan dengan *");
      } else if (tombol == '#') {
        if (sandiCocok()) {
          masukKeadaan(Keadaan::TERBUKA, "PIN benar");
        } else {
          masukKeadaan(Keadaan::SALAH, "PIN salah");
        }
      } else if (sekarang - waktuTombolTerakhir >= BATAS_DIAM_MS) {
        masukKeadaan(Keadaan::TERKUNCI, "10 s tanpa tombol");
      }
      break;

    case Keadaan::TERBUKA:
      if (lama >= LAMA_TERBUKA_MS) {
        masukKeadaan(Keadaan::TERKUNCI, "kunci otomatis");
      }
      break;

    case Keadaan::SALAH:
      if (lama >= LAMA_SALAH_MS) {
        if (jumlahSalah >= BATAS_SALAH) {
          masukKeadaan(Keadaan::BLOKIR, "3 kali salah");
        } else {
          masukKeadaan(Keadaan::TERKUNCI, "coba lagi");
        }
      }
      break;

    case Keadaan::BLOKIR: {
      // LED berkedip tanpa delay(): nyala-padam bergantian tiap 250 ms
      digitalWrite(PIN_LED, (lama / KEDIP_BLOKIR_MS) % 2 == 0 ? HIGH : LOW);
      if (lama >= LAMA_BLOKIR_MS) {
        jumlahSalah = 0;
        masukKeadaan(Keadaan::TERKUNCI, "blokir selesai");
        break;
      }
      // Hitung mundur: sisa detik dibulatkan ke atas, dicetak saat angkanya berubah
      unsigned long sisaDetik = (LAMA_BLOKIR_MS - lama + 999) / 1000;
      if (sisaDetik != sisaDetikTerakhir) {
        sisaDetikTerakhir = sisaDetik;
        Serial.printf("[BLOKIR] Coba lagi dalam %lu detik\n", sisaDetik);
      }
      if (tombol != NO_KEY) {
        Serial.println("[PERINGATAN] Keypad diblokir, tombol diabaikan.");
      }
      break;
    }
  }
}

// Aksi masuk-keadaan: dijalankan SEKALI setiap kali pindah keadaan.
void masukKeadaan(Keadaan baru, const char *alasan) {
  keadaan = baru;
  waktuMasuk = millis();
  Serial.printf("[FSM] -> %s (%s)\n", namaKeadaan(baru), alasan);

  switch (baru) {
    case Keadaan::TERKUNCI:
      servoKunci.write(SUDUT_TERKUNCI);
      digitalWrite(PIN_LED, LOW);
      panjangSandi = 0;                  // buang digit lama
      sandi[0] = '\0';
      Serial.println("[INFO] Pintu terkunci. Ketik PIN lalu tekan #.");
      break;
    case Keadaan::MEMASUKKAN_PIN:
      panjangSandi = 0;
      sandi[0] = '\0';
      break;
    case Keadaan::TERBUKA:
      servoKunci.write(SUDUT_TERBUKA);
      jumlahSalah = 0;                   // berhasil: hitungan salah kembali ke nol
      Serial.printf("[INFO] Pintu terbuka selama %lu detik.\n", LAMA_TERBUKA_MS / 1000);
      break;
    case Keadaan::SALAH:
      jumlahSalah++;
      digitalWrite(PIN_LED, HIGH);
      Serial.printf("[PERINGATAN] PIN salah (%d dari %d).\n", jumlahSalah, BATAS_SALAH);
      break;
    case Keadaan::BLOKIR:
      sisaDetikTerakhir = 0;             // paksa hitung mundur langsung tercetak
      Serial.println("[PERINGATAN] Terlalu banyak salah. Keypad diblokir.");
      break;
  }
}

void tambahDigit(char digit) {
  waktuTombolTerakhir = millis();
  if (panjangSandi >= PANJANG_SANDI_MAKS) {
    Serial.println("[PERINGATAN] PIN terlalu panjang, digit diabaikan.");
    return;
  }
  sandi[panjangSandi++] = digit;
  sandi[panjangSandi] = '\0';
  // Tampilkan tersamar: PIN asli tidak boleh tercetak ke Serial.
  Serial.print("[KEYPAD] PIN: ");
  for (byte i = 0; i < panjangSandi; i++) {
    Serial.print('*');
  }
  Serial.println();
}

bool sandiCocok() {
  return strcmp(sandi, SANDI_BENAR) == 0;
}

const char *namaKeadaan(Keadaan k) {
  switch (k) {
    case Keadaan::TERKUNCI:       return "TERKUNCI";
    case Keadaan::MEMASUKKAN_PIN: return "MEMASUKKAN_PIN";
    case Keadaan::TERBUKA:        return "TERBUKA";
    case Keadaan::SALAH:          return "SALAH";
    case Keadaan::BLOKIR:         return "BLOKIR";
  }
  return "?";
}
