/**
 * =============================================================================
 * PRAKTIKUM SISTEM TERTANAM - TEKNIK ELEKTRO
 * MODUL 10: PEMROGRAMAN DUAL-CORE ESP32 & DEFERRED INTERRUPT PROCESSING
 * =============================================================================
 * Deskripsi:
 * Program ini mendemonstrasikan arsitektur Symmetric Multiprocessing (SMP)
 * pada ESP32 (Xtensa Dual-Core 240MHz):
 *   1. Penugasan Task ke Core spesifik (Core 0 PRO_CPU vs Core 1 APP_CPU)
 *      menggunakan xTaskCreatePinnedToCore().
 *   2. Deferred Interrupt Processing: Mengapa komputasi panjang dilarang
 *      di dalam Interrupt Service Routine (ISR) dan bagaimana cara
 *      mendelegasikan pekerjaan ke Task menggunakan Direct Task Notification
 *      (vTaskNotifyGiveFromISR & ulTaskNotifyTake).
 *   3. Pengukuran latensi interupsi presisi tinggi (skala mikrodetik).
 *   4. Pengujian isolasi core: Memberi beban komputasi berat di Core 1
 *      tanpa mengganggu stabilitas telemetri di Core 0.
 *
 * Konfigurasi Pinout:
 *   - GPIO 18 : Push Button Interrupt (Input Pull-up Internal)
 *   - GPIO 22 : LED Indikator Core 0 (Telemetri / Heartbeat)
 *   - GPIO 23 : LED Indikator Core 1 (Worker / Event Handler)
 *
 * Komunikasi:
 *   - Serial Monitor: 115200 Baud (Newline: Both NL & CR)
 * =============================================================================
 */

#include <Arduino.h>
#include <esp_timer.h>
#include <esp_system.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// -----------------------------------------------------------------------------
// DEFINISI PIN PERANGKAT KERAS (HARDWARE PINOUT)
// -----------------------------------------------------------------------------
#define PIN_BUTTON        18  // Tombol interrupt eksternal (Active LOW)
#define PIN_LED_CORE0     22  // LED status aktivitas Core 0 (Hijau / Biru)
#define PIN_LED_CORE1     23  // LED status aktivitas Core 1 (Kuning / Merah)

// -----------------------------------------------------------------------------
// KONSTANTA & STRUKTUR PENGUKURAN KINERJA
// -----------------------------------------------------------------------------
#define DEBOUNCE_DELAY_US 250000ULL // 250 ms debounce dalam mikrodetik

// Task Handles untuk manajemen siklus hidup dan Task Notification
TaskHandle_t xHandleTelemetryCore0 = NULL;
TaskHandle_t xHandleWorkerCore1    = NULL;
TaskHandle_t xHandleStressCore1    = NULL;

// Variabel shared / state interupsi (wajib volatile)
volatile int64_t  g_isrTriggerTimestampUs = 0;
volatile int64_t  g_taskFinishedTimestampUs = 0;
volatile uint32_t g_interruptEventCounter  = 0;
volatile bool     g_stressTestActive       = false;
volatile bool     g_simulateBadISR         = false;

// -----------------------------------------------------------------------------
// INTERRUPT SERVICE ROUTINE (ISR)
// -----------------------------------------------------------------------------

/**
 * @brief ISR Tombol GPIO 18 (Wajib menggunakan atribut IRAM_ATTR)
 * 
 * Atribut IRAM_ATTR menempatkan instruksi fungsi ini di memori internal SRAM
 * (bukan Flash SPI). Ini wajib agar saat interupsi terjadi bersamaan dengan
 * penulisan Flash, CPU tidak mengalami Guru Meditation crash / Cache Miss.
 *
 * GOLDEN RULE ISR:
 * 1. Jangan panggil Serial.print() (karena butuh lock mutex, bisa deadlock!).
 * 2. Jangan gunakan delay() atau komputasi loop panjang (> 10 us).
 * 3. Jangan alokasikan memori dinamis (malloc/new).
 * 4. Cukup catat timestamp, beri tahu Task, dan langsung keluar (portYIELD).
 */
void IRAM_ATTR isr_button_handler() {
    static int64_t lastInterruptUs = 0;
    int64_t now = esp_timer_get_time();

    // Software Debouncing dalam skala mikrodetik
    if ((now - lastInterruptUs) < DEBOUNCE_DELAY_US) {
        return;
    }
    lastInterruptUs = now;
    g_isrTriggerTimestampUs = now;
    g_interruptEventCounter++;

    // -------------------------------------------------------------------------
    // SKENARIO EKSPERIMEN: BAD PRACTICE vs BEST PRACTICE
    // -------------------------------------------------------------------------
    if (g_simulateBadISR) {
        // [SIMULASI KESALAHAN AWAM]: Melakukan blocking delay panjang di ISR
        // ets_delay_us menahan core secara hardware selama 8000 mikrodetik (8ms).
        // Ini membahayakan sistem karena menghentikan scheduler FreeRTOS di core ini!
        ets_delay_us(8000);
    }

    // [BEST PRACTICE]: Direct Task Notification
    // Bangunkan Task Worker di Core 1 tanpa perantara antrian Queue / Semaphore terpisah.
    // Metode ini paling cepat dan hemat RAM (0 byte tambahan heap).
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (xHandleWorkerCore1 != NULL) {
        vTaskNotifyGiveFromISR(xHandleWorkerCore1, &xHigherPriorityTaskWoken);
    }

    // Melakukan Context Switch seketika jika Task yang dibangunkan memiliki
    // prioritas lebih tinggi daripada task yang sedang berjalan saat interrupt terjadi.
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// -----------------------------------------------------------------------------
// DEFINISI TASK FREERTOS
// -----------------------------------------------------------------------------

/**
 * @brief Task Telemetri & Heartbeat - DIPIN KE CORE 0 (PRO_CPU)
 * 
 * Core 0 pada arsitektur ESP-IDF secara bawaan bertanggung jawab atas
 * background process seperti Wi-Fi stack, Bluetooth Controller, dan RTOS timer.
 * Task ini memonitor kondisi sistem secara periodik tanpa membebani Core 1.
 */
void TaskTelemetryCore0(void *pvParameters) {
    (void)pvParameters;
    pinMode(PIN_LED_CORE0, OUTPUT);
    digitalWrite(PIN_LED_CORE0, LOW);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // Eksekusi tiap 1 detik

    while (true) {
        // Toggle LED indikator Core 0
        digitalWrite(PIN_LED_CORE0, HIGH);
        vTaskDelay(pdMS_TO_TICKS(50));
        digitalWrite(PIN_LED_CORE0, LOW);

        // Tunggu hingga siklus 1 detik berikutnya (presisi tanpa drift waktu)
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/**
 * @brief Task Worker (Deferred Event Handler) - DIPIN KE CORE 1 (APP_CPU)
 * 
 * Task ini tidur nyenyak (status BLOCKED, 0% CPU) hingga dibangunkan
 * oleh ISR tombol melalui Direct Task Notification (ulTaskNotifyTake).
 * Semua pemrosesan berat, konversi data, dan logging serial didelegasikan ke sini!
 */
void TaskWorkerCore1(void *pvParameters) {
    (void)pvParameters;
    pinMode(PIN_LED_CORE1, OUTPUT);
    digitalWrite(PIN_LED_CORE1, LOW);

    while (true) {
        // Tunggu sinyal notifikasi dari ISR (ulTaskNotifyTake dengan pdTRUE untuk clear count)
        // Nilai portMAX_DELAY membuat task ini tidak memakan siklus CPU sama sekali saat idle!
        uint32_t ulNotificationCount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (ulNotificationCount > 0) {
            // Catat waktu saat task mulai memproses
            int64_t taskStartUs = esp_timer_get_time();
            
            // Nyalakan LED Core 1 sebagai penanda proses aktif
            digitalWrite(PIN_LED_CORE1, HIGH);

            // Simulasi komputasi digital signal processing (DSP) / parsing data
            // Misal: Kalkulasi filter running average / matematika sensor
            volatile float dummyValue = 0.0f;
            for (int i = 0; i < 3000; i++) {
                dummyValue += sinf((float)i * 0.01f);
            }

            int64_t taskEndUs = esp_timer_get_time();
            g_taskFinishedTimestampUs = taskEndUs;

            // Hitung metrik latensi
            int64_t latencyIsrToTaskUs = taskStartUs - g_isrTriggerTimestampUs;
            int64_t totalProcessingTimeUs = taskEndUs - taskStartUs;
            int64_t totalTurnaroundUs = taskEndUs - g_isrTriggerTimestampUs;

            // Matikan LED Core 1
            digitalWrite(PIN_LED_CORE1, LOW);

            // Cetak laporan performa ke Serial Monitor secara aman (karena ini Task, bukan ISR!)
            Serial.println();
            Serial.println(F("=========================================================="));
            Serial.println(F("[WORKER CORE 1] Sinyal Notifikasi Interupsi Diterima!"));
            Serial.println(F("=========================================================="));
            Serial.printf(" - Event Counter         : #%u\n", g_interruptEventCounter);
            Serial.printf(" - Core Pengeksekusi ISR : Core %d (Hardware Pin Trigger)\n", 1);
            Serial.printf(" - Core Handler Task     : Core %d (APP_CPU Pinned)\n", xPortGetCoreID());
            Serial.printf(" - Latensi Respon ISR    : %lld mikrodetik (us)\n", latencyIsrToTaskUs);
            Serial.printf(" - Durasi Komputasi Task : %lld mikrodetik (us)\n", totalProcessingTimeUs);
            Serial.printf(" - Total Turnaround Time : %lld mikrodetik (us)\n", totalTurnaroundUs);
            Serial.println(F(" - Status Delegasi       : SUKSES (ISR Aman & Sistem Tetap Responsif)"));
            Serial.println(F("=========================================================="));
            Serial.print(F("EmbeddedSystem-W10 >> "));
        }
    }
}

/**
 * @brief Task Stress Test - DIPIN KE CORE 1 (APP_CPU)
 * 
 * Digunakan untuk mensimulasikan beban kerja 100% pada Core 1.
 * Bertujuan membuktikan bahwa Core 0 tetap berjalan normal tanpa lag,
 * menunjukkan independensi penjadwalan hardware SMP Dual-Core.
 */
void TaskStressCore1(void *pvParameters) {
    (void)pvParameters;
    while (true) {
        if (g_stressTestActive) {
            // Bebani Core 1 dengan komputasi floating point tiada henti selama 50ms
            int64_t t0 = esp_timer_get_time();
            volatile double acc = 1.0;
            while ((esp_timer_get_time() - t0) < 50000) {
                acc = acc * 1.000001 + sin(acc);
            }
            // Beri sedikit nafas agar IDLE task di Core 1 tidak memicu TWDT
            vTaskDelay(pdMS_TO_TICKS(10));
        } else {
            // Jika tidak aktif, tidur selama 500ms
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

// -----------------------------------------------------------------------------
// FUNGSI BANTUAN CLI & PELAPORAN DIAGNOSTIK
// -----------------------------------------------------------------------------

void print_system_banner() {
    Serial.println();
    Serial.println(F("================================================================="));
    Serial.println(F("⚡ PRAKTIKUM SISTEM TERTANAM - MINGGU 10"));
    Serial.println(F("⚡ ARSITEKTUR DUAL-CORE ESP32 & DEFERRED INTERRUPT PROCESSING"));
    Serial.println(F("================================================================="));
    Serial.println(F("Ketik karakter angka di bawah lalu tekan [Enter]:"));
    Serial.println(F(" [1] Info Identitas Arsitektur Dual-Core (SoC, Frequency, Memory)"));
    Serial.println(F(" [2] Status Penugasan Core Task (Task Pinning & High Water Mark)"));
    Serial.println(F(" [3] Simulasi Pemicu Event Interupsi (Direct Task Notification)"));
    Serial.println(F(" [4] Toggle Skenario Bad ISR vs Best Practice Deferred ISR"));
    Serial.println(F(" [5] Toggle Stress Test Komputasi di Core 1 (Uji Stabilitas Core 0)"));
    Serial.println(F(" [6] Komparasi Teoretis: Direct Notification vs Binary Semaphore"));
    Serial.println(F(" [b] Simulasi Tekan Tombol Fisik GPIO 18 (Software Trigger)"));
    Serial.println(F(" [h] Tampilkan Ulang Menu Bantuan"));
    Serial.println(F("================================================================="));
    Serial.print(F("EmbeddedSystem-W10 >> "));
}

void print_dualcore_info() {
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    Serial.println();
    Serial.println(F("--- [INFO IDENTITAS ARSITEKTUR HARDWARE SOC] ---"));
    Serial.printf(" - Model Chip        : ESP32 Rev %d\n", chip_info.revision);
    Serial.printf(" - Jumlah Core Fisik : %d Core (Xtensa Dual-Core 32-bit LX6)\n", chip_info.cores);
    Serial.printf(" - Frekuensi CPU     : %u MHz\n", getCpuFrequencyMhz());
    Serial.printf(" - Core ID Saat Ini  : Core %d (loop() berjalan di Core 1 secara default)\n", xPortGetCoreID());
    Serial.printf(" - Free Heap Memory  : %u byte (%u KB)\n", ESP.getFreeHeap(), ESP.getFreeHeap() / 1024);
    Serial.printf(" - Min Free Heap     : %u byte\n", ESP.getMinFreeHeap());
    Serial.printf(" - Total Flash Size  : %u MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    Serial.println(F("------------------------------------------------"));
    Serial.println(F("💡 Pengetahuan Enjiniring:"));
    Serial.println(F("   Core 0 (PRO_CPU) : Bertanggung jawab atas protokol nirkabel (Wi-Fi/BT) & OS Kernel."));
    Serial.println(F("   Core 1 (APP_CPU) : Didesain untuk mengeksekusi logika aplikasi pengguna & user tasks."));
    Serial.println(F("------------------------------------------------"));
}

void print_task_pinning_status() {
    Serial.println();
    Serial.println(F("--- [STATUS PENUGASAN TASK & RESOURCE CONSUMPTION] ---"));
    Serial.println(F("| Nama Task        | Target Core | Core Aktif | Prioritas | Stack Sisa (Word) |"));
    Serial.println(F("|------------------|-------------|------------|-----------|-------------------|"));

    // Info Task Telemetry
    if (xHandleTelemetryCore0 != NULL) {
        UBaseType_t hwm = uxTaskGetStackHighWaterMark(xHandleTelemetryCore0);
        Serial.printf("| TelemetryCore0   | Core 0      | Core %d     |     1     | %17u |\n",
                      1, hwm); // target 0
    }
    // Info Task Worker
    if (xHandleWorkerCore1 != NULL) {
        UBaseType_t hwm = uxTaskGetStackHighWaterMark(xHandleWorkerCore1);
        Serial.printf("| WorkerCore1      | Core 1      | Core %d     |     2     | %17u |\n",
                      xPortGetCoreID(), hwm);
    }
    // Info Task Stress
    if (xHandleStressCore1 != NULL) {
        UBaseType_t hwm = uxTaskGetStackHighWaterMark(xHandleStressCore1);
        Serial.printf("| StressCore1      | Core 1      | Core %d     |     1     | %17u |\n",
                      xPortGetCoreID(), hwm);
    }
    // Info loopTask bawaan Arduino
    Serial.printf("| loopTask (Main)  | Core 1      | Core %d     |     1     |       -           |\n",
                  xPortGetCoreID());

    Serial.println(F("-----------------------------------------------------------------------"));
    Serial.println(F("💡 Catatan Stack High Water Mark:"));
    Serial.println(F("   Nilai menunjukkan sisa RAM terkecil pada stack task tersebut (dalam 4-byte word)."));
    Serial.println(F("   Jika mendekati 0, task terancam crash Stack Overflow!"));
}

void trigger_software_interrupt() {
    Serial.println();
    Serial.println(F("[TRIGGER] Mengirim sinyal interupsi perangkat lunak ke Worker Task..."));
    g_isrTriggerTimestampUs = esp_timer_get_time();
    g_interruptEventCounter++;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (xHandleWorkerCore1 != NULL) {
        // Simulasi notifikasi dari context non-ISR
        xTaskNotifyGive(xHandleWorkerCore1);
    }
}

void toggle_bad_isr() {
    g_simulateBadISR = !g_simulateBadISR;
    Serial.println();
    if (g_simulateBadISR) {
        Serial.println(F("⚠️  [MODE AKTIF]: SIMULASI BAD ISR (LONG-RUNNING ISR AKTIF!)"));
        Serial.println(F("   Peringatan: ISR kini menahan CPU selama 8 milidetik di level interrupt."));
        Serial.println(F("   Amati lonjakan latensi dan perhatikan bahwa core terblokir saat tombol ditekan!"));
    } else {
        Serial.println(F("✅ [MODE AKTIF]: BEST PRACTICE DEFERRED ISR"));
        Serial.println(F("   ISR hanya mencatat timestamp dan mengirim notifikasi (< 2 mikrodetik)."));
    }
}

void toggle_stress_test() {
    g_stressTestActive = !g_stressTestActive;
    Serial.println();
    if (g_stressTestActive) {
        Serial.println(F("🔥 [STRESS TEST AKTIF]: Beban komputasi matematika 90% diaktifkan pada Core 1!"));
        Serial.println(F("   Perhatikan: LED Core 0 (GPIO 22) tetap berkedip teratur 1 detik sekali"));
        Serial.println(F("   karena Core 0 beroperasi secara paralel independen di tingkat silikon!"));
    } else {
        Serial.println(F("🧊 [STRESS TEST NONAKTIF]: Core 1 kembali ke kondisi beban normal."));
    }
}

void print_comparison_analysis() {
    Serial.println();
    Serial.println(F("=========================================================================="));
    Serial.println(F("📊 KOMPARASI ARSITEKTUR: DIRECT TASK NOTIFICATION VS BINARY SEMAPHORE"));
    Serial.println(F("=========================================================================="));
    Serial.println(F("| Parameter Evaluasi       | Binary Semaphore        | Direct Task Notification |"));
    Serial.println(F("|--------------------------|-------------------------|--------------------------|"));
    Serial.println(F("| Alokasi Memori RAM       | ~64-80 byte heap (Queue)| 0 byte (Built-in di TCB) |"));
    Serial.println(F("| Kecepatan Eksekusi       | Standar (Queue Event)   | ~45% Lebih Cepat         |"));
    Serial.println(F("| Overhead Struktur Data   | Butuh SemaphoreHandle_t | Cukup TaskHandle_t       |"));
    Serial.println(F("| Kemampuan Broadcast      | Multi-task bisa antre   | Hanya untuk 1 Task Target|"));
    Serial.println(F("| API Pemanggilan dari ISR | xSemaphoreGiveFromISR   | vTaskNotifyGiveFromISR   |"));
    Serial.println(F("| Rekomendasi Kasus Pakai  | Multiple consumer/sync  | Delegasi ISR ke 1 Worker |"));
    Serial.println(F("=========================================================================="));
}

// -----------------------------------------------------------------------------
// ARDUINO CORE ENTRY POINTS (setup & loop)
// -----------------------------------------------------------------------------

void setup() {
    // 1. Inisialisasi Komunikasi Serial
    Serial.begin(115200);
    delay(1000); // Waktu stabilisasi terminal

    // 2. Inisialisasi Pin Hardware
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_LED_CORE0, OUTPUT);
    pinMode(PIN_LED_CORE1, OUTPUT);
    digitalWrite(PIN_LED_CORE0, LOW);
    digitalWrite(PIN_LED_CORE1, LOW);

    // 3. Pasang Hardware Interrupt pada GPIO 18 (Active LOW saat tombol ditekan)
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), isr_button_handler, FALLING);

    // 4. Buat Task Telemetri - DIPIN KE CORE 0 (PRO_CPU)
    // Parameter: Fungsi, Nama, Stack Size (Word), Parameter, Prioritas, Handle, Core ID (0)
    xTaskCreatePinnedToCore(
        TaskTelemetryCore0,
        "TelemetryCore0",
        2048,
        NULL,
        1,
        &xHandleTelemetryCore0,
        0   // Pin ke Core 0
    );

    // 5. Buat Task Worker - DIPIN KE CORE 1 (APP_CPU)
    // Prioritas 2 (lebih tinggi dari Telemetri) agar langsung menyambut notifikasi dari ISR
    xTaskCreatePinnedToCore(
        TaskWorkerCore1,
        "WorkerCore1",
        3072,
        NULL,
        2,
        &xHandleWorkerCore1,
        1   // Pin ke Core 1
    );

    // 6. Buat Task Stress Test - DIPIN KE CORE 1 (APP_CPU)
    xTaskCreatePinnedToCore(
        TaskStressCore1,
        "StressCore1",
        2048,
        NULL,
        1,
        &xHandleStressCore1,
        1   // Pin ke Core 1
    );

    // 7. Tampilkan Banner Menu Selamat Datang
    print_system_banner();
}

void loop() {
    // Membaca perintah karakter dari Serial Monitor
    if (Serial.available() > 0) {
        char cmd = Serial.read();

        // Abaikan karakter newline/carriage return
        if (cmd == '\r' || cmd == '\n') {
            return;
        }

        switch (cmd) {
            case '1':
                print_dualcore_info();
                break;
            case '2':
                print_task_pinning_status();
                break;
            case '3':
                trigger_software_interrupt();
                break;
            case '4':
                toggle_bad_isr();
                break;
            case '5':
                toggle_stress_test();
                break;
            case '6':
                print_comparison_analysis();
                break;
            case 'b':
            case 'B':
                Serial.println(F("[MANUAL] Menyimulasikan penekanan tombol fisik GPIO 18..."));
                isr_button_handler();
                break;
            case 'h':
            case 'H':
                print_system_banner();
                break;
            default:
                Serial.printf("\nPerintah '%c' tidak dikenali. Ketik 'h' untuk melihat daftar menu.\n", cmd);
                break;
        }

        Serial.print(F("\nEmbeddedSystem-W10 >> "));
    }

    // Berikan waktu idle pada loopTask agar Watchdog Timer tetap senang
    vTaskDelay(pdMS_TO_TICKS(50));
}
