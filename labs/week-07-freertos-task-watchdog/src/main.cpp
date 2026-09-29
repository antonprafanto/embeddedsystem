/**
 * ============================================================================
 * PRAKTIKUM SISTEM TERTANAM: FREERTOS TASK SCHEDULING & WATCHDOG TIMER (TWDT)
 * ============================================================================
 * Laboratorium Sistem Tertanam (Embedded Systems) - Teknik Elektro
 * Target Hardware: ESP32-WROOM-32 / ESP32-S3 (30/38 Pin DevKit)
 *
 * MODUL MINGGU 07:
 * 1. Preemptive Multitasking: Time-slicing FreeRTOS Tick Timer (1000 Hz / 1 ms).
 * 2. Task Lifecycle: Running, Ready, Blocked, Suspended, dan Pengaturan Prioritas.
 * 3. Task Stack Profiling: uxTaskGetStackHighWaterMark() untuk mendeteksi Stack Overflow.
 * 4. Task Watchdog Timer (TWDT): Eksperimen memicu Watchdog Panic dan penyembuhannya.
 * 5. Menu Interaktif Serial Monitor (115200 bps).
 * ============================================================================
 */

#include <Arduino.h>
#include <esp_task_wdt.h>

// ============================================================================
// DEFINISI PIN HARDWARE & KONSTANTA
// ============================================================================
#define PIN_LED_ONBOARD      2    // LED status aktivitas sistem
#define TWDT_TIMEOUT_SECONDS 3    // Batas waktu timeout Watchdog Timer (3 detik)

// ============================================================================
// TASK HANDLES
// ============================================================================
TaskHandle_t hTaskSensor     = NULL;
TaskHandle_t hTaskHeartbeat  = NULL;
TaskHandle_t hTaskReporter   = NULL;
TaskHandle_t hTaskHog        = NULL;

// ============================================================================
// VARIABEL DATA TELEMETRI BERSAMA
// ============================================================================
struct TelemetryData {
    float temperature;
    float humidity;
    uint32_t sample_count;
    uint32_t heartbeat_count;
} g_telemetry = {25.0f, 60.0f, 0, 0};

// Flag kontrol eksperimen
volatile bool g_trigger_starvation = false;
volatile bool g_run_normal_tasks   = true;

// ============================================================================
// TASK 1: PEMBACAAN SENSOR TELEMETRI (PERIODIK 50 MS / 20 HZ)
// ============================================================================
void TaskSensor(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(50); // Tepat 50 ms

    while (1) {
        if (g_run_normal_tasks) {
            // Simulasi pembacaan ADC / Sensor Suhu & Kelembaban
            g_telemetry.sample_count++;
            g_telemetry.temperature = 24.5f + (float)(rand() % 80) / 10.0f;
            g_telemetry.humidity    = 55.0f + (float)(rand() % 250) / 10.0f;
        }

        // vTaskDelayUntil menjamin periode eksekusi deterministik (anti-jitter)
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// ============================================================================
// TASK 2: KEDIP LED HEARTBEAT (PERIODIK 500 MS / 1 HZ)
// ============================================================================
void TaskHeartbeat(void *pvParameters) {
    pinMode(PIN_LED_ONBOARD, OUTPUT);

    while (1) {
        if (g_run_normal_tasks) {
            digitalWrite(PIN_LED_ONBOARD, HIGH);
            vTaskDelay(pdMS_TO_TICKS(100)); // LED Menyala 100 ms
            digitalWrite(PIN_LED_ONBOARD, LOW);
            g_telemetry.heartbeat_count++;
            vTaskDelay(pdMS_TO_TICKS(400)); // LED Padam 400 ms (Total periode = 500 ms)
        } else {
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}

// ============================================================================
// TASK 3: LAPORAN TELEMETRI BERKALA (PERIODIK 2000 MS)
// ============================================================================
void TaskReporter(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(2000));

        if (g_run_normal_tasks && !g_trigger_starvation) {
            // Mengukur sisa stack terkecil (High Water Mark)
            UBaseType_t stackSensor    = (hTaskSensor != NULL)    ? uxTaskGetStackHighWaterMark(hTaskSensor) : 0;
            UBaseType_t stackHeartbeat = (hTaskHeartbeat != NULL) ? uxTaskGetStackHighWaterMark(hTaskHeartbeat) : 0;
            UBaseType_t stackReporter  = uxTaskGetStackHighWaterMark(NULL);

            Serial.printf("[AUTO-TELEMETRY] Sampel: #%lu | Suhu: %.1f C | RH: %.1f %% | Detak LED: #%lu | Stack Sisa: [Sens:%uB, HB:%uB, Rep:%uB]\n",
                          g_telemetry.sample_count,
                          g_telemetry.temperature,
                          g_telemetry.humidity,
                          g_telemetry.heartbeat_count,
                          (unsigned int)stackSensor,
                          (unsigned int)stackHeartbeat,
                          (unsigned int)stackReporter);
        }
    }
}

// ============================================================================
// TASK KHUSUS EKSPERIMEN 3: TASK NAKAL PEMICU WATCHDOG (CPU HOG)
// ============================================================================
void TaskHungryHog(void *pvParameters) {
    bool with_yield = (bool)(intptr_t)pvParameters;

    Serial.println("\n--------------------------------------------------------------");
    Serial.printf("[HOG STARTED] Task HungryHog aktif pada Prioritas 3 (Core %d)!\n", xPortGetCoreID());
    if (with_yield) {
        Serial.println(">> MODE AMAN: Melakukan kalkulasi berat DENGAN vTaskDelay(1) untuk yield.");
        Serial.println(">> Watchdog TIDAK AKAN terpicu karena Idle Task tetap kebagian CPU!");
    } else {
        Serial.println(">> MODE BAHAYA: Memonopoli CPU 100% TANPA memanggil vTaskDelay()!");
        Serial.printf(">> Tunggu %d detik... Task Watchdog Timer (TWDT) AKAN MELEDAK!\n", TWDT_TIMEOUT_SECONDS);
    }
    Serial.println("--------------------------------------------------------------");

    uint32_t loop_counter = 0;
    unsigned long start_time = millis();

    while (1) {
        loop_counter++;
        
        // Komputasi matematika dummy agar CPU bekerja keras
        volatile double dummy = 3.14159265;
        dummy = dummy * dummy / 1.0001;
        (void)dummy;

        if (with_yield) {
            // Memberi kesempatan kepada CPU untuk mengeksekusi Idle Task & reset WDT
            if (loop_counter % 50000 == 0) {
                vTaskDelay(pdMS_TO_TICKS(2));
                Serial.printf("   [HOG YIELD] Iterasi: %lu | Berhasil berbagi CPU dengan aman.\n", loop_counter);
            }
            if (millis() - start_time > 4000) {
                Serial.println("[HOG FINISHED] Pengujian mode aman tuntas! Menghapus task hog...");
                hTaskHog = NULL;
                vTaskDelete(NULL); // Menghapus diri sendiri
            }
        } else {
            // TIDAK ADA vTaskDelay sama sekali!
            if (loop_counter % 2000000 == 0) {
                Serial.printf("   [HOG MONOPOLI] Iterasi: %lu | Masih menolak yield ke CPU... (%lu ms)\n",
                              loop_counter, millis() - start_time);
            }
        }
    }
}

// ============================================================================
// FUNGSI MENU 1: STATUS MULTITASKING NORMAL
// ============================================================================
void show_multitasking_status() {
    Serial.println("\n========================================================");
    Serial.println("  STATUS SCHEDULER FREERTOS & MULTI-TASKING ESP32       ");
    Serial.println("========================================================");
    Serial.printf("  • Frekuensi Tick FreeRTOS : %d Hz (1 Tick = %d ms)\n", 
                  configTICK_RATE_HZ, 1000 / configTICK_RATE_HZ);
    Serial.printf("  • Jumlah Core Aktif       : Dual-Core Xtensa LX6 (240 MHz)\n");
    Serial.printf("  • Core Eksekusi Task Menu : Core %d (APP_CPU)\n", xPortGetCoreID());
    Serial.printf("  • Status Task Sensor      : %s (Periodik 50 ms / 20 Hz, Prioritas 2)\n",
                  (hTaskSensor != NULL) ? "BERJALAN" : "NONAKTIF");
    Serial.printf("  • Status Task Heartbeat   : %s (Periodik 500 ms / 1 Hz, Prioritas 1)\n",
                  (hTaskHeartbeat != NULL) ? "BERJALAN" : "NONAKTIF");
    Serial.printf("  • Status Task Reporter    : %s (Periodik 2000 ms, Prioritas 1)\n",
                  (hTaskReporter != NULL) ? "BERJALAN" : "NONAKTIF");
    Serial.println("--------------------------------------------------------");
    Serial.println("[INSIGHT TEKNIK ELEKTRO]:");
    Serial.println("Preemptive Multitasking memungkinkan sensor dibaca setiap 50 ms");
    Serial.println("secara presisi tanpa terganggu oleh kedipan LED yang butuh 500 ms!");
}

// ============================================================================
// FUNGSI MENU 2: PROFILING MEMORI STACK (HIGH WATER MARK)
// ============================================================================
void show_stack_profiling() {
    Serial.println("\n==========================================================================");
    Serial.println("   PROFILING MEMORI TASK STACK (uxTaskGetStackHighWaterMark)             ");
    Serial.println("==========================================================================");
    Serial.println("| Nama Task       | Prioritas | Core | Alokasi Awal | Sisa Min (B) | Status   |");
    Serial.println("|-----------------|-----------|------|--------------|--------------|----------|");

    struct TaskInspect {
        TaskHandle_t handle;
        const char *name;
        uint32_t allocated;
        uint8_t priority;
        int core;
    } tasks[] = {
        {hTaskSensor,    "TaskSensor     ", 3072, 2, 1},
        {hTaskHeartbeat, "TaskHeartbeat  ", 2048, 1, 1},
        {hTaskReporter,  "TaskReporter   ", 3072, 1, 1},
        {NULL,           "TaskLoop (CLI) ", 8192, 1, 1}
    };

    for (int i = 0; i < 4; i++) {
        TaskHandle_t th = (tasks[i].handle != NULL) ? tasks[i].handle : xTaskGetCurrentTaskHandle();
        UBaseType_t hwm = uxTaskGetStackHighWaterMark(th);
        
        const char *health = "AMAN";
        if (hwm < 256) health = "BAHAYA!";
        else if (hwm < 512) health = "WASPADA";

        Serial.printf("| %s | %9u | %4d | %9u B | %9u B | %-8s |\n",
                      tasks[i].name, tasks[i].priority, tasks[i].core,
                      tasks[i].allocated, (unsigned int)hwm, health);
    }
    Serial.println("==========================================================================");
    Serial.println("[CATATAN ENJINIRING MEMORI]:");
    Serial.println("1. 'Sisa Min (B)' adalah High Water Mark: jarak terdekat ujung stack");
    Serial.println("   ke batas luapan memori (Stack Overflow) sejak task pertama kali dibuat.");
    Serial.println("2. Jika 'Sisa Min' menyentuh angka 0, ESP32 seketika mengalami crash");
    Serial.println("   dengan pesan fatal: 'Guru Meditation Error: Core 1 panic'd (Unhandled debug exception)'.");
}

// ============================================================================
// FUNGSI MENU 3: SIMULASI MEMICU WATCHDOG TIMEOUT (TWDT CRASH DEMO)
// ============================================================================
void trigger_watchdog_timeout() {
    Serial.println("\n========================================================");
    Serial.println("  DEMO 3: SIMULASI MEMICU TASK WATCHDOG TIMER (TWDT)    ");
    Serial.println("========================================================");
    Serial.println("[PERINGATAN]: Eksperimen ini SENGAJA memonopoli CPU 100%");
    Serial.println("              tanpa memberi kesempatan pada Idle Task.");
    Serial.println("              ESP32 AKAN REBOOT SECARA OTOMATIS setelah");
    Serial.printf("              timeout %d detik terlewati!\n", TWDT_TIMEOUT_SECONDS);
    Serial.println("========================================================");
    delay(1000);

    // Buat TaskHog pada Prioritas 3 (lebih tinggi dari Idle Task yang berprioritas 0)
    // Parameter false = TANPA YIELD (Memicu Watchdog)
    xTaskCreatePinnedToCore(
        TaskHungryHog,
        "TaskHungryHog",
        3072,
        (void *)(intptr_t)false,
        3,
        &hTaskHog,
        1
    );
}

// ============================================================================
// FUNGSI MENU 4: PENGUJIAN MODE AMAN (TASK BERAT DENGAN YIELD)
// ============================================================================
void test_safe_task_computation() {
    Serial.println("\n========================================================");
    Serial.println("  DEMO 4: PENANGANAN TEPAT KOMPUTASI BERAT (DENGAN YIELD)");
    Serial.println("========================================================");
    Serial.println("[SOLUSI ENJINIRING]:");
    Serial.println("Setiap task yang menjalankan komputasi panjang WAJIB menyisipkan");
    Serial.println("panggilan vTaskDelay(1) atau taskYIELD() secara berkala.");
    Serial.println("Ini memberikan jatah CPU pada Idle Task untuk mereset Watchdog!");
    Serial.println("========================================================");

    if (hTaskHog != NULL) {
        Serial.println("[FAIL] Masih ada Task Hog yang berjalan!");
        return;
    }

    // Parameter true = DENGAN YIELD
    xTaskCreatePinnedToCore(
        TaskHungryHog,
        "TaskSafeHog",
        3072,
        (void *)(intptr_t)true,
        3,
        &hTaskHog,
        1
    );
}

// ============================================================================
// FUNGSI MENU 5: DEMONSTRASI PREEMPTIVE TASK PRIORITY
// ============================================================================
void demonstrate_task_preemption() {
    Serial.println("\n========================================================");
    Serial.println("  DEMO 5: ANALISIS PREEMPTION & PERBEDAAN PRIORITAS TASK ");
    Serial.println("========================================================");
    Serial.println("Pada FreeRTOS, Task dengan angka prioritas LEBIH TINGGI");
    Serial.println("akan SEKETIKA merebut (preempt) CPU dari Task berprioritas rendah.");
    Serial.println("\nHierarki Prioritas Saat Ini:");
    Serial.println("  • Prioritas 3 : Task HungryHog / Alarm Darurat");
    Serial.println("  • Prioritas 2 : Task Sensor Telemetri (50 ms)");
    Serial.println("  • Prioritas 1 : Task Heartbeat & Reporter (500 ms - 2000 ms)");
    Serial.println("  • Prioritas 0 : Task IDLE Bawaan ESP-IDF (Pembersih Memori & WDT Feeder)");
    Serial.println("--------------------------------------------------------");
    Serial.println("HUKUM EMAS FREERTOS:");
    Serial.println("Jangan biarkan Task berprioritas tinggi berjalan terus-menerus tanpa jeda!");
    Serial.println("Jika Task Prioritas 1 atau lebih memonopoli CPU, Task IDLE (Prioritas 0)");
    Serial.println("tidak akan pernah dieksekusi -> Memicu Task Watchdog Timeout!");
}

// ============================================================================
// TAMPILKAN BANNER MENU
// ============================================================================
void print_menu() {
    Serial.println("\n+-------------------------------------------------------------+");
    Serial.println("|     MENU INTERAKTIF LAB WEEK 07: FREERTOS & WATCHDOG        |");
    Serial.println("+-------------------------------------------------------------+");
    Serial.println("| [1] Status Multi-Tasking & Pembagian Waktu Scheduler        |");
    Serial.println("| [2] Profiling Memori Stack Task (Deteksi Stack Overflow)    |");
    Serial.println("| [3] Simulasi Memicu Crash Watchdog Timer (TWDT Timeout Demo)|");
    Serial.println("| [4] Uji Komputasi Berat yang Aman (Penyembuhan via Yield)   |");
    Serial.println("| [5] Analisis Preemptive Priority & Peran Idle Task          |");
    Serial.println("| [m] Tampilkan Ulang Menu Pilihan                            |");
    Serial.println("+-------------------------------------------------------------+");
    Serial.print("Ketik angka pilihan Anda [1-5]: ");
}

// ============================================================================
// SETUP & MAIN LOOP
// ============================================================================
void setup() {
    // 1. Inisialisasi Serial Debugging (115200 bps)
    Serial.begin(115200);
    delay(1000);

    // 2. Salam Pembuka Ramah Awam
    Serial.println("\n========================================================");
    Serial.println("SELAMAT DATANG DI PRAKTIKUM MINGGU 07 - SISTEM TERTANAM");
    Serial.println("Real-Time Operating Systems (FreeRTOS) & Watchdog Timer");
    Serial.println("Laboratorium Sistem Tertanam - Teknik Elektro");
    Serial.println("========================================================");

    // 3. Konfigurasi Task Watchdog Timer (TWDT) bawaan ESP-IDF
    // Mengaktifkan TWDT dengan timeout 3 detik dan panic=true (reboot saat hang)
    #if ESP_IDF_VERSION_MAJOR >= 5
        esp_task_wdt_config_t twdt_config = {
            .timeout_ms = TWDT_TIMEOUT_SECONDS * 1000,
            .idle_core_mask = (1 << 0) | (1 << 1), // Pantau Idle Task di kedua core
            .trigger_panic = true
        };
        esp_task_wdt_reconfigure(&twdt_config);
    #else
        esp_task_wdt_init(TWDT_TIMEOUT_SECONDS, true);
        esp_task_wdt_add(NULL); // Daftarkan task loop() ke TWDT
    #endif

    Serial.printf("[INIT] Task Watchdog Timer (TWDT) aktif dengan timeout %d detik.\n", TWDT_TIMEOUT_SECONDS);

    // 4. Membuat Task-Task Paralel FreeRTOS
    // xTaskCreatePinnedToCore(TaskFunction, Name, StackBytes, Param, Priority, Handle, CoreID)
    
    // Task 1: Sensor (Prioritas 2, Stack 3072 Byte, Core 1)
    xTaskCreatePinnedToCore(
        TaskSensor,
        "TaskSensor",
        3072,
        NULL,
        2,
        &hTaskSensor,
        1
    );

    // Task 2: Heartbeat LED (Prioritas 1, Stack 2048 Byte, Core 1)
    xTaskCreatePinnedToCore(
        TaskHeartbeat,
        "TaskHeartbeat",
        2048,
        NULL,
        1,
        &hTaskHeartbeat,
        1
    );

    // Task 3: Telemetry Reporter (Prioritas 1, Stack 3072 Byte, Core 1)
    xTaskCreatePinnedToCore(
        TaskReporter,
        "TaskReporter",
        3072,
        NULL,
        1,
        &hTaskReporter,
        1
    );

    Serial.println("[OK] Tiga Task Paralel FreeRTOS berhasil dibuat di Core 1!");
    print_menu();
}

void loop() {
    // Loop Arduino Core berjalan sebagai task prioritas 1 di Core 1
    #if ESP_IDF_VERSION_MAJOR < 5
        esp_task_wdt_reset(); // Beri makan watchdog untuk task loop
    #endif

    if (Serial.available() > 0) {
        char cmd = Serial.read();
        while (Serial.available() > 0) Serial.read(); // Bersihkan sisa buffer newline

        switch (cmd) {
            case '1':
                show_multitasking_status();
                break;
            case '2':
                show_stack_profiling();
                break;
            case '3':
                trigger_watchdog_timeout();
                break;
            case '4':
                test_safe_task_computation();
                break;
            case '5':
                demonstrate_task_preemption();
                break;
            case 'm':
            case 'M':
                print_menu();
                break;
            default:
                if (cmd != '\r' && cmd != '\n') {
                    Serial.printf("\n[PERINGATAN] Perintah '%c' tidak dikenali! Ketik 1, 2, 3, 4, 5, atau m.\n", cmd);
                    print_menu();
                }
                break;
        }
    }
    vTaskDelay(pdMS_TO_TICKS(50));
}
