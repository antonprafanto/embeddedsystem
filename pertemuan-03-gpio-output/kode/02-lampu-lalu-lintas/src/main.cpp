/*
 * =====================================================================
 *  Pertemuan 03 · Praktikum 2 — Lampu Lalu Lintas
 * =====================================================================
 *  Tujuan   : Mengendalikan tiga LED dengan urutan dan durasi seperti
 *             lampu lalu lintas: MERAH → HIJAU → KUNING → MERAH → ...
 *  Rangkaian: Tiap LED: GPIO → resistor 220 Ω → anoda LED; katode → GND
 *             (pin GND di sisi kiri board).
 *               - Merah  : GPIO 25
 *               - Kuning : GPIO 26
 *               - Hijau  : GPIO 27
 *  Cara menjalankan:
 *    - Board asli : rangkai dulu, lalu Build (✓) → Upload (→) → Serial Monitor.
 *    - Wokwi web  : salin berkas ini ke sketch.ino dan isi diagram.json, klik ▶.
 *  Hasil yang diharapkan:
 *    Merah 6 detik → hijau 5 detik → kuning 2 detik, lalu berulang
 *    (13 detik per siklus). Serial Monitor mencetak nama fase dan durasinya.
 *  Catatan: program ini sengaja memakai delay() agar mudah dipahami.
 *    Di Pertemuan 5 kamu belajar cara menunggu tanpa delay().
 * =====================================================================
 */
#include <Arduino.h>

const int PIN_MERAH = 25;
const int PIN_KUNING = 26;
const int PIN_HIJAU = 27;

// Durasi tiap fase dalam milidetik. Di jalan raya, merah dan hijau bisa
// puluhan detik; di sini dipersingkat agar cepat diamati. Kuning tetap
// paling singkat karena hanya peringatan sebelum merah.
const unsigned long DURASI_MERAH_MS = 6000;
const unsigned long DURASI_HIJAU_MS = 5000;
const unsigned long DURASI_KUNING_MS = 2000;

unsigned long nomorSiklus = 0;

// Deklarasi fungsi pembantu (isinya ada di bawah loop)
void aturLampu(bool merah, bool kuning, bool hijau);
void jalankanFase(const char *namaFase, bool merah, bool kuning, bool hijau,
                  unsigned long durasiMs);

void setup() {
  Serial.begin(115200);
  pinMode(PIN_MERAH, OUTPUT);
  pinMode(PIN_KUNING, OUTPUT);
  pinMode(PIN_HIJAU, OUTPUT);
  aturLampu(false, false, false);  // semua padam dulu
  Serial.println("[INFO] Praktikum 2: lampu lalu lintas");
  Serial.println("[INFO] Urutan: MERAH -> HIJAU -> KUNING -> MERAH ...");
}

void loop() {
  nomorSiklus++;
  Serial.print("--- Siklus ");
  Serial.print(nomorSiklus);
  Serial.println(" ---");

  //           nama fase                    merah  kuning hijau  durasi
  jalankanFase("MERAH  : berhenti",         true,  false, false, DURASI_MERAH_MS);
  jalankanFase("HIJAU  : jalan",            false, false, true,  DURASI_HIJAU_MS);
  jalankanFase("KUNING : bersiap berhenti", false, true,  false, DURASI_KUNING_MS);
}

// Menyalakan/memadamkan ketiga lampu sekaligus.
// (kondisi ? HIGH : LOW) artinya: jika kondisi benar pilih HIGH, jika salah LOW.
void aturLampu(bool merah, bool kuning, bool hijau) {
  digitalWrite(PIN_MERAH, merah ? HIGH : LOW);
  digitalWrite(PIN_KUNING, kuning ? HIGH : LOW);
  digitalWrite(PIN_HIJAU, hijau ? HIGH : LOW);
}

// Satu fase = atur lampu, cetak keterangan, lalu tunggu selama durasinya.
void jalankanFase(const char *namaFase, bool merah, bool kuning, bool hijau,
                  unsigned long durasiMs) {
  aturLampu(merah, kuning, hijau);
  Serial.print("[LAMPU] ");
  Serial.print(namaFase);
  Serial.print(" (");
  Serial.print(durasiMs / 1000);   // ubah milidetik menjadi detik
  Serial.println(" detik)");
  delay(durasiMs);
}
