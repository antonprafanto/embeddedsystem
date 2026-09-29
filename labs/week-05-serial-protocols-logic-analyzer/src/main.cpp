/**
 * ============================================================================
 * PRAKTIKUM SISTEM TERTANAM — MINGGU 05: PROTOKOL SERIAL & LOGIC ANALYZER
 * Program Studi Sarjana (S1) Teknik Elektro
 * 
 * Deskripsi:
 * Program interaktif komprehensif untuk pengujian dan pembuktian protokol serial:
 * 1. I2C (Inter-Integrated Circuit): Bus Scanner, Register Read & Emulation (GPIO 21 SDA, GPIO 22 SCL)
 * 2. UART (Asinkron): Structured Frame Generator & Framing Error Test (GPIO 17 TX2, GPIO 16 RX2)
 * 3. SPI (Synchronous 4-Wire): Hardware Transaction & CPOL/CPHA Modes (GPIO 18 SCK, 23 MOSI, 19 MISO, 5 CS)
 * 4. Mode Latihan Logic Analyzer: Mengirim sinyal berulang agar mudah di-capture dan di-decode di PulseView.
 * ============================================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// -----------------------------------------------------------------------------
// KONFIGURASI PIN PERIFERAL KOMUNIKASI
// -----------------------------------------------------------------------------
// Bus I2C Default ESP32
#define I2C_SDA_PIN        21
#define I2C_SCL_PIN        22
#define I2C_CLOCK_SPEED    100000  // 100 kHz (Standard Mode I2C)

// Bus SPI (VSPI) Default ESP32
#define SPI_SCK_PIN        18
#define SPI_MISO_PIN       19
#define SPI_MOSI_PIN       23
#define SPI_CS_PIN         5       // Chip Select (Aktif LOW)
#define SPI_CLOCK_SPEED    1000000 // 1 MHz (Aman untuk capture Logic Analyzer 24MHz)

// Port Serial Tambahan UART2 (HardwareSerial)
#define UART2_TX_PIN       17
#define UART2_RX_PIN       16
#define UART2_BAUD_RATE    115200

// Inisialisasi Objek Hardware
HardwareSerial SerialPort2(2);
SPIClass vspi(VSPI);

// Variabel Kontrol
bool burstModeActive = false;
unsigned long lastBurstTime = 0;
const unsigned long BURST_INTERVAL_MS = 600; // Interval antar paket burst
uint8_t packetSequence = 0;

// -----------------------------------------------------------------------------
// FUNGSI TAMPILAN BANNER & MENU EDUKATIF
// -----------------------------------------------------------------------------
void printBanner() {
    Serial.println("\n==================================================================");
    Serial.println("  LAB SISTEM TERTANAM: PROTOKOL SERIAL & 8-CHANNEL LOGIC ANALYZER ");
    Serial.println("  Platform: ESP32-WROOM-32 | Clock CPU: 240 MHz | Framework: Arduino ");
    Serial.println("==================================================================");
    Serial.println("Pemetaan Pin Jumper ke Probe USB Logic Analyzer:");
    Serial.println("  • CH 0 -> GPIO 21 (I2C SDA  - Serial Data)");
    Serial.println("  • CH 1 -> GPIO 22 (I2C SCL  - Serial Clock)");
    Serial.println("  • CH 2 -> GPIO 5  (SPI CS   - Chip Select, Aktif LOW)");
    Serial.println("  • CH 3 -> GPIO 18 (SPI SCK  - Clock SPI)");
    Serial.println("  • CH 4 -> GPIO 23 (SPI MOSI - Master Out Slave In)");
    Serial.println("  • CH 5 -> GPIO 17 (UART TX2 - Transmit Serial 2)");
    Serial.println("  • GND  -> ESP32 GND (WAJIB: Referensi Ground Bersama!)");
    Serial.println("------------------------------------------------------------------");
    Serial.println("PILIHAN MENU PENGUJIAN:");
    Serial.println("  [1] Jalankan I2C Bus Scanner (Deteksi Alamat 0x01 s.d. 0x7F)");
    Serial.println("  [2] Uji Transaksi I2C (Baca Register Chip-ID 0xD0)");
    Serial.println("  [3] Kirim Paket Data UART Terstruktur (Frame Start/Checksum/Stop)");
    Serial.println("  [4] Uji Transaksi SPI (Perbandingan Mode 0 vs Mode 3)");
    Serial.println("  [5] Aktifkan / Matikan Mode Burst Kontinu (Untuk Capture PulseView)");
    Serial.println("  [h] Tampilkan Ulang Menu Bantuan Ini");
    Serial.println("==================================================================");
    Serial.print("Pilih opsi [1-5 / h]: ");
}

// -----------------------------------------------------------------------------
// MENU 1: I2C BUS SCANNER
// -----------------------------------------------------------------------------
void runI2CScanner() {
    Serial.println("\n[I2C] Memulai Pemindaian Bus I2C (Alamat 0x01 s.d. 0x7F)...");
    Serial.println("--------------------------------------------------");
    
    int deviceCount = 0;
    
    // Tampilan header grid heksadesimal
    Serial.print("     ");
    for (int col = 0; col < 16; col++) {
        Serial.printf("%X  ", col);
    }
    Serial.println();

    for (int row = 0; row < 128; row += 16) {
        Serial.printf("0x%02X:", row);
        for (int col = 0; col < 16; col++) {
            int addr = row + col;
            if (addr < 1 || addr > 127) {
                Serial.print(" --");
                continue;
            }

            // Memulai transmisi ke alamat uji
            Wire.beginTransmission(addr);
            byte error = Wire.endTransmission();

            if (error == 0) {
                // Perangkat merespons ACK
                Serial.printf(" %02X", addr);
                deviceCount++;
            } else if (error == 4) {
                // Error jalur fisik / bus hang
                Serial.print(" ER");
            } else {
                // Merespons NACK (tidak ada perangkat di alamat ini)
                Serial.print(" ..");
            }
        }
        Serial.println();
    }
    
    Serial.println("--------------------------------------------------");
    if (deviceCount == 0) {
        Serial.println(">> HASIL: Tidak ada perangkat I2C yang terdeteksi!");
        Serial.println("   Catatan Enjiniring:");
        Serial.println("   1. Pastikan sensor terhubung ke GPIO 21 (SDA) dan GPIO 22 (SCL).");
        Serial.println("   2. Pastikan sensor mendapatkan catu daya 3.3V dan GND terhubung rapat.");
        Serial.println("   3. Karena sirkuit Open-Drain, pastikan resistor pull-up 4.7kΩ terpasang!");
    } else {
        Serial.printf(">> HASIL: Berhasil mendeteksi %d perangkat aktif di bus I2C.\n", deviceCount);
        Serial.println("   Referensi Alamat Sensor Populer:");
        Serial.println("   • 0x76 / 0x77 : Sensor Tekanan/Suhu Bosch BME280 / BMP280");
        Serial.println("   • 0x3C / 0x3D : Layar Display OLED SSD1306 (0.96 inch)");
        Serial.println("   • 0x68        : Gyroscope/Accelerometer MPU6050 atau RTC DS3231");
        Serial.println("   • 0x38        : Sensor Kelembaban AHT10 / AHT20");
    }
}

// -----------------------------------------------------------------------------
// MENU 2: TRANSAKSI REGISTER I2C (BACA CHIP-ID 0xD0)
// -----------------------------------------------------------------------------
void runI2CRegisterRead() {
    uint8_t targetAddress = 0x76; // Alamat khas Bosch BME280
    uint8_t regChipID = 0xD0;     // Register alamat Chip-ID

    Serial.printf("\n[I2C] Mengirim Transaksi Pembacaan Register ke 0x%02X...\n", targetAddress);
    Serial.println("  Alur Fisik yang Terkirim ke Bus:");
    Serial.printf("  1. [START] -> [Addr 0x%02X + WRITE(0)] -> [Reg 0x%02X] -> [ACK/NACK]\n", targetAddress, regChipID);
    Serial.printf("  2. [REPEATED START] -> [Addr 0x%02X + READ(1)] -> [BACA 1 BYTE] -> [NACK] -> [STOP]\n", targetAddress);

    // Langkah 1: Arahkan pointer internal register sensor
    Wire.beginTransmission(targetAddress);
    Wire.write(regChipID);
    byte statusWrite = Wire.endTransmission(false); // false = Kirim Repeated Start (RESTART)

    if (statusWrite != 0) {
        Serial.printf("  [!] Peringatan: Slave 0x%02X tidak merespons ACK (NACK received / Device absent).\n", targetAddress);
        Serial.println("      (Pulsa sinyal tetap sukses dikirim ke kabel SDA/SCL untuk direkam Logic Analyzer!)");
    } else {
        // Langkah 2: Minta 1 byte data balasan dari slave
        Wire.requestFrom(targetAddress, (uint8_t)1);
        if (Wire.available()) {
            uint8_t chipID = Wire.read();
            Serial.printf("  [OK] Nilai Register 0x%02X berhasil dibaca: 0x%02X\n", regChipID, chipID);
            if (chipID == 0x60) {
                Serial.println("       -> Terkonfirmasi: Sensor Fisik Asli adalah Bosch BME280!");
            } else if (chipID == 0x58) {
                Serial.println("       -> Terkonfirmasi: Sensor Fisik Asli adalah Bosch BMP280!");
            }
        }
    }
}

// -----------------------------------------------------------------------------
// MENU 3: PAKET UART TERSTRUKTUR & DEMO FRAMING
// -----------------------------------------------------------------------------
void sendUARTStructuredPacket() {
    Serial.println("\n[UART] Mengirim Paket Data Terstruktur ke Port Serial 2 (GPIO 17 TX2)...");
    
    // Format Paket Protokol Biner:
    // [0] START BYTE : 0xAA (Penanda awal paket)
    // [1] SEQUENCE   : Nomor urut paket (0-255)
    // [2] PAYLOAD_1  : Data simulasi sensor ADC (Tinggi)
    // [3] PAYLOAD_2  : Data simulasi sensor ADC (Rendah)
    // [4] CHECKSUM   : XOR dari seluruh byte sebelumnya (Integritas data)
    // [5] STOP BYTE  : 0x55 (Penanda akhir paket)
    
    uint16_t simADC = 2048 + (packetSequence * 5) % 1000;
    uint8_t p1 = (simADC >> 8) & 0xFF;
    uint8_t p2 = simADC & 0xFF;
    uint8_t startByte = 0xAA;
    uint8_t stopByte = 0x55;
    uint8_t checksum = startByte ^ packetSequence ^ p1 ^ p2;

    uint8_t packet[6] = { startByte, packetSequence, p1, p2, checksum, stopByte };

    // Kirim data biner murni via Serial2
    SerialPort2.write(packet, sizeof(packet));

    Serial.println("  Paket Hex yang Terkirim:");
    Serial.printf("  [0x%02X] [Seq:0x%02X] [D1:0x%02X] [D2:0x%02X] [CRC:0x%02X] [0x%02X]\n",
                  packet[0], packet[1], packet[2], packet[3], packet[4], packet[5]);
    Serial.println("  Baud Rate UART2: 115200 bps (1 Start Bit, 8 Data Bit, No Parity, 1 Stop Bit).");
    Serial.println("  Gunakan Protocol Decoder 'UART' di PulseView pada Channel 5 untuk melihat paket ini!");
    
    packetSequence++;
}

// -----------------------------------------------------------------------------
// MENU 4: TRANSAKSI SPI (PERBANDINGAN MODE 0 VS MODE 3)
// -----------------------------------------------------------------------------
void runSPITransactions() {
    Serial.println("\n[SPI] Menjalankan Transaksi SPI Hardware (VSPI)...");
    Serial.println("  Pin Terhubung: CS=GPIO 5, SCK=GPIO 18, MOSI=GPIO 23");

    // 1. Uji Transaksi SPI Mode 0 (CPOL=0, CPHA=0)
    // Clock Idle di posisi LOW (0V). Data di-sample pada Sisi Naik (Rising Edge).
    Serial.println("  -> Mengirim 4 Byte Data pada SPI MODE 0 (Idle LOW)...");
    vspi.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE0));
    digitalWrite(SPI_CS_PIN, LOW); // Tarik CS ke LOW untuk mengaktifkan slave
    delayMicroseconds(5);

    uint8_t txDataMode0[4] = { 0x9F, 0x00, 0xA5, 0x5A };
    uint8_t rxDataMode0[4];
    for (int i = 0; i < 4; i++) {
        rxDataMode0[i] = vspi.transfer(txDataMode0[i]); // Full-Duplex: Kirim sekaligus terima
    }

    delayMicroseconds(5);
    digitalWrite(SPI_CS_PIN, HIGH); // Kembalikan CS ke HIGH (Idle)
    vspi.endTransaction();

    delay(20);

    // 2. Uji Transaksi SPI Mode 3 (CPOL=1, CPHA=1)
    // Clock Idle di posisi HIGH (3.3V). Data di-sample pada Sisi Naik (Rising Edge).
    Serial.println("  -> Mengirim 4 Byte Data pada SPI MODE 3 (Idle HIGH)...");
    vspi.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE3));
    digitalWrite(SPI_CS_PIN, LOW);
    delayMicroseconds(5);

    uint8_t txDataMode3[4] = { 0xD0, 0xFF, 0x12, 0x34 };
    uint8_t rxDataMode3[4];
    for (int i = 0; i < 4; i++) {
        rxDataMode3[i] = vspi.transfer(txDataMode3[i]);
    }

    delayMicroseconds(5);
    digitalWrite(SPI_CS_PIN, HIGH);
    vspi.endTransaction();

    Serial.println("  [OK] Transaksi SPI selesai!");
    Serial.println("  Amati di PulseView: Perhatikan garis SCK (CH3) sebelum CS aktif:");
    Serial.println("  - Pada Mode 0: SCK berada di 0V (LOW).");
    Serial.println("  - Pada Mode 3: SCK berada di 3.3V (HIGH)!");
}

// -----------------------------------------------------------------------------
// MENU 5: BURST SINKRON SEMUA PROTOKOL (LOGIC ANALYZER TRAINING MODE)
// -----------------------------------------------------------------------------
void triggerMultiProtocolBurst() {
    // 1. Kirim Transaksi I2C (Alamat 0x76, Register 0xD0)
    Wire.beginTransmission(0x76);
    Wire.write(0xD0);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)0x76, (uint8_t)1);
    if (Wire.available()) Wire.read();

    // Jeda mikrodetik kecil agar sinyal tidak tumpang tindih rapat
    delayMicroseconds(50);

    // 2. Kirim Transaksi SPI Mode 0 (4 byte)
    vspi.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE0));
    digitalWrite(SPI_CS_PIN, LOW);
    delayMicroseconds(2);
    vspi.transfer(0xAA);
    vspi.transfer(packetSequence);
    vspi.transfer(0x55);
    delayMicroseconds(2);
    digitalWrite(SPI_CS_PIN, HIGH);
    vspi.endTransaction();

    delayMicroseconds(50);

    // 3. Kirim Frame UART2 (6 byte)
    uint8_t uPacket[4] = { 0x55, packetSequence, (uint8_t)(~packetSequence), 0xAA };
    SerialPort2.write(uPacket, sizeof(uPacket));

    packetSequence++;
}

// -----------------------------------------------------------------------------
// SETUP & INITIALIZATION
// -----------------------------------------------------------------------------
void setup() {
    // Inisialisasi Serial Monitor Utama (Debugging ke PC)
    Serial.begin(115200);
    while (!Serial && millis() < 3000); // Tunggu serial monitor siap
    delay(500);

    // Inisialisasi Bus I2C
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(I2C_CLOCK_SPEED);

    // Inisialisasi Bus SPI (VSPI)
    pinMode(SPI_CS_PIN, OUTPUT);
    digitalWrite(SPI_CS_PIN, HIGH); // CS default adalah HIGH (Idle/Non-aktif)
    vspi.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_CS_PIN);

    // Inisialisasi Port Serial Tambahan UART2
    SerialPort2.begin(UART2_BAUD_RATE, SERIAL_8N1, UART2_RX_PIN, UART2_TX_PIN);

    printBanner();
}

// -----------------------------------------------------------------------------
// MAIN LOOP & SERIAL COMMAND PARSER
// -----------------------------------------------------------------------------
void loop() {
    // Mode Burst Berulang (Bila diaktifkan via Menu 5)
    if (burstModeActive) {
        if (millis() - lastBurstTime >= BURST_INTERVAL_MS) {
            lastBurstTime = millis();
            triggerMultiProtocolBurst();
            Serial.printf(">> [Burst #%d Terkirim ke CH0..CH5] ", packetSequence);
        }
    }

    // Pembacaan Perintah Serial Monitor Pengguna
    if (Serial.available() > 0) {
        char cmd = Serial.read();

        // Abaikan karakter newline dan carriage return
        if (cmd == '\r' || cmd == '\n') return;

        switch (cmd) {
            case '1':
                runI2CScanner();
                break;
            case '2':
                runI2CRegisterRead();
                break;
            case '3':
                sendUARTStructuredPacket();
                break;
            case '4':
                runSPITransactions();
                break;
            case '5':
                burstModeActive = !burstModeActive;
                if (burstModeActive) {
                    Serial.println("\n[BURST MODE AKTIF] Mengirim pulsa I2C, SPI, dan UART setiap 600 ms.");
                    Serial.println("  Buka PulseView -> Klik 'Run' untuk merekam sinyal multi-channel!");
                    Serial.println("  (Tekan tombol '5' kembali di Serial Monitor untuk menghentikan burst).");
                } else {
                    Serial.println("\n[BURST MODE NONAKTIF] Pengiriman sinyal kontinu dihentikan.");
                }
                break;
            case 'h':
            case 'H':
                printBanner();
                break;
            default:
                Serial.printf("\nPerintah '%c' tidak dikenali. Ketik 'h' untuk melihat menu.\n", cmd);
                break;
        }
        Serial.print("\nPilih opsi [1-5 / h]: ");
    }
}
