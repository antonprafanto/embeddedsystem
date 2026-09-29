/**
 * ============================================================================
 * JOBSHEET MINGGU 06: INDUSTRIAL LONG-DISTANCE BUS (CAN / TWAI & RS-485 MODBUS)
 * Laboratorium Sistem Tertanam - Program Studi Teknik Elektro
 * 
 * Target Hardware : ESP32-WROOM-32 / ESP32-S3
 * Framework       : Arduino Core on PlatformIO (Native ESP-IDF TWAI Driver)
 * Baud Rate Serial: 115200 bps
 * 
 * DESKRIPSI:
 * Program interaktif berbasis antarmuka terminal Serial Monitor untuk menguji
 * dua protokol komunikasi jarak jauh standar industri:
 * 1. CAN Bus 2.0B via kontroler bawaan ESP32 (TWAI - Two-Wire Automotive Interface).
 * 2. RS-485 Half-Duplex dengan pembentukan paket protokol Modbus RTU & CRC-16.
 * ============================================================================
 */

#include <Arduino.h>
#include "driver/twai.h"

// ============================================================================
// DEFINISI PIN PERIFERAL & KONFIGURASI HARDWARE
// ============================================================================
// Pin Kontroler CAN Bus / TWAI ESP32
#define CAN_TX_PIN          GPIO_NUM_5
#define CAN_RX_PIN          GPIO_NUM_4

// Pin Port Serial RS-485 (Hardware Serial2 ESP32)
#define RS485_TX_PIN        17
#define RS485_RX_PIN        16
#define RS485_DE_RE_PIN     18     // HIGH = Transmit, LOW = Receive

// LED Indikator Onboard
#define PIN_LED_ONBOARD     2

// Status Driver TWAI
static bool twai_installed = false;
static bool twai_running = false;

// ============================================================================
// FUNGSI BANTUAN CRC-16 MODBUS RTU
// ============================================================================
/**
 * Menghitung Checksum CRC-16 Modbus (Polinomial 0xA001, Nilai awal 0xFFFF).
 * Checksum ini mutlak digunakan di dunia industri untuk menjamin data tidak
 * mengalami korupsi akibat interferensi kabel panjang (hingga 1.200 meter).
 */
uint16_t calculate_modbus_crc16(const uint8_t *buffer, uint16_t length) {
    uint16_t crc = 0xFFFF;
    for (uint16_t pos = 0; pos < length; pos++) {
        crc ^= (uint16_t)buffer[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// ============================================================================
// INISIALISASI & KONTROL DRIVER TWAI (CAN BUS)
// ============================================================================
bool init_twai_driver(twai_mode_t mode, uint32_t speed_kbps) {
    // Jika driver sudah terpasang, stop dan uninstall terlebih dahulu
    if (twai_installed) {
        if (twai_running) {
            twai_stop();
            twai_running = false;
        }
        twai_driver_uninstall();
        twai_installed = false;
    }

    // 1. Konfigurasi Umum TWAI (General Config)
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, mode);
    g_config.rx_queue_len = 10;
    g_config.tx_queue_len = 10;

    // 2. Konfigurasi Kecepatan Waktu Bit (Timing Config)
    twai_timing_config_t t_config;
    switch (speed_kbps) {
        case 125: t_config = TWAI_TIMING_CONFIG_125KBITS(); break;
        case 250: t_config = TWAI_TIMING_CONFIG_250KBITS(); break;
        case 500: t_config = TWAI_TIMING_CONFIG_500KBITS(); break;
        case 1000: t_config = TWAI_TIMING_CONFIG_1MBITS(); break;
        default:  t_config = TWAI_TIMING_CONFIG_250KBITS(); break;
    }

    // 3. Konfigurasi Filter Penerimaan (Acceptance Filter: Terima Semua ID)
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    // Pasang driver ke kernel ESP-IDF
    esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
    if (err != ESP_OK) {
        Serial.printf("[ERROR] Gagal memasang driver TWAI (Kode Error: 0x%X)!\n", err);
        return false;
    }
    twai_installed = true;

    // Aktifkan antarmuka TWAI
    err = twai_start();
    if (err != ESP_OK) {
        Serial.printf("[ERROR] Gagal memulai komunikasi TWAI (Kode Error: 0x%X)!\n", err);
        return false;
    }
    twai_running = true;
    return true;
}

// ============================================================================
// DEMO 1: SELF-TEST / INTERNAL LOOPBACK (NO EXTERNAL HARDWARE REQUIRED)
// ============================================================================
void run_can_self_test_demo() {
    Serial.println("\n========================================================");
    Serial.println("  DEMO 1: CAN BUS SELF-TEST (INTERNAL NO-ACK LOOPBACK)  ");
    Serial.println("========================================================");
    Serial.println("[INFO] Fitur ini sangat ramah untuk praktikan yang hanya");
    Serial.println("       memiliki 1 board ESP32 tanpa transceiver fisik eksternal!");
    Serial.println("[INFO] Menginisialisasi TWAI ke mode TWAI_MODE_NO_ACK pada 250 kbps...");

    if (!init_twai_driver(TWAI_MODE_NO_ACK, 250)) {
        Serial.println("[FAIL] Inisialisasi Self-Test TWAI gagal.");
        return;
    }
    Serial.println("[OK] Driver TWAI berhasil aktif dalam mode No-Ack Loopback!");

    // Siapkan frame data CAN standar (11-bit ID)
    twai_message_t tx_msg;
    tx_msg.identifier = 0x123;         // ID Sensor Suhu Lingkungan (Standar 11-bit)
    tx_msg.extd = 0;                   // 0 = Standar ID (11-bit), 1 = Extended ID (29-bit)
    tx_msg.rtr = 0;                    // 0 = Data Frame, 1 = Remote Request Frame
    tx_msg.data_length_code = 4;       // Panjang Payload = 4 byte
    tx_msg.data[0] = 0x18;             // Suhu: 24.5 Celcius (245 = 0x00F5)
    tx_msg.data[1] = 0x00;
    tx_msg.data[2] = 0x62;             // Kelembaban: 62%
    tx_msg.data[3] = 0xAA;             // Status Flag: 0xAA (Sensor Sehat)

    Serial.printf("\n[TRANSMIT] Mengirim paket CAN ID: 0x%03X, DLC: %d byte\n", tx_msg.identifier, tx_msg.data_length_code);
    Serial.print("           Raw Payload: ");
    for (int i = 0; i < tx_msg.data_length_code; i++) {
        Serial.printf("0x%02X ", tx_msg.data[i]);
    }
    Serial.println();

    // Kirim pesan
    esp_err_t res = twai_transmit(&tx_msg, pdMS_TO_TICKS(100));
    if (res == ESP_OK) {
        Serial.println("[TRANSMIT OK] Frame berhasil dimasukkan ke buffer transmisi hardware.");
    } else {
        Serial.printf("[TRANSMIT FAIL] Gagal mengirim frame (Kode: 0x%X)\n", res);
        return;
    }

    // Tunggu dan baca kembali dari buffer penerima
    twai_message_t rx_msg;
    res = twai_receive(&rx_msg, pdMS_TO_TICKS(500));
    if (res == ESP_OK) {
        digitalWrite(PIN_LED_ONBOARD, HIGH);
        Serial.println("\n[RECEIVE OK] Frame berhasil diterima kembali secara loopback!");
        Serial.printf("             ID Diterima   : 0x%03X (%s)\n", 
                      rx_msg.identifier, rx_msg.extd ? "Extended 29-bit" : "Standard 11-bit");
        Serial.printf("             Panjang (DLC) : %d byte\n", rx_msg.data_length_code);
        Serial.print("             Muatan Data   : ");
        for (int i = 0; i < rx_msg.data_length_code; i++) {
            Serial.printf("0x%02X ", rx_msg.data[i]);
        }
        Serial.println();

        // Dekode data biner ke besaran enjiniring
        int16_t raw_temp = (rx_msg.data[1] << 8) | rx_msg.data[0];
        uint8_t humidity = rx_msg.data[2];
        uint8_t status = rx_msg.data[3];
        Serial.printf("             [DEKODE FISIK] Suhu: %d C, Kelembaban: %d %%, Status: 0x%02X (Normal)\n",
                      raw_temp, humidity, status);
        delay(100);
        digitalWrite(PIN_LED_ONBOARD, LOW);
    } else {
        Serial.printf("[RECEIVE TIMEOUT] Tidak ada frame yang terbaca (Kode: 0x%X).\n", res);
    }
}

// ============================================================================
// DEMO 2: TRANSMIT TELEMETRI CAN BUS NORMAL (REQUIRES TRANSCEIVER)
// ============================================================================
void run_can_transmit_demo() {
    Serial.println("\n========================================================");
    Serial.println("  DEMO 2: TRANSMIT FRAME CAN BUS (MODE NORMAL 250 KBPS) ");
    Serial.println("========================================================");
    Serial.println("[WIRING] Pastikan pin GPIO 5 (TX) dan GPIO 4 (RX) terhubung ke");
    Serial.println("         modul transceiver CAN 3.3V (SN65HVD230 / VP230) dan");
    Serial.println("         terdapat resistor terminasi 120-Ohm di bus CAN!");

    if (!init_twai_driver(TWAI_MODE_NORMAL, 250)) {
        Serial.println("[FAIL] Gagal mengaktifkan mode Normal TWAI.");
        return;
    }

    // Mengirim 3 frame berturut-turut dengan ID prioritas berbeda
    struct TelemetryFrame {
        uint32_t id;
        bool is_extended;
        uint8_t dlc;
        uint8_t data[8];
        const char *desc;
    } frames[] = {
        {0x0A0, false, 2, {0x00, 0x12}, "Emergency Stop Alarm (ID Prioritas Tinggi)"},
        {0x205, false, 4, {0x03, 0xE8, 0x00, 0x4B}, "Speed & RPM Engine (ID Prioritas Menengah)"},
        {0x18FEE600, true, 8, {0x14, 0x22, 0x30, 0x01, 0x00, 0x00, 0xFF, 0xAA}, "SAE J1939 Diagnostic Fleet (Extended 29-bit)"}
    };

    for (int i = 0; i < 3; i++) {
        twai_message_t msg;
        msg.identifier = frames[i].id;
        msg.extd = frames[i].is_extended ? 1 : 0;
        msg.rtr = 0;
        msg.data_length_code = frames[i].dlc;
        memcpy(msg.data, frames[i].data, frames[i].dlc);

        Serial.printf("\n--> Mengirim Frame #%d: [%s]\n", i + 1, frames[i].desc);
        Serial.printf("    ID: 0x%X (%s), DLC: %d\n", msg.identifier, msg.extd ? "EXT" : "STD", msg.data_length_code);
        
        esp_err_t res = twai_transmit(&msg, pdMS_TO_TICKS(100));
        if (res == ESP_OK) {
            Serial.println("    [STATUS] Sukses terkirim ke bus fisik!");
        } else {
            Serial.printf("    [STATUS ERROR] Gagal mengirim frame (Kode: 0x%X).\n", res);
            Serial.println("    [DIAGNOSA] Pastikan kabel CAN_H dan CAN_L terhubung ke node penerima");
            Serial.println("               karena pada mode normal CAN butuh ACK bit dari node lain!");
        }
        delay(250);
    }
}

// ============================================================================
// DEMO 3: MONITOR & SNIFFER FRAME CAN BUS (LISTEN ONLY)
// ============================================================================
void run_can_listen_demo() {
    Serial.println("\n========================================================");
    Serial.println("  DEMO 3: CAN BUS PACKET MONITOR / SNIFFER (LISTEN ONLY)");
    Serial.println("========================================================");
    Serial.println("[INFO] Masuk ke mode mendengarkan bus. Tekan sembarang tombol di");
    Serial.println("       keyboard komputer untuk menghentikan sniffer.");

    if (!init_twai_driver(TWAI_MODE_LISTEN_ONLY, 250)) {
        Serial.println("[FAIL] Gagal mengaktifkan mode Listen-Only TWAI.");
        return;
    }

    uint32_t packet_count = 0;
    while (!Serial.available()) {
        twai_message_t rx_msg;
        esp_err_t res = twai_receive(&rx_msg, pdMS_TO_TICKS(100));
        if (res == ESP_OK) {
            packet_count++;
            Serial.printf("[%lu ms] PKT #%lu | ID: 0x%08X (%s) | DLC: %d | Data: ",
                          millis(), packet_count, rx_msg.identifier,
                          rx_msg.extd ? "EXT" : "STD", rx_msg.data_length_code);
            for (int i = 0; i < rx_msg.data_length_code; i++) {
                Serial.printf("%02X ", rx_msg.data[i]);
            }
            Serial.println();
        }
    }
    // Bersihkan buffer serial
    while (Serial.available()) Serial.read();
    Serial.printf("\n[STOP] Sesi sniffer dihentikan. Total paket terbaca: %lu\n", packet_count);
}

// ============================================================================
// DEMO 4: RS-485 MODBUS RTU QUERY & CRC-16 CALCULATION
// ============================================================================
void run_rs485_modbus_demo() {
    Serial.println("\n========================================================");
    Serial.println("  DEMO 4: RS-485 HALF-DUPLEX & MODBUS RTU FRAME BUILDER ");
    Serial.println("========================================================");
    Serial.println("[KONSEP] RS-485 menggunakan 2 kawat diferensial (A dan B).");
    Serial.println("         Pin DE/RE wajib ditarik HIGH sebelum transmisi serial,");
    Serial.println("         dan wajib ditarik LOW seketika transmisi selesai!");

    // Siapkan frame permintaan Modbus RTU standar:
    // Permintaan baca 2 Holding Registers mulai dari alamat 0x006B dari Slave ID 0x01
    uint8_t modbus_req[8];
    modbus_req[0] = 0x01;              // Slave Address (Alamat perangkat target: 1)
    modbus_req[1] = 0x03;              // Function Code: 0x03 (Read Holding Registers)
    modbus_req[2] = 0x00;              // Starting Address High
    modbus_req[3] = 0x6B;              // Starting Address Low (Register 107)
    modbus_req[4] = 0x00;              // Quantity of Registers High
    modbus_req[5] = 0x02;              // Quantity of Registers Low (Membaca 2 register)

    // Hitung Checksum CRC-16 Modbus terhadap 6 byte pertama
    uint16_t crc = calculate_modbus_crc16(modbus_req, 6);
    modbus_req[6] = crc & 0xFF;        // CRC-16 Low Byte (Aturan Modbus: Low byte dulu!)
    modbus_req[7] = (crc >> 8) & 0xFF; // CRC-16 High Byte

    Serial.println("\n[BEDAH PAKET MODBUS RTU (HEX)]:");
    Serial.printf("  • Slave Address    : 0x%02X (Perangkat #1)\n", modbus_req[0]);
    Serial.printf("  • Function Code    : 0x%02X (Read Holding Registers)\n", modbus_req[1]);
    Serial.printf("  • Start Register   : 0x%02X%02X (Alamat 107 desimal)\n", modbus_req[2], modbus_req[3]);
    Serial.printf("  • Jumlah Register  : 0x%02X%02X (2 Register = 4 Byte data)\n", modbus_req[4], modbus_req[5]);
    Serial.printf("  • Kalkulasi CRC-16 : 0x%04X -> Low: 0x%02X, High: 0x%02X\n", crc, modbus_req[6], modbus_req[7]);
    
    Serial.print("  • Paket Lengkap    : ");
    for (int i = 0; i < 8; i++) {
        Serial.printf("%02X ", modbus_req[i]);
    }
    Serial.println();

    // Langkah transmisi fisik ke transceiver MAX3485 / MAX485:
    Serial.println("\n[EKSEKUSI FISIK RS-485]:");
    Serial.println("  1. Menarik Pin DE/RE ke HIGH (Driver Aktif Menguasai Kabel Bus)...");
    digitalWrite(RS485_DE_RE_PIN, HIGH);
    delayMicroseconds(50); // Delay stabilisasi gerbang driver

    Serial.println("  2. Mengirimkan 8 byte paket Modbus lewat Serial2 Hardware UART...");
    Serial2.write(modbus_req, sizeof(modbus_req));
    Serial2.flush(); // Tunggu hingga byte fisik terakhir tuntas keluar dari shift register UART!

    Serial.println("  3. Menarik Pin DE/RE ke LOW (Kembali ke Mode Receiver Siaga)...");
    digitalWrite(RS485_DE_RE_PIN, LOW);
    Serial.println("[OK] Transmisi frame Modbus RTU tuntas!");
}

// ============================================================================
// DEMO 5: STATUS & DIAGNOSTIK KESEHATAN BUS CAN (TWAI HEALTH INSPECTION)
// ============================================================================
void run_can_diagnostics_demo() {
    Serial.println("\n========================================================");
    Serial.println("  DEMO 5: DIAGNOSTIK KESEHATAN CONTROLLER CAN BUS TWAI  ");
    Serial.println("========================================================");

    if (!twai_installed) {
        Serial.println("[INFO] Memasang driver TWAI untuk inspeksi status...");
        init_twai_driver(TWAI_MODE_NORMAL, 250);
    }

    twai_status_info_t status_info;
    esp_err_t res = twai_get_status_info(&status_info);
    if (res != ESP_OK) {
        Serial.printf("[ERROR] Gagal membaca status info TWAI (Kode: 0x%X)\n", res);
        return;
    }

    const char *state_str = "UNKNOWN";
    switch (status_info.state) {
        case TWAI_STATE_STOPPED:     state_str = "STOPPED (Tidak Aktif)"; break;
        case TWAI_STATE_RUNNING:     state_str = "RUNNING (Beroperasi Normal)"; break;
        case TWAI_STATE_BUS_OFF:     state_str = "BUS-OFF (Terisolasi karena Gangguan Fisik/Korsleting!)"; break;
        case TWAI_STATE_RECOVERING:  state_str = "RECOVERING (Proses Pemulihan Otomatis 128x11 bit)"; break;
    }

    Serial.printf("  • Status Operasional Controller: %s\n", state_str);
    Serial.printf("  • Transmit Error Counter (TEC) : %lu (Maks aman: 127, >255 = Bus-Off)\n", status_info.tx_error_counter);
    Serial.printf("  • Receive Error Counter (REC)  : %lu\n", status_info.rx_error_counter);
    Serial.printf("  • Pesan Mengantri di RX Queue  : %lu\n", status_info.msgs_to_rx);
    Serial.printf("  • Pesan Mengantri di TX Queue  : %lu\n", status_info.msgs_to_tx);
    Serial.printf("  • Kegagalan Transmisi (Failed) : %lu\n", status_info.tx_failed_count);
    Serial.printf("  • Kehilangan Arbitrasi (Lost)  : %lu\n", status_info.arb_lost_count);
    Serial.printf("  • Deteksi Bus Error            : %lu\n", status_info.bus_error_count);
    Serial.println("--------------------------------------------------------");
    Serial.println("[INSIGHT TEKNIK ELEKTRO]:");
    Serial.println("CAN Bus memiliki mekanisme 'Fault Confinement'. Jika ada kabel");
    Serial.println("putus atau korslet, TEC akan naik. Bila TEC > 255, chip otomatis");
    Serial.println("memutus diri (Bus-Off) agar tidak merusak komunikasi node lain!");
}

// ============================================================================
// TAMPILKAN MENU CLI
// ============================================================================
void print_menu() {
    Serial.println("\n+-------------------------------------------------------------+");
    Serial.println("|   MENU INTERAKTIF LAB WEEK 06: INDUSTRIAL CAN & RS-485      |");
    Serial.println("+-------------------------------------------------------------+");
    Serial.println("| [1] Self-Test TWAI Loopback (Uji Mandiri Tanpa Transceiver) |");
    Serial.println("| [2] Transmit Frame CAN Bus Normal (Kirim Paket Telemetri)   |");
    Serial.println("| [3] Sniffer / Monitor Frame CAN Bus (Listen Only Mode)      |");
    Serial.println("| [4] RS-485 Modbus RTU Query Generator (Kalkulasi CRC-16)    |");
    Serial.println("| [5] Status & Diagnostik Bus CAN (TEC, REC, & Bus-Off State) |");
    Serial.println("| [m] Cetak Ulang Menu Bantuan                                |");
    Serial.println("+-------------------------------------------------------------+");
    Serial.print("Ketik angka pilihan Anda [1-5]: ");
}

// ============================================================================
// SETUP & MAIN LOOP
// ============================================================================
void setup() {
    // 1. Inisialisasi Serial Debugging ke Komputer
    Serial.begin(115200);
    delay(1000);

    // 2. Inisialisasi LED Onboard
    pinMode(PIN_LED_ONBOARD, OUTPUT);
    digitalWrite(PIN_LED_ONBOARD, LOW);

    // 3. Inisialisasi Serial2 Hardware UART untuk RS-485
    pinMode(RS485_DE_RE_PIN, OUTPUT);
    digitalWrite(RS485_DE_RE_PIN, LOW); // Default mode Receive
    Serial2.begin(9600, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);

    // 4. Tampilkan Salam Pembuka Ramah Awam
    Serial.println("\n========================================================");
    Serial.println("SELAMAT DATANG DI PRAKTIKUM MINGGU 06 - SISTEM TERTANAM");
    Serial.println("Eksplorasi Komunikasi Industri: CAN Bus / TWAI & RS-485");
    Serial.println("Laboratorium Sistem Tertanam - Teknik Elektro");
    Serial.println("========================================================");
    Serial.printf("[INFO PINOUT ESP32]:\n");
    Serial.printf("  • CAN Bus (TWAI) : TX = GPIO %d, RX = GPIO %d\n", CAN_TX_PIN, CAN_RX_PIN);
    Serial.printf("  • RS-485 Serial2 : TX = GPIO %d, RX = GPIO %d, DE/RE = GPIO %d\n", 
                  RS485_TX_PIN, RS485_RX_PIN, RS485_DE_RE_PIN);
    Serial.println("--------------------------------------------------------");
    
    print_menu();
}

void loop() {
    if (Serial.available()) {
        char input = Serial.read();
        
        // Abaikan karakter newline/carriage return
        if (input == '\r' || input == '\n') return;

        Serial.println(input); // Echo pilihan user

        switch (input) {
            case '1':
                run_can_self_test_demo();
                break;
            case '2':
                run_can_transmit_demo();
                break;
            case '3':
                run_can_listen_demo();
                break;
            case '4':
                run_rs485_modbus_demo();
                break;
            case '5':
                run_can_diagnostics_demo();
                break;
            case 'm':
            case 'M':
                print_menu();
                return;
            default:
                Serial.println("[!] Perintah tidak dikenali. Ketik angka 1-5 atau 'm'.");
                break;
        }

        Serial.println("\n[SELESAI] Tekan 'm' untuk menu atau pilih angka pengujian lain.");
        Serial.print("Pilihan Anda: ");
    }
}
