/*
 * =====================================================================
 *  Pertemuan 13 · Praktikum 1 — Memindai dan Menyambung ke Wi-Fi
 * =====================================================================
 *  Tujuan   : Memindai jaringan Wi-Fi di sekitar (SSID, RSSI, kanal, jenis
 *             keamanan), lalu menyambung ke satu jaringan dan menampilkan
 *             alamat IP yang diberikan oleh DHCP.
 *  Rangkaian: Tidak perlu komponen tambahan; antena sudah ada di modul.
 *  Cara menjalankan:
 *    - Wokwi web  : salin berkas ini ke sketch.ino (+ diagram.json), klik ▶.
 *                   Program otomatis memakai jaringan simulasi Wokwi-GUEST.
 *    - Board asli : salin include/rahasia.example.h menjadi include/rahasia.h,
 *                   isi SSID & sandi hotspot 2,4 GHz-mu, lalu Build → Upload.
 *  Hasil yang diharapkan:
 *    Tabel jaringan (SSID, RSSI, kanal, keamanan), lalu "Tersambung!",
 *    alamat IP, dan kekuatan sinyal yang diperbarui setiap 5 detik.
 *    Angka RSSI, IP, dan MAC di tempatmu bisa berbeda.
 * =====================================================================
 */
#include <Arduino.h>
#include <WiFi.h>

// ---------------------------------------------------------------------
// Pengaturan Wi-Fi
//  - Board asli: salin include/rahasia.example.h menjadi include/rahasia.h,
//    lalu isi SSID & sandi hotspot 2,4 GHz-mu (rahasia.h tidak di-commit).
//  - Wokwi: rahasia.h tidak ada, jadi otomatis memakai Wokwi-GUEST.
// ---------------------------------------------------------------------
#if __has_include("rahasia.h")
#include "rahasia.h"
#define SUMBER_WIFI "include/rahasia.h"
#else
#define WIFI_SSID "Wokwi-GUEST"  // Wi-Fi virtual Wokwi: terbuka, tanpa sandi
#define WIFI_SANDI ""
#define WIFI_KANAL 6             // Wokwi-GUEST ada di kanal 6 (sambung lebih cepat)
#define SUMBER_WIFI "bawaan Wokwi (rahasia.h tidak ditemukan)"
#endif
#ifndef WIFI_KANAL
#define WIFI_KANAL 0             // 0 = cari jaringan di semua kanal
#endif

const unsigned long BATAS_TUNGGU_MS = 20000;  // menyerah setelah 20 detik
const unsigned long INTERVAL_INFO_MS = 5000;  // laporan sinyal tiap 5 detik

unsigned long waktuInfoTerakhir = 0;

// Deklarasi fungsi (ditulis lengkap di bawah loop)
void pindaiJaringan();
bool sambungWiFi();
const char *kategoriSinyal(long rssi);
const char *namaKeamanan(wifi_auth_mode_t jenis);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== Praktikum 1: Pindai & Sambung Wi-Fi ===");

  WiFi.mode(WIFI_STA);  // mode stasiun (STA): ESP32 menjadi "tamu" sebuah AP
  WiFi.disconnect();    // mulai dari keadaan bersih
  delay(100);

  pindaiJaringan();
  sambungWiFi();
}

void loop() {
  // Laporkan kondisi sambungan setiap 5 detik tanpa delay() panjang
  if (millis() - waktuInfoTerakhir >= INTERVAL_INFO_MS) {
    waktuInfoTerakhir = millis();
    if (WiFi.status() == WL_CONNECTED) {
      long rssi = WiFi.RSSI();  // kekuatan sinyal AP yang sedang dipakai
      Serial.printf("[DATA] RSSI: %ld dBm (%s)\n", rssi, kategoriSinyal(rssi));
    } else {
      // Fitur auto-reconnect ESP32 (aktif secara bawaan) terus mencoba
      Serial.printf("[PERINGATAN] Tidak tersambung (status %d)\n", (int)WiFi.status());
    }
  }
}

void pindaiJaringan() {
  Serial.println("[INFO] Memindai jaringan Wi-Fi di sekitar...");
  int jumlah = WiFi.scanNetworks();  // menunggu sampai selesai (beberapa detik)
  if (jumlah < 0) {
    Serial.println("[PERINGATAN] Pemindaian gagal.");
    return;
  }
  Serial.printf("[INFO] Ditemukan %d jaringan:\n", jumlah);
  if (jumlah == 0) return;

  Serial.println(" No | SSID                 | RSSI | Sinyal       | Kanal | Keamanan");
  for (int i = 0; i < jumlah; i++) {
    long rssi = WiFi.RSSI(i);
    Serial.printf("%3d | %-20.20s | %4ld | %-12s | %5ld | %s\n", i + 1, WiFi.SSID(i).c_str(), rssi,
                  kategoriSinyal(rssi), (long)WiFi.channel(i), namaKeamanan(WiFi.encryptionType(i)));
  }
  WiFi.scanDelete();  // bebaskan memori hasil pemindaian
}

bool sambungWiFi() {
  Serial.printf("[INFO] Pengaturan Wi-Fi: %s\n", SUMBER_WIFI);
  Serial.printf("[INFO] Menyambung ke \"%s\"", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_SANDI, WIFI_KANAL);

  unsigned long mulai = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - mulai > BATAS_TUNGGU_MS) {  // jangan menunggu selamanya
      Serial.println();
      Serial.printf("[PERINGATAN] Gagal tersambung (status %d). Cek SSID, sandi, dan pita 2,4 GHz.\n",
                    (int)WiFi.status());
      return false;
    }
    Serial.print(".");
    delay(500);
  }
  Serial.println(" Tersambung!");
  Serial.print("[INFO] Alamat IP  : ");
  Serial.println(WiFi.localIP());  // diberikan server DHCP di router/hotspot
  Serial.printf("[INFO] Kanal      : %ld\n", (long)WiFi.channel());
  Serial.printf("[INFO] Alamat MAC : %s\n", WiFi.macAddress().c_str());
  return true;
}

// Patokan kasar kekuatan sinyal (bukan standar resmi)
const char *kategoriSinyal(long rssi) {
  if (rssi >= -50) return "sangat kuat";
  if (rssi >= -60) return "kuat";
  if (rssi >= -70) return "cukup";
  if (rssi >= -80) return "lemah";
  return "sangat lemah";
}

const char *namaKeamanan(wifi_auth_mode_t jenis) {
  switch (jenis) {
    case WIFI_AUTH_OPEN:            return "terbuka (tanpa sandi)";
    case WIFI_AUTH_WEP:             return "WEP (usang, tidak aman)";
    case WIFI_AUTH_WPA_PSK:         return "WPA";
    case WIFI_AUTH_WPA2_PSK:        return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-Enterprise";
    case WIFI_AUTH_WPA3_PSK:        return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:   return "WPA2/WPA3";
    default:                        return "lainnya";
  }
}
