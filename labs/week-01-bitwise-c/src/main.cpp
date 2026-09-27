/**
 * ============================================================================
 * MODUL PRAKTIKUM SISTEM TERTANAM - MINGGU 1
 * Topik: Fondasi Bahasa C untuk Sistem Tertanam & Manipulasi Bitwise
 * ============================================================================
 * Mahasiswa: [Nama Lengkap Anda]
 * NIM      : [Nomor Induk Mahasiswa]
 * Kelas    : S1 Teknik Elektro
 * ============================================================================
 */

#include <Arduino.h>

// Definisi pin fisik LED onboard ESP32
#define ONBOARD_LED_PIN 2

// Simulasi Register Status 8-bit Mikrokontroler
// Bit 0: ALARM_FLAG
// Bit 1: MOTOR_RUN_FLAG
// Bit 2: SENSOR_READY_FLAG
// Bit 3: COMM_OK_FLAG
// Bit 4-7: RESERVED
volatile uint8_t virtual_register = 0b00000000;

// Definisi Bit Position
#define BIT_ALARM        0
#define BIT_MOTOR_RUN    1
#define BIT_SENSOR_READY 2
#define BIT_COMM_OK      3

// ============================================================================
// FUNGSI BANTUAN OPERASI BITWISE (TIER 1 - WAJIB DIKERJAKAN)
// ============================================================================

/**
 * @brief Menyalakan (Set to 1) bit tertentu pada register menggunakan bitwise OR (|)
 * Rumus: Reg = Reg | (1 << bit_pos)
 */
void set_register_bit(volatile uint8_t *reg, uint8_t bit_pos) {
    // TODO [Level 1.1]: Tuliskan rumus bitwise OR di bawah ini
    *reg |= (1 << bit_pos);
}

/**
 * @brief Mematikan (Clear to 0) bit tertentu pada register menggunakan bitwise AND (&) dan NOT (~)
 * Rumus: Reg = Reg & ~(1 << bit_pos)
 */
void clear_register_bit(volatile uint8_t *reg, uint8_t bit_pos) {
    // TODO [Level 1.2]: Tuliskan rumus bitwise AND + NOT di bawah ini
    *reg &= ~(1 << bit_pos);
}

/**
 * @brief Membalik nilai (Toggle 0->1 atau 1->0) bit tertentu menggunakan bitwise XOR (^)
 * Rumus: Reg = Reg ^ (1 << bit_pos)
 */
void toggle_register_bit(volatile uint8_t *reg, uint8_t bit_pos) {
    // TODO [Level 1.3]: Tuliskan rumus bitwise XOR di bawah ini
    *reg ^= (1 << bit_pos);
}

/**
 * @brief Memeriksa status (Check bit) apakah bernilai 1 atau 0
 * Return: true jika bit bernilai 1, false jika bit bernilai 0
 */
bool check_register_bit(volatile uint8_t reg, uint8_t bit_pos) {
    // TODO [Level 1.4]: Tuliskan rumus pengecekan bit di bawah ini
    return (reg & (1 << bit_pos)) != 0;
}

/**
 * @brief Mencetak isi byte dalam format biner ke Serial Monitor
 */
void print_binary(uint8_t byte_val) {
    for (int8_t i = 7; i >= 0; i--) {
        Serial.print((byte_val & (1 << i)) ? "1" : "0");
        if (i == 4) Serial.print(" "); // Pemisah nibble
    }
}

// ============================================================================
// STATE MACHINE NON-BLOCKING (TIER 2 - ANALISIS SISTEM)
// ============================================================================

// Variabel waktu untuk non-blocking loop
unsigned long previous_millis = 0;
const unsigned long BLINK_INTERVAL_MS = 500; // Periode kedip 500 ms
bool led_state = false;

void run_non_blocking_led_state_machine() {
    unsigned long current_millis = millis();

    // Memeriksa apakah selang waktu sudah tercapai tanpa menggunakan fungsi delay()
    if (current_millis - previous_millis >= BLINK_INTERVAL_MS) {
        previous_millis = current_millis;

        // Toggle status LED secara fisik
        led_state = !led_state;
        digitalWrite(ONBOARD_LED_PIN, led_state ? HIGH : LOW);

        // Toggle juga bit indikator di register status virtual
        toggle_register_bit(&virtual_register, BIT_COMM_OK);

        // Cetak status ke serial monitor
        Serial.print("[Tick: ");
        Serial.print(current_millis);
        Serial.print(" ms] Status Register: 0b");
        print_binary(virtual_register);
        Serial.print(" | LED State: ");
        Serial.println(led_state ? "ON" : "OFF");
    }
}

// ============================================================================
// SETUP & MAIN LOOP
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000); // Jeda stabilisasi serial

    pinMode(ONBOARD_LED_PIN, OUTPUT);
    digitalWrite(ONBOARD_LED_PIN, LOW);

    Serial.println("==================================================");
    Serial.println("  PRAKTIKUM SISTEM TERTANAM - MINGGU 01           ");
    Serial.println("  Uji Operasi Bitwise & Non-Blocking State Machine");
    Serial.println("==================================================");

    // Pengujian Awal Operasi Bitwise (Level 1)
    Serial.print("Status Register Awal          : 0b");
    print_binary(virtual_register);
    Serial.println();

    Serial.println("-> Menyalakan BIT_SENSOR_READY (Bit 2)...");
    set_register_bit(&virtual_register, BIT_SENSOR_READY);
    Serial.print("   Hasil: 0b");
    print_binary(virtual_register);
    Serial.println();

    Serial.println("-> Menyalakan BIT_MOTOR_RUN (Bit 1)...");
    set_register_bit(&virtual_register, BIT_MOTOR_RUN);
    Serial.print("   Hasil: 0b");
    print_binary(virtual_register);
    Serial.println();

    Serial.println("-> Mematikan BIT_SENSOR_READY (Bit 2)...");
    clear_register_bit(&virtual_register, BIT_SENSOR_READY);
    Serial.print("   Hasil: 0b");
    print_binary(virtual_register);
    Serial.println();

    Serial.println("==================================================");
    Serial.println("Memulai Loop Utama (Non-Blocking State Machine)...");
}

void loop() {
    // Menjalankan tugas non-blocking
    run_non_blocking_led_state_machine();

    // CPU tidak terhalang (tidak ada fungsi delay() di sini!)
    // Mahasiswa dapat menambahkan komputasi lain secara bebas di loop ini.
}
