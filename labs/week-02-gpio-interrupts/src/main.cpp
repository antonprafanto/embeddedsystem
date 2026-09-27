/**
 * ============================================================================
 * MODUL PRAKTIKUM SISTEM TERTANAM - MINGGU 2
 * Topik: Karakteristik Elektrikal Pin, Hardware Interrupt & Crash Debugging
 * ============================================================================
 * Mahasiswa: [Nama Lengkap Anda]
 * NIM      : [Nomor Induk Mahasiswa]
 * Kelas    : S1 Teknik Elektro
 * ============================================================================
 */

#include <Arduino.h>

// ============================================================================
// KONFIGURASI PIN PERANGKAT KERAS (HARDWARE PIN MAPPING)
// Catatan: Selalu gunakan pin yang tergolong "PIN PALING AMAN" (lihat docs/esp32_pin_gotchas.md)
// ============================================================================
#define BUTTON_PIN       18   // Pin tombol interupsi (memiliki Pull-Up internal)
#define ONBOARD_LED_PIN   2   // Pin LED onboard standar ESP32 DevKit

// ============================================================================
// VARIABEL GLOBAL BERBAGI (SHARED VOLATILE VARIABLES)
// Aturan Emas: Variabel yang diubah di dalam ISR WAJIB dideklarasikan sebagai volatile!
// ============================================================================
volatile uint32_t button_press_count = 0;
volatile bool button_event_flag = false;
volatile unsigned long last_interrupt_time = 0;
const unsigned long DEBOUNCE_LOCKOUT_MS = 50; // Jendela waktu debounce (50 ms)

// ============================================================================
// FUNGSI INTERRUPT SERVICE ROUTINE (ISR) - LEVEL 1
// ============================================================================
/**
 * @brief ISR yang dipanggil secara otomatis oleh hardware saat tombol ditekan (FALLING edge).
 * 
 * ATURAN KERAMAT ISR:
 * 1. Selalu gunakan atribut IRAM_ATTR agar fungsi tersimpan di memori instruksi internal (IRAM),
 *    bukan di SPI Flash yang lambat.
 * 2. Jangan pernah menggunakan delay() atau operasi waktu panjang di dalam ISR!
 * 3. Hindari memanggil Serial.print() di dalam ISR karena menggunakan buffer interupsi lain.
 * 4. Buat ISR seringkas mungkin: hanya catat status/flag, lalu proses di loop() utama!
 */
void IRAM_ATTR isr_button_pressed() {
    unsigned long current_time = millis();

    // Algoritma Software Debouncing:
    // Hanya proses jika selisih waktu dari pemicu terakhir > 50 milidetik
    if (current_time - last_interrupt_time > DEBOUNCE_LOCKOUT_MS) {
        button_press_count++;
        button_event_flag = true;
        last_interrupt_time = current_time;
    }
}

// ============================================================================
// SIMULASI CRASH TERKONTROL (LEVEL 2 - CRASH DEBUGGING)
// ============================================================================
/**
 * @brief Fungsi untuk mensimulasikan kegagalan fatal akses memori (Null Pointer Dereference).
 * Fungsi ini sengaja dibuat agar mahasiswa belajar membaca log "Guru Meditation Error"
 * dan melacak baris program penyebab crash melalui Exception Decoder!
 */
void trigger_controlled_crash() {
    Serial.println("\n[PERINGATAN BAHAYA]: Memulai simulasi crash memori...");
    Serial.println("Mengakses alamat pointer kosong (Null Pointer Dereference)...");
    Serial.flush(); // Pastikan semua teks serial terkirim sebelum CPU panic
    delay(100);

    // BARIS PENYEBAB CRASH (Target Pelacakan Backtrace):
    volatile int *illegal_pointer = nullptr;
    *illegal_pointer = 42; // EXCEPTION FATAL: LoadProhibited / StoreProhibited!
}

// ============================================================================
// FUNGSI BANTUAN UI SERIAL MONITOR
// ============================================================================
void print_banner() {
    Serial.println("\n========================================================");
    Serial.println("  PRAKTIKUM SISTEM TERTANAM - MINGGU 02                 ");
    Serial.println("  Hardware Interrupt, Debouncing, & Crash Debugging     ");
    Serial.println("========================================================");
    Serial.println("Panduan Tombol & Uji Interaktif:");
    Serial.println(" -> Tekan tombol fisik di GPIO 18 untuk memicu Hardware Interrupt.");
    Serial.println(" -> Ketik 'c' pada Serial Monitor lalu Enter untuk Uji Crash (Level 2).");
    Serial.println(" -> Ketik 'r' pada Serial Monitor untuk mereset counter tombol.");
    Serial.println(" -> Ketik 'h' pada Serial Monitor untuk memunculkan menu bantuan.");
    Serial.println("========================================================\n");
}

// ============================================================================
// INISIALISASI SISTEM (SETUP)
// ============================================================================
void setup() {
    // Inisialisasi komunikasi serial dengan baud rate 115200 bps
    Serial.begin(115200);
    delay(1000); // Jeda stabilisasi serial

    print_banner();

    // Konfigurasi pin LED sebagai output
    pinMode(ONBOARD_LED_PIN, OUTPUT);
    digitalWrite(ONBOARD_LED_PIN, LOW);

    // Konfigurasi pin tombol sebagai INPUT dengan PULL-UP INTERNAL
    // Kondisi normal: HIGH (3.3V). Saat tombol ditekan ke GND: LOW (0V).
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Pasang Hardware Interrupt pada BUTTON_PIN:
    // Mode FALLING: terpicu saat tegangan berpindah dari HIGH (3.3V) ke LOW (0V)
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isr_button_pressed, FALLING);

    Serial.println("[SETUP OK]: Hardware Interrupt aktif pada GPIO 18 (Mode: FALLING).");
    Serial.println("Menunggu aksi interupsi fisik atau perintah serial...\n");
}

// ============================================================================
// LOOP UTAMA (NON-BLOCKING DEFERRED PROCESSING)
// ============================================================================
void loop() {
    // 1. Memeriksa apakah ada event interupsi yang tertunda dari ISR
    if (button_event_flag) {
        // Reset flag segera agar tidak diproses berulang
        button_event_flag = false;

        // Toggle status LED onboard sebagai konfirmasi visual
        digitalWrite(ONBOARD_LED_PIN, !digitalRead(ONBOARD_LED_PIN));

        // Cetak informasi penekanan tombol secara aman di luar ISR
        Serial.print("[INTERRUPT DETECTED] Tombol Ditekan! Total Tekanan: ");
        Serial.print(button_press_count);
        Serial.print(" kali | Waktu: ");
        Serial.print(last_interrupt_time);
        Serial.println(" ms");
    }

    // 2. Memeriksa perintah diagnostik dari Serial Monitor
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        if (cmd == 'c' || cmd == 'C') {
            // Jalankan simulasi crash terkontrol (Tugas Level 2)
            trigger_controlled_crash();
        } else if (cmd == 'r' || cmd == 'R') {
            button_press_count = 0;
            Serial.println("[RESET]: Counter tombol berhasil direset ke 0.");
        } else if (cmd == 'h' || cmd == 'H') {
            print_banner();
        }
    }

    // CPU tidak terhalang (bebas delay!), dapat disisipi tugas non-blocking lain.
}
