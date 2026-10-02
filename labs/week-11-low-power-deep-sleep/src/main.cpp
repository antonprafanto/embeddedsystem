/**
 * =============================================================================
 * PRAKTIKUM SISTEM TERTANAM - TEKNIK ELEKTRO
 * MODUL 11: DESAIN SISTEM BERTENAGA BATERAI (LOW-POWER & DEEP SLEEP)
 * =============================================================================
 * Deskripsi:
 * Program ini mendemonstrasikan teknik optimasi konsumsi daya pada ESP32
 * untuk aplikasi IoT bertenaga baterai (Battery-Powered Edge Devices):
 *   1. Analisis 4 Mode Daya: Active, Modem-Sleep, Light-Sleep, dan Deep-Sleep.
 *   2. Memori Persisten RTC (RTC_DATA_ATTR): Menyimpan variabel melintasi
 *      siklus Deep Sleep tanpa membebani / mengikis siklus Flash NVS.
 *   3. Sumber Pembangkit Bangun (Wake-up Sources):
 *      - Timer Wake-up (Bangun periodik via RTC Timer).
 *      - External Wake-up EXT0 (Bangun berbasis tombol di pin RTC GPIO 33).
 *   4. Demonstrasi Light-Sleep vs Deep-Sleep (Reboot vs Lanjut Eksekusi).
 *   5. Kalkulator masa pakai baterai Li-Ion 18650 berbasis Duty Cycle.
 *
 * Konfigurasi Pinout:
 *   - GPIO 33 : Tombol External Wake-up (RTC_GPIO8, Active LOW via INPUT_PULLUP)
 *   - GPIO 22 : LED Indikator Status Aktif (Menyala saat Awake, Mati saat Sleep)
 *
 * Komunikasi:
 *   - Serial Monitor: 115200 Baud (Newline: Both NL & CR)
 * =============================================================================
 */

#include <Arduino.h>
#include <esp_sleep.h>
#include <esp_timer.h>
#include <driver/rtc_io.h>

// -----------------------------------------------------------------------------
// DEFINISI PIN PERANGKAT KERAS (HARDWARE PINOUT)
// -----------------------------------------------------------------------------
// Catatan Enjiniring: Untuk membangunkan ESP32 dari Deep Sleep menggunakan EXT0,
// pin yang dipilih WAJIB merupakan pin RTC GPIO (misal: GPIO 33 / RTC_GPIO8).
#define PIN_WAKEUP_BUTTON  GPIO_NUM_33 // Pin RTC_GPIO8 untuk External Wake-up EXT0
#define PIN_LED_STATUS     22          // LED penanda ESP32 dalam kondisi Awake

// Durasi default timer sleep dalam mikrodetik (5 detik)
#define US_TO_S_FACTOR     1000000ULL  // Faktor konversi mikrodetik ke detik
#define TIME_TO_SLEEP_S    5           // ESP32 akan tidur selama 5 detik

// -----------------------------------------------------------------------------
// VARIABEL PERSISTEN DI MEMORI RTC (RTC SLOW SRAM 8 KB)
// -----------------------------------------------------------------------------
// Atribut RTC_DATA_ATTR menempatkan variabel pada memori internal RTC Slow SRAM.
// Memori ini tetap dialiri daya (~10 uA) selama Deep Sleep sehingga nilainya
// TIDAK AKAN HILANG saat CPU utama mati, tanpa perlu menulis Flash NVS!
RTC_DATA_ATTR static uint32_t s_bootCount = 0;
RTC_DATA_ATTR static float    s_lastTemperature = 26.5f;
RTC_DATA_ATTR static uint32_t s_totalSleepCycles = 0;

// -----------------------------------------------------------------------------
// FUNGSI ANALISIS SUMBER PENYEBAB BANGUN (WAKE-UP REASON)
// -----------------------------------------------------------------------------

void print_wakeup_reason() {
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    Serial.println(F("\n========================================================"));
    Serial.println(F("[DIAGNOSTIK WAKE-UP]: Menganalisis Pemicu Bangun Sistem"));
    Serial.println(F("========================================================"));

    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println(F(" • Penyebab Bangun : EXTERNAL WAKE-UP (EXT0 via RTC_GPIO)"));
            Serial.printf( " • Keterangan      : Tombol fisik pada GPIO %d ditekan praktikan!\n", PIN_WAKEUP_BUTTON);
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println(F(" • Penyebab Bangun : EXTERNAL WAKE-UP (EXT1 Multi-Pin Mask)"));
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println(F(" • Penyebab Bangun : INTERNAL RTC TIMER WAKE-UP"));
            Serial.printf( " • Keterangan      : Waktu tidur terjadwal (%d detik) telah habis.\n", TIME_TO_SLEEP_S);
            s_totalSleepCycles++;
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            Serial.println(F(" • Penyebab Bangun : SENSOR SENTUH (Touchpad RTC)"));
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            Serial.println(F(" • Penyebab Bangun : ULP COPROCESSOR TRIGGER"));
            break;
        default:
            Serial.println(F(" • Penyebab Bangun : COLD BOOT / POWER-ON RESET"));
            Serial.println(F(" • Keterangan      : Board baru saja dinyalakan atau tombol EN ditekan."));
            break;
    }

    // Tampilkan data persisten yang tersimpan di RTC Slow Memory
    Serial.printf(" • Akumulasi Boot  : Siklus ke-%u\n", s_bootCount);
    Serial.printf(" • Data Sensor RTC : %.2f °C (Tersimpan aman di RTC Slow SRAM)\n", s_lastTemperature);
    Serial.printf(" • Siklus Tidur    : %u kali tertidur lelap\n", s_totalSleepCycles);
    Serial.println(F("========================================================"));
}

// -----------------------------------------------------------------------------
// FUNGSI DEMONSTRASI MODE SLEEP
// -----------------------------------------------------------------------------

/**
 * @brief Masuk ke mode Deep Sleep dengan pemicu Timer RTC
 */
void enter_deep_sleep_timer(uint32_t seconds) {
    Serial.printf("\n[DEEP SLEEP]: Menyiapkan timer bangun dalam %u detik...\n", seconds);
    Serial.println(F("[DEEP SLEEP]: Mematikan Core 0, Core 1, Wi-Fi, dan Main RAM..."));
    Serial.println(F("[DEEP SLEEP]: Mengalirkan daya hanya ke RTC Subsystem (~10 uA)."));
    Serial.println(F("Zzz... ESP32 tidur lelap sekarang!"));
    Serial.flush(); // Pastikan seluruh teks terkirim ke Serial sebelum CPU mati!

    // Matikan LED penanda aktif
    digitalWrite(PIN_LED_STATUS, LOW);

    // 1. Konfigurasi sumber bangun: RTC Timer
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * US_TO_S_FACTOR);

    // 2. Mulai Deep Sleep (Eksekusi berhenti total di sini)
    esp_deep_sleep_start();
}

/**
 * @brief Masuk ke mode Deep Sleep dengan pemicu Tombol Eksternal (EXT0)
 */
void enter_deep_sleep_ext0() {
    Serial.println(F("\n[DEEP SLEEP EXT0]: Menyiapkan wake-up berbasis tombol GPIO 33..."));
    Serial.println(F("[DEEP SLEEP EXT0]: ESP32 akan tidur tanpa batas waktu sampai tombol ditekan!"));
    Serial.println(F("Tekan tombol di GPIO 33 untuk membangunkan ESP32. Zzz..."));
    Serial.flush();

    digitalWrite(PIN_LED_STATUS, LOW);

    // 1. Aktifkan pull-up internal pada pin RTC agar tidak mengambang (floating)
    rtc_gpio_pullup_en(PIN_WAKEUP_BUTTON);
    rtc_gpio_pulldown_dis(PIN_WAKEUP_BUTTON);

    // 2. Konfigurasi EXT0: Bangun saat pin ditarik ke LOW (logika 0 saat ditekan)
    esp_sleep_enable_ext0_wakeup(PIN_WAKEUP_BUTTON, 0);

    // 3. Masuk ke mode Deep Sleep
    esp_deep_sleep_start();
}

/**
 * @brief Masuk ke mode Light Sleep (CPU clock dihentikan, RAM tetap utuh)
 */
void enter_light_sleep(uint32_t seconds) {
    Serial.printf("\n[LIGHT SLEEP]: Masuk ke mode Light-Sleep selama %u detik...\n", seconds);
    Serial.println(F("[LIGHT SLEEP]: RAM tetap hidup (~0.8 mA), program TIDAK REBOOT!"));
    Serial.flush();

    digitalWrite(PIN_LED_STATUS, LOW);

    // Konfigurasi timer bangun
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * US_TO_S_FACTOR);

    int64_t tBefore = esp_timer_get_time();

    // Masuk ke Light Sleep (CPU membeku di sini, tetapi RAM tetap terjaga)
    esp_light_sleep_start();

    // Begitu waktu habis, eksekusi LANGSUNG BERLANJUT ke baris di bawah ini!
    int64_t tAfter = esp_timer_get_time();
    digitalWrite(PIN_LED_STATUS, HIGH);

    Serial.println(F("\n✅ [LIGHT SLEEP BANGKIT]: ESP32 terbangun dan melanjutkan baris kode!"));
    Serial.printf( " • Durasi Terlelap : %lld ms (Tanpa reboot sistem!)\n", (tAfter - tBefore) / 1000);
}

// -----------------------------------------------------------------------------
// FUNGSI INFORMASI & KALKULATOR ENJINIRING
// -----------------------------------------------------------------------------

void print_power_modes_info() {
    Serial.println();
    Serial.println(F("=========================================================================="));
    Serial.println(F("⚡ PROFIL 4 MODE KONSUMSI DAYA ESP32 (XTENSA DUAL-CORE)"));
    Serial.println(F("=========================================================================="));
    Serial.println(F("| Mode Daya     | Arus Tipikal | Status CPU   | Status RAM   | Status Radio RF  |"));
    Serial.println(F("|---------------|--------------|--------------|--------------|------------------|"));
    Serial.println(F("| Active Mode   | 80 - 240 mA  | ON (240 MHz) | ON (SRAM)    | Wi-Fi / BT Aktif |"));
    Serial.println(F("| Modem-Sleep   | 20 - 30 mA   | ON (240 MHz) | ON (SRAM)    | Radio RF OFF     |"));
    Serial.println(F("| Light-Sleep   | ~0.8 mA      | Clock Gated  | ON (Retained)| Radio RF OFF     |"));
    Serial.println(F("| Deep-Sleep    | ~10 - 15 uA  | POWER OFF    | OFF (RTC ON) | Seluruh Chip OFF |"));
    Serial.println(F("=========================================================================="));
    Serial.println(F("💡 Pengetahuan Enjiniring:"));
    Serial.println(F("   1. Deep-Sleep menghemat daya hingga 99.99% dibanding Active Mode!"));
    Serial.println(F("   2. Dalam Deep-Sleep, seluruh memori SRAM utama mati. Variabel biasa akan hilang."));
    Serial.println(F("   3. Gunakan atribut RTC_DATA_ATTR agar variabel selamat di dalam RTC Slow Memory."));
    Serial.println(F("--------------------------------------------------------------------------"));
}

void print_rtc_memory_status() {
    Serial.println();
    Serial.println(F("--- [STATUS MEMORI PERSISTEN RTC SLOW SRAM] ---"));
    Serial.printf(" - Variabel s_bootCount        : %u (Disimpan di RTC_DATA_ATTR)\n", s_bootCount);
    Serial.printf(" - Variabel s_lastTemperature  : %.2f °C\n", s_lastTemperature);
    Serial.printf(" - Variabel s_totalSleepCycles : %u siklus tidur\n", s_totalSleepCycles);
    Serial.println(F("------------------------------------------------"));
    Serial.println(F("💡 Mengapa RTC_DATA_ATTR Lebih Unggul dari Flash NVS untuk Data Sensor?"));
    Serial.println(F("   • Flash NVS memiliki batas siklus tulis (~100.000 kali). Jika sensor mencatat"));
    Serial.println(F("     tiap 1 menit ke NVS, chip Flash memori akan rusak dalam waktu < 2.5 bulan!"));
    Serial.println(F("   • RTC Slow SRAM adalah RAM sejati: siklus baca/tulis tanpa batas (Unlimited),"));
    Serial.println(F("     latensi nol, dan tidak mengikis umur chip fisik ESP32."));
    Serial.println(F("------------------------------------------------"));
}

void calculate_battery_lifetime(float capacity_mah, float i_active_ma, float t_active_s, float i_sleep_ua, float t_sleep_s) {
    float t_total = t_active_s + t_sleep_s;
    float duty_cycle = (t_active_s / t_total) * 100.0f;

    // Hitung rata-rata arus tertimbang (Weighted Average Current)
    float i_sleep_ma = i_sleep_ua / 1000.0f;
    float i_avg_ma = ((i_active_ma * t_active_s) + (i_sleep_ma * t_sleep_s)) / t_total;

    // Estimasi umur baterai (dengan efisiensi 90% karena self-discharge baterai)
    float lifetime_hours = (capacity_mah * 0.90f) / i_avg_ma;
    float lifetime_days = lifetime_hours / 24.0f;
    float lifetime_years = lifetime_days / 365.25f;

    Serial.println();
    Serial.println(F("=========================================================================="));
    Serial.println(F("🔋 HASIL ESTIMASI MASA PAKAI BATERAI LI-ION 18650"));
    Serial.println(F("=========================================================================="));
    Serial.printf(" • Kapasitas Baterai 18650  : %.0f mAh (Efisiensi Riil 90%% = %.0f mAh)\n", capacity_mah, capacity_mah * 0.90f);
    Serial.printf(" • Durasi Aktif (T_active)  : %.2f detik (Arus: %.1f mA)\n", t_active_s, i_active_ma);
    Serial.printf(" • Durasi Tidur (T_sleep)   : %.1f detik / %.2f menit (Arus: %.1f uA)\n", t_sleep_s, t_sleep_s / 60.0f, i_sleep_ua);
    Serial.printf(" • Duty Cycle Operasional   : %.3f %%\n", duty_cycle);
    Serial.printf(" • Rata-rata Arus (I_avg)   : %.4f mA (Hanya %.2f uA rata-rata!)\n", i_avg_ma, i_avg_ma * 1000.0f);
    Serial.println(F("--------------------------------------------------------------------------"));
    Serial.printf(" 👉 ESTIMASI MASA PAKAI     : %.1f Jam\n", lifetime_hours);
    Serial.printf(" 👉 SETARA DENGAN           : %.1f Hari (~ %.2f Tahun!)\n", lifetime_days, lifetime_years);
    Serial.println(F("=========================================================================="));
    Serial.println(F("💡 Kesimpulan Enjiniring:"));
    Serial.println(F("   Dengan menidurkan ESP32 selama 10 menit setelah membaca sensor dalam 0.1 detik,"));
    Serial.println(F("   satu buah baterai 18650 standar dapat menghidupi node IoT selama bertahun-tahun!"));
    Serial.println(F("--------------------------------------------------------------------------"));
}

void print_system_banner() {
    Serial.println();
    Serial.println(F("================================================================="));
    Serial.println(F("⚡ PRAKTIKUM SISTEM TERTANAM - MINGGU 11"));
    Serial.println(F("⚡ LOW-POWER DESIGN, DEEP-SLEEP & RTC PERSISTENT MEMORY"));
    Serial.println(F("================================================================="));
    Serial.println(F("Ketik karakter angka di bawah lalu tekan [Enter]:"));
    Serial.println(F(" [1] Info Profil 4 Mode Daya ESP32 (Active, Modem, Light, Deep)"));
    Serial.println(F(" [2] Status Memori RTC Slow SRAM (Variabel RTC_DATA_ATTR)"));
    Serial.println(F(" [3] Masuk Deep Sleep via Timer (Tidur 5 Detik lalu Reboot Bangun)"));
    Serial.println(F(" [4] Masuk Deep Sleep via External Button EXT0 (Pin GPIO 33)"));
    Serial.println(F(" [5] Masuk Light Sleep (Tidur 3 Detik Tanpa Reboot / RAM Retained)"));
    Serial.println(F(" [6] Kalkulator Interaktif Masa Pakai Baterai 18650 (Duty Cycle)"));
    Serial.println(F(" [s] Simulasi Pembacaan Sensor Suhu Baru (Update ke RTC Memory)"));
    Serial.println(F(" [h] Tampilkan Ulang Menu Bantuan"));
    Serial.println(F("================================================================="));
    Serial.print(F("EmbeddedSystem-W11 >> "));
}

// -----------------------------------------------------------------------------
// ARDUINO SETUP & LOOP
// -----------------------------------------------------------------------------

void setup() {
    // 1. Inisialisasi Serial Terminal
    Serial.begin(115200);
    delay(1000); // Waktu stabilisasi terminal serial

    // 2. Inisialisasi Pin Hardware
    pinMode(PIN_LED_STATUS, OUTPUT);
    digitalWrite(PIN_LED_STATUS, HIGH); // Nyalakan LED menandakan ESP32 sedang Awake!

    pinMode(PIN_WAKEUP_BUTTON, INPUT_PULLUP);

    // 3. Naikkan counter boot persisten
    s_bootCount++;

    // 4. Periksa dan tampilkan penyebab bangun
    print_wakeup_reason();

    // 5. Tampilkan Menu CLI
    print_system_banner();
}

void loop() {
    // Membaca perintah input dari Serial Monitor
    if (Serial.available() > 0) {
        char cmd = Serial.read();

        if (cmd == '\r' || cmd == '\n') {
            return;
        }

        switch (cmd) {
            case '1':
                print_power_modes_info();
                break;
            case '2':
                print_rtc_memory_status();
                break;
            case '3':
                enter_deep_sleep_timer(TIME_TO_SLEEP_S);
                break;
            case '4':
                enter_deep_sleep_ext0();
                break;
            case '5':
                enter_light_sleep(3);
                break;
            case '6':
                // Parameter: Kapasitas 2500 mAh, I_active 120 mA, T_active 0.1s, I_sleep 15 uA, T_sleep 600s (10 menit)
                calculate_battery_lifetime(2500.0f, 120.0f, 0.10f, 15.0f, 600.0f);
                break;
            case 's':
            case 'S':
                s_lastTemperature += 0.35f;
                if (s_lastTemperature > 38.0f) s_lastTemperature = 25.0f;
                Serial.printf("\n[SENSOR]: Membaca suhu baru: %.2f °C -> Disimpan ke RTC_DATA_ATTR!\n", s_lastTemperature);
                break;
            case 'h':
            case 'H':
                print_system_banner();
                break;
            default:
                Serial.printf("\nPerintah '%c' tidak dikenali. Ketik 'h' untuk bantuan.\n", cmd);
                break;
        }

        Serial.print(F("\nEmbeddedSystem-W11 >> "));
    }

    // Beri sedikit jeda agar CPU tidak 100% sibuk saat menunggu input serial
    delay(50);
}
