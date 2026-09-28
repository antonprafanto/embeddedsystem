/**
 * ============================================================================
 * MODUL PRAKTIKUM MINGGU 04: MEMORI PERSISTEN (NVS & LITTLEFS) & DATASHEET
 * ============================================================================
 * Program Studi : Sarjana (S1) Teknik Elektro
 * Mata Kuliah   : Sistem Tertanam (Embedded Systems)
 * Platform      : ESP32 (Xtensa Dual-Core 32-bit)
 * Framework     : Arduino Core via PlatformIO
 * ============================================================================
 * TUJUAN PRAKTIKUM:
 * 1. Memahami perbedaan memori volatil (RAM) vs non-volatil (Flash/NVS).
 * 2. Mengimplementasikan pustaka Preferences (NVS Key-Value) untuk menyimpan
 *    parameter sistem (boot counter, sensor threshold) yang tahan mati listrik.
 * 3. Mengimplementasikan sistem berkas LittleFS untuk penyimpanan file statis.
 * 4. Menyediakan antarmuka interaktif Serial CLI untuk pengujian langsung oleh
 *    mahasiswa melalui Serial Monitor komputer.
 * 5. Menghubungkan teori decoding register biner dari datasheet sensor industri.
 * ============================================================================
 */

#include <Arduino.h>
#include <Preferences.h>
#include <FS.h>
#include <LittleFS.h>

// Inisialisasi Objek Preferences untuk NVS
Preferences preferences;

// Nama Namespace NVS (Maksimal 15 karakter)
const char* NVS_NAMESPACE = "lab_storage";

// Variabel Global Runtime (di RAM - volatil)
uint32_t currentBootCount = 0;
float currentThreshold = 30.0f;
String deviceOwner = "";

// Prototipe Fungsi
void initNVS();
void initLittleFS();
void printSystemSummary();
void processSerialCLI();
void executeCommand(String cmd);
void readFileContent(const char* path);
void appendLogEntry(const String& entry);
void demonstrateRegisterDecoding();

void setup() {
    // 1. Inisialisasi Komunikasi Serial UART
    Serial.begin(115200);
    delay(1000); // Waktu stabilisasi chip USB-to-UART

    Serial.println();
    Serial.println("===============================================================");
    Serial.println("   PRAKTIKUM MINGGU 4: MEMORI PERSISTEN (NVS & LITTLEFS)");
    Serial.println("   Laboratorium Sistem Tertanam - S1 Teknik Elektro");
    Serial.println("===============================================================");

    // 2. Inisialisasi Non-Volatile Storage (NVS) via Preferences
    initNVS();

    // 3. Inisialisasi File System LittleFS
    initLittleFS();

    // 4. Catat Riwayat Booting ke Berkas Log di LittleFS
    String bootLog = "System boot #" + String(currentBootCount) + " | UpTime: " + String(millis()) + " ms";
    appendLogEntry(bootLog);

    // 5. Cetak Ringkasan Sistem & Panduan CLI
    printSystemSummary();

    // 6. Demonstrasi Pemecahan Register Datasheet (Teori ke Kode C)
    demonstrateRegisterDecoding();

    Serial.println("Ketik 'HELP' di Serial Monitor untuk melihat daftar perintah interaktif.");
    Serial.print("ESP32-CLI> ");
}

void loop() {
    // Memproses perintah masukan dari mahasiswa via Serial Monitor secara non-blocking
    processSerialCLI();
}

/**
 * Inisialisasi dan Pembacaan Data dari NVS (Non-Volatile Storage)
 */
void initNVS() {
    Serial.println("[NVS] Membuka partisi NVS pada namespace: '" + String(NVS_NAMESPACE) + "'...");

    // Buka namespace NVS dalam mode Read/Write (false = R/W, true = Read Only)
    if (!preferences.begin(NVS_NAMESPACE, false)) {
        Serial.println("[NVS ERROR] Gagal menginisialisasi NVS! Periksa tabel partisi.");
        return;
    }

    // 1. Baca Boot Counter dari NVS (Gunakan nilai default 0 jika key belum ada)
    currentBootCount = preferences.getUInt("boot_count", 0);
    currentBootCount++; // Tambah 1 setiap kali ESP32 dinyalakan/reset

    // Simpan kembali nilai baru ke NVS
    preferences.putUInt("boot_count", currentBootCount);

    // 2. Baca Nilai Ambang Batas Sensor (Threshold)
    currentThreshold = preferences.getFloat("threshold", 32.5f);

    // 3. Baca Identitas Kepemilikan Perangkat
    deviceOwner = preferences.getString("owner", "Mahasiswa Teknik Elektro");

    // Tutup sesi preferences untuk melepaskan resource
    preferences.end();

    Serial.println("[NVS OK] Data persisten berhasil dimuat:");
    Serial.println("  • Total Boot Count : " + String(currentBootCount) + " kali (Tersimpan di Flash)");
    Serial.println("  • Nilai Threshold  : " + String(currentThreshold, 2) + " °C");
    Serial.println("  • Pemilik Board    : " + deviceOwner);
}

/**
 * Inisialisasi dan Pemasangan (Mounting) Sistem Berkas LittleFS
 */
void initLittleFS() {
    Serial.println("\n[LittleFS] Memasang sistem berkas flash memory...");

    // LittleFS.begin(true) -> Parameter true: otomatis format jika partisi belum valid
    if (!LittleFS.begin(true)) {
        Serial.println("[LittleFS ERROR] Pemasangan LittleFS gagal! Periksa setting partition table.");
        return;
    }

    size_t totalBytes = LittleFS.totalBytes();
    size_t usedBytes = LittleFS.usedBytes();

    Serial.println("[LittleFS OK] Sistem berkas berhasil dipasang:");
    Serial.println("  • Total Kapasitas : " + String(totalBytes / 1024) + " KB");
    Serial.println("  • Ruang Terpakai  : " + String(usedBytes / 1024) + " KB");
    Serial.println("  • Ruang Bebas     : " + String((totalBytes - usedBytes) / 1024) + " KB");

    // Uji coba membaca berkas konfigurasi jika ada
    if (LittleFS.exists("/config.json")) {
        Serial.println("[LittleFS] Ditemukan berkas konfigurasi statis '/config.json'.");
    } else {
        Serial.println("[LittleFS INFO] '/config.json' belum diunggah via LittleFS Data Upload.");
        Serial.println("                Membuat berkas default mandiri di dalam flash...");
        File defFile = LittleFS.open("/config.json", FILE_WRITE);
        if (defFile) {
            defFile.println("{\"device\":\"ESP32-LAB-04\",\"default_threshold\":32.5,\"mode\":\"industrial\"}");
            defFile.close();
            Serial.println("[LittleFS OK] Berkas default '/config.json' berhasil dibuat.");
        }
    }
}

/**
 * Mencetak Ringkasan Status Sistem ke Serial Monitor
 */
void printSystemSummary() {
    Serial.println("\n---------------------------------------------------------------");
    Serial.println("                 STATUS SISTEM PERSISTEN ESP32                 ");
    Serial.println("---------------------------------------------------------------");
    Serial.println(" [1] Boot Counter NVS   : " + String(currentBootCount) + " kali start");
    Serial.println(" [2] Sensor Threshold   : " + String(currentThreshold, 2) + " °C");
    Serial.println(" [3] Nama Device Owner  : " + deviceOwner);
    Serial.println(" [4] Free Flash Heap    : " + String(ESP.getFreeHeap()) + " bytes (RAM)");
    Serial.println(" [5] Chip Model & Rev   : " + String(ESP.getChipModel()) + " rev " + String(ESP.getChipRevision()));
    Serial.println("---------------------------------------------------------------");
}

/**
 * Menambahkan Catatan Riwayat ke Berkas Log LittleFS
 */
void appendLogEntry(const String& entry) {
    File logFile = LittleFS.open("/boot_log.txt", FILE_APPEND);
    if (!logFile) {
        Serial.println("[LOG ERROR] Gagal membuka /boot_log.txt untuk penulisan append.");
        return;
    }
    logFile.println(entry);
    logFile.close();
}

/**
 * Membaca dan Menampilkan Isi Berkas dari LittleFS ke Serial
 */
void readFileContent(const char* path) {
    Serial.println("\n[FILE READ] Membuka berkas: " + String(path));
    if (!LittleFS.exists(path)) {
        Serial.println("[FILE ERROR] Berkas '" + String(path) + "' tidak ditemukan!");
        return;
    }

    File f = LittleFS.open(path, FILE_READ);
    if (!f) {
        Serial.println("[FILE ERROR] Gagal membuka berkas untuk pembacaan!");
        return;
    }

    Serial.println("--- ISI BERKAS (" + String(f.size()) + " bytes) ---");
    while (f.available()) {
        Serial.write(f.read());
    }
    Serial.println("\n--- AKHIR DARI BERKAS ---");
    f.close();
}

/**
 * Demonstrasi Literasi Datasheet: Rekonstruksi Register Biner Sensor Industri
 * Contoh Kasus: Membaca register data suhu 20-bit dari Bosch BME280 (Register 0xFA, 0xFB, 0xFC)
 */
void demonstrateRegisterDecoding() {
    Serial.println("\n---------------------------------------------------------------");
    Serial.println(" 📖 LITERASI ENJINIRING: BACA REGISTER DARI DATASHEET BOSCH BME280");
    Serial.println("---------------------------------------------------------------");
    Serial.println(" Skenario: Sensor mengirim 3 byte register data suhu mentah:");
    Serial.println("  • Reg 0xFA (MSB)  = 0x82 (130 desimal, bit [19:12])");
    Serial.println("  • Reg 0xFB (LSB)  = 0x4B (75 desimal,  bit [11:4])");
    Serial.println("  • Reg 0xFC (XLSB) = 0xC0 (192 desimal, bit [7:4] digunakan)");
    
    // Simulasi byte yang diterima dari bus I2C/SPI
    uint8_t msb  = 0x82;
    uint8_t lsb  = 0x4B;
    uint8_t xlsb = 0xC0;

    // Rumus Datasheet Bosch Sensortec BME280:
    // raw_temp = (msb << 12) | (lsb << 4) | (xlsb >> 4)
    int32_t raw_temp = ((int32_t)msb << 12) | ((int32_t)lsb << 4) | ((int32_t)xlsb >> 4);

    Serial.println(" Langkah Eksekusi Bitwise C:");
    Serial.println("  1. (msb << 12)  = 0x" + String((int32_t)msb << 12, HEX));
    Serial.println("  2. (lsb << 4)   = 0x" + String((int32_t)lsb << 4, HEX));
    Serial.println("  3. (xlsb >> 4)  = 0x" + String((int32_t)xlsb >> 4, HEX));
    Serial.println("  -> Hasil ADC 20-bit : " + String(raw_temp) + " (0x" + String(raw_temp, HEX) + ")");
    Serial.println(" Teori ini membuktikan mengapa manipulasi bitwise (Minggu 1) mutlak");
    Serial.println(" diperlukan untuk mengubah tabel register datasheet menjadi nilai fisika nyata!");
    Serial.println("---------------------------------------------------------------\n");
}

/**
 * Pemroses Masukan CLI Serial Interaktif (Mahasiswa Friendly)
 */
void processSerialCLI() {
    static String inputBuffer = "";

    while (Serial.available() > 0) {
        char inChar = (char)Serial.read();

        // Tampilkan karakter kembali (echo) ke serial terminal
        Serial.print(inChar);

        if (inChar == '\n' || inChar == '\r') {
            inputBuffer.trim();
            if (inputBuffer.length() > 0) {
                Serial.println(); // Baris baru
                executeCommand(inputBuffer);
                inputBuffer = "";
                Serial.print("\nESP32-CLI> ");
            }
        } else {
            inputBuffer += inChar;
        }
    }
}

/**
 * Eksekutor Perintah CLI
 */
void executeCommand(String cmd) {
    cmd.trim();
    String upperCmd = cmd;
    upperCmd.toUpperCase();

    if (upperCmd == "HELP") {
        Serial.println("=== DAFTAR PERINTAH INTERAKTIF CLI (MINGGU 4) ===");
        Serial.println(" 1. STATUS         : Menampilkan ringkasan data NVS dan status memori.");
        Serial.println(" 2. SET <nilai>    : Mengubah nilai threshold dan menyimpannya ke NVS.");
        Serial.println("                     Contoh: SET 38.5");
        Serial.println(" 3. OWNER <nama>   : Menyimpan nama pemilik board ke NVS.");
        Serial.println("                     Contoh: OWNER Anton-Prafanto");
        Serial.println(" 4. RESET_NVS      : Menghapus data NVS kembali ke pengaturan default.");
        Serial.println(" 5. FS_LIST        : Menampilkan seluruh berkas di partisi LittleFS.");
        Serial.println(" 6. FS_READ <path> : Membaca isi berkas teks dari LittleFS.");
        Serial.println("                     Contoh: FS_READ /config.json");
        Serial.println(" 7. FS_LOG         : Membaca berkas riwayat booting (/boot_log.txt).");
        Serial.println(" 8. REBOOT         : Merestart ESP32 untuk menguji ketahanan data NVS.");
        Serial.println("==================================================");
    }
    else if (upperCmd == "STATUS") {
        printSystemSummary();
    }
    else if (upperCmd.startsWith("SET ")) {
        String valStr = cmd.substring(4);
        float newThresh = valStr.toFloat();
        if (newThresh == 0.0f && valStr != "0" && valStr != "0.0") {
            Serial.println("[ERROR] Nilai threshold tidak valid. Contoh format: SET 35.7");
            return;
        }

        // Buka NVS dan simpan nilai baru
        preferences.begin(NVS_NAMESPACE, false);
        preferences.putFloat("threshold", newThresh);
        preferences.end();

        currentThreshold = newThresh;
        Serial.println("[NVS SUKSES] Nilai threshold baru berhasil disimpan permanen: " + String(currentThreshold, 2) + " °C");
        Serial.println("             Silakan cabut kabel USB atau REBOOT, nilai ini tidak akan hilang!");
    }
    else if (upperCmd.startsWith("OWNER ")) {
        String newOwner = cmd.substring(6);
        newOwner.trim();

        preferences.begin(NVS_NAMESPACE, false);
        preferences.putString("owner", newOwner);
        preferences.end();

        deviceOwner = newOwner;
        Serial.println("[NVS SUKSES] Nama pemilik perangkat disimpan: '" + deviceOwner + "'");
    }
    else if (upperCmd == "RESET_NVS") {
        preferences.begin(NVS_NAMESPACE, false);
        preferences.clear(); // Hapus semua key dalam namespace
        preferences.end();

        Serial.println("[NVS FACTORY RESET] Semua data dalam namespace '" + String(NVS_NAMESPACE) + "' telah dihapus.");
        Serial.println("                      Silakan ketik REBOOT untuk memulai ulang sistem.");
    }
    else if (upperCmd == "FS_LIST") {
        Serial.println("\n[LittleFS] Daftar berkas di partisi filesystem:");
        File root = LittleFS.open("/");
        File file = root.openNextFile();
        int count = 0;
        while (file) {
            Serial.printf("  [%d] %-20s  (%d bytes)\n", ++count, file.name(), file.size());
            file = root.openNextFile();
        }
        if (count == 0) {
            Serial.println("  (Tidak ada berkas di partisi root)");
        }
    }
    else if (upperCmd.startsWith("FS_READ ")) {
        String path = cmd.substring(8);
        path.trim();
        readFileContent(path.c_str());
    }
    else if (upperCmd == "FS_LOG") {
        readFileContent("/boot_log.txt");
    }
    else if (upperCmd == "REBOOT") {
        Serial.println("[SYSTEM] Me-restart ESP32 dalam 1 detik...");
        delay(1000);
        ESP.restart();
    }
    else {
        Serial.println("[PERINTAH TIDAK DIKENAL] Ketik 'HELP' untuk melihat panduan sintaks.");
    }
}
