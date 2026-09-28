/**
 * ============================================================================
 * MODUL PRAKTIKUM SISTEM TERTANAM - MINGGU 3
 * Topik: Interfacing Beban Daya (Transistor Driver) & Periferal Analog (ADC1 & PWM)
 * ============================================================================
 * Mahasiswa: [Nama Lengkap Anda]
 * NIM      : [Nomor Induk Mahasiswa]
 * Kelas    : S1 Teknik Elektro
 * ============================================================================
 */

#include <Arduino.h>

// ============================================================================
// KONFIGURASI PIN PERANGKAT KERAS (HARDWARE PIN MAPPING)
// ============================================================================
// Aturan Emas ESP32: HANYA gunakan ADC1 (GPIO 32 - 39) untuk pembacaan analog!
// GPIO 34 adalah pin input-only pada ADC1 yang sangat stabil dan aman dari Wi-Fi.
#define POT_ADC_PIN         34   // Input sinyal analog potensiometer (ADC1_CH6)
#define DRIVER_PWM_PIN      19   // Output sinyal PWM ke Basis Transistor 2N2222
#define ONBOARD_LED_PIN      2   // LED indikator onboard standar ESP32 DevKit

// ============================================================================
// KONFIGURASI HARDWARE PWM (LEDC CONTROLLER)
// ============================================================================
#define PWM_CHANNEL          0   // Saluran timer LEDC (0 - 15)
#define PWM_FREQ          5000   // Frekuensi PWM: 5 kHz (ideal untuk motor DC / LED)
#define PWM_RESOLUTION       8   // Resolusi 8-bit: rentang nilai duty cycle 0 - 255
#define MAX_DUTY_CYCLE     255   // Nilai maksimum untuk resolusi 8-bit (2^8 - 1)

// ============================================================================
// VARIABEL SISTEM & TELEMETRI
// ============================================================================
uint32_t last_telemetry_time = 0;
const uint32_t TELEMETRY_INTERVAL_MS = 250; // Periode update serial: 4 kali per detik

bool auto_mode = true;           // True = kendali dari Potensiometer, False = manual Serial
uint16_t current_adc_raw = 0;    // Nilai mentah ADC 12-bit (0 - 4095)
float current_voltage = 0.0;     // Estimasi tegangan analog (0.0 - 3.3 Volt)
uint8_t current_duty = 0;        // Duty cycle PWM saat ini (0 - 255)

// ============================================================================
// FUNGSI LEVEL 1: FILTER MULTISAMPLING ADC1
// ============================================================================
/**
 * @brief Membaca nilai ADC dengan teknik multisampling (rata-rata 16 sampel).
 * Teknik ini sangat ampuh meredam derau tegangan (noise) dan fluktuasi sinyal analog.
 * 
 * @param pin Nomor GPIO pin ADC1 yang dibaca
 * @param samples Jumlah sampel rata-rata (default 16)
 * @return uint16_t Nilai rata-rata ADC (rentang 0 - 4095)
 */
uint16_t read_adc_multisampling(uint8_t pin, uint8_t samples = 16) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < samples; i++) {
        sum += analogRead(pin);
        delayMicroseconds(50); // Jeda singkat agar konverter ADC internal stabil
    }
    return (uint16_t)(sum / samples);
}

// ============================================================================
// FUNGSI LEVEL 2: PENULISAN HARDWARE PWM (LEDC)
// ============================================================================
/**
 * @brief Mengatur nilai duty cycle PWM ke pin penggerak transistor.
 * Dibuat kompatibel untuk ESP32 Arduino Core versi 2.x maupun versi 3.x.
 * 
 * @param duty Nilai duty cycle (0 s.d. 255)
 */
void write_pwm_duty(uint8_t duty) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(DRIVER_PWM_PIN, duty);
#else
    ledcWrite(PWM_CHANNEL, duty);
#endif
    current_duty = duty;
}

// ============================================================================
// FUNGSI BANTUAN UI SERIAL MONITOR
// ============================================================================
void print_banner() {
    Serial.println("\n========================================================");
    Serial.println("  PRAKTIKUM SISTEM TERTANAM - MINGGU 03                 ");
    Serial.println("  Transistor Driver, ADC1 Precision, & Hardware PWM     ");
    Serial.println("========================================================");
    Serial.println("Panduan Pengujian & Perintah Serial:");
    Serial.println(" [m] -> Ganti mode (Auto Potensiometer / Manual Serial)");
    Serial.println(" [0] - [9] -> Set duty cycle manual (0% s.d. 90%)");
    Serial.println(" [f] -> Set duty cycle maksimum penuh (100% / Full Power)");
    Serial.println(" [s] -> Tampilkan status telemetri saat ini");
    Serial.println(" [h] -> Cetak ulang menu bantuan ini");
    Serial.println("========================================================\n");
}

void print_telemetry() {
    float duty_percent = ((float)current_duty / MAX_DUTY_CYCLE) * 100.0;
    
    Serial.print("[TELEMETRI] Mode: ");
    Serial.print(auto_mode ? "OTOMATIS (POT) | " : "MANUAL SERIAL  | ");
    Serial.print("ADC Raw: ");
    Serial.print(current_adc_raw);
    Serial.print(" (12-bit) | Tegangan: ");
    Serial.print(current_voltage, 2);
    Serial.print(" V | PWM Duty: ");
    Serial.print(current_duty);
    Serial.print("/255 (");
    Serial.print(duty_percent, 1);
    Serial.println(" %)");
}

// ============================================================================
// INISIALISASI SISTEM (SETUP)
// ============================================================================
void setup() {
    Serial.begin(115200);
    delay(1000); // Waktu stabilisasi komunikasi serial

    print_banner();

    // 1. Konfigurasi Pin Indikator
    pinMode(ONBOARD_LED_PIN, OUTPUT);
    digitalWrite(ONBOARD_LED_PIN, LOW);

    // 2. Konfigurasi ADC1 ESP32
    // Resolusi 12-bit (nilai 0 - 4095)
    analogReadResolution(12);
    // Atenuasi 11 dB agar mampu membaca tegangan penuh hingga ~3.3 Volt
    analogSetAttenuation(ADC_11db);
    pinMode(POT_ADC_PIN, INPUT);

    // 3. Konfigurasi Hardware PWM (LEDC)
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
    // Sintaks ESP32 Arduino Core v3.x
    ledcAttach(DRIVER_PWM_PIN, PWM_FREQ, PWM_RESOLUTION);
#else
    // Sintaks ESP32 Arduino Core v2.x
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(DRIVER_PWM_PIN, PWM_CHANNEL);
#endif

    // Mulai dengan kondisi motor/beban mati (duty cycle = 0)
    write_pwm_duty(0);

    Serial.println("[SETUP OK]: ADC1 aktif pada GPIO 34 (12-bit, Atten 11dB).");
    Serial.println("[SETUP OK]: Hardware PWM LEDC aktif pada GPIO 19 (5 kHz, 8-bit).");
    Serial.println("[SISTEM AKTIF]: Memulai pembacaan analog...\n");
}

// ============================================================================
// LOOP UTAMA (NON-BLOCKING REAL-TIME CONTROL)
// ============================================================================
void loop() {
    // ------------------------------------------------------------------------
    // TAHAP 1: PEMBACAAN SENSOR ANALOG & PENGENDALIAN OTOMATIS
    // ------------------------------------------------------------------------
    if (auto_mode) {
        // Baca nilai ADC potensiometer dengan filter rata-rata 16 sampel
        current_adc_raw = read_adc_multisampling(POT_ADC_PIN, 16);

        // Konversi nilai ADC 12-bit ke estimasi tegangan fisik (0.0V - 3.3V)
        current_voltage = (current_adc_raw / 4095.0) * 3.3;

        // Pemetaan nilai ADC 12-bit (0-4095) ke duty cycle PWM 8-bit (0-255)
        uint8_t target_duty = map(current_adc_raw, 0, 4095, 0, 255);

        // Terapkan sinyal PWM ke basis transistor
        write_pwm_duty(target_duty);

        // Indikator visual: nyalakan LED onboard jika duty cycle > 10%
        digitalWrite(ONBOARD_LED_PIN, target_duty > 25 ? HIGH : LOW);
    }

    // ------------------------------------------------------------------------
    // TAHAP 2: PENGIRIMAN LOG TELEMETRI PERIODIK (NON-BLOCKING)
    // ------------------------------------------------------------------------
    uint32_t current_time = millis();
    if (current_time - last_telemetry_time >= TELEMETRY_INTERVAL_MS) {
        last_telemetry_time = current_time;
        print_telemetry();
    }

    // ------------------------------------------------------------------------
    // TAHAP 3: MENERIMA PERINTAH INTERAKTIF DARI SERIAL MONITOR
    // ------------------------------------------------------------------------
    if (Serial.available() > 0) {
        char cmd = Serial.read();

        // Mengabaikan karakter baris baru (\r atau \n)
        if (cmd == '\r' || cmd == '\n') return;

        if (cmd == 'm' || cmd == 'M') {
            auto_mode = !auto_mode;
            Serial.print("\n>>> [MODE SWITCH]: Beralih ke Mode ");
            Serial.println(auto_mode ? "OTOMATIS (Potensiometer)" : "MANUAL (Serial Control)");
            if (!auto_mode) {
                Serial.println(">>> Ketik angka '0' s.d. '9' atau 'f' untuk mengatur kecepatan motor!");
            }
        } 
        else if (!auto_mode && cmd >= '0' && cmd <= '9') {
            // Ubah karakter '0' - '9' menjadi persentase 0% - 90%
            uint8_t level = cmd - '0';
            uint8_t duty = (level * 255) / 10;
            write_pwm_duty(duty);
            Serial.print(">>> [MANUAL SET]: Duty Cycle disetel ke ");
            Serial.print(level * 10);
            Serial.print("% (Nilai Register PWM: ");
            Serial.print(duty);
            Serial.println(")");
        } 
        else if (!auto_mode && (cmd == 'f' || cmd == 'F')) {
            write_pwm_duty(255);
            Serial.println(">>> [MANUAL SET]: FULL POWER! Duty Cycle 100% (255/255)");
        } 
        else if (cmd == 's' || cmd == 'S') {
            Serial.println("\n--- Status Sistem Saat Ini ---");
            print_telemetry();
        } 
        else if (cmd == 'h' || cmd == 'H') {
            print_banner();
        }
    }
}
