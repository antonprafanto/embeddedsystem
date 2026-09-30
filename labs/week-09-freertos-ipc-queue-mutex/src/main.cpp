/**
 * ============================================================================
 * PRAKTIKUM SISTEM TERTANAM: FREERTOS INTER-PROCESS COMMUNICATION (IPC)
 * ============================================================================
 * Laboratorium Sistem Tertanam (Embedded Systems) - Teknik Elektro
 * Target Hardware: ESP32-WROOM-32 / ESP32-S3 (30/38 Pin DevKit)
 *
 * MODUL MINGGU 09:
 * 1. Queue: Pengiriman paket data antar-task secara thread-safe (FIFO).
 * 2. Mutex (Mutual Exclusion): Mengunci resource bersama & proteksi Race Condition.
 * 3. Binary Semaphore: Sinkronisasi kejadian (event-driven) dari ISR / Task.
 * 4. Investigasi Deadlock & Pemulihan Berbasis Timeout.
 * 5. Menu Interaktif Serial Monitor (115200 bps).
 * ============================================================================
 */

#include <Arduino.h>

// ============================================================================
// DEFINISI PIN HARDWARE & KONSTANTA
// ============================================================================
#define PIN_LED_ONBOARD      2     // LED status aktivitas sistem
#define PIN_BUTTON_EMERGENCY 18    // Tombol simulasi interupsi pemicu semafor
#define QUEUE_LENGTH         10    // Kapasitas maksimum antrean data (10 paket)

// ============================================================================
// STRUKTUR DATA PAKET SENSOR (THREAD-SAFE PAYLOAD)
// ============================================================================
typedef struct {
    uint32_t packet_id;
    float    temperature;
    float    humidity;
    uint32_t timestamp_ms;
} SensorPacket_t;

// ============================================================================
// IPC HANDLES (QUEUE, MUTEX & SEMAPHORE)
// ============================================================================
QueueHandle_t     xSensorQueue        = NULL;  // Antrean paket sensor (Producer-Consumer)
SemaphoreHandle_t xPrintMutex         = NULL;  // Mutex pelindung akses Serial Output
SemaphoreHandle_t xEmergencySemaphore = NULL;  // Binary semaphore pemicu alarm darurat
SemaphoreHandle_t xMutexA             = NULL;  // Mutex uji coba deadlock A
SemaphoreHandle_t xMutexB             = NULL;  // Mutex uji coba deadlock B

// Task Handles
TaskHandle_t hTaskProducer  = NULL;
TaskHandle_t hTaskConsumer  = NULL;
TaskHandle_t hTaskEmergency = NULL;
TaskHandle_t hTaskLoop      = NULL;

// Flag kontrol runtime
volatile bool g_producer_active = true;
volatile bool g_use_mutex_demo  = true;

// ============================================================================
// HELPER PRINT THREAD-SAFE DENGAN MUTEX
// ============================================================================
void safePrint(const char *msg) {
    if (xPrintMutex != NULL) {
        if (xSemaphoreTake(xPrintMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            Serial.print(msg);
            xSemaphoreGive(xPrintMutex);
        } else {
            Serial.println("[MUTEX TIMEOUT] Gagal mengambil kunci cetak!");
        }
    } else {
        Serial.print(msg);
    }
}

// ============================================================================
// ISR INTERUPSI TOMBOL FISIK (MEMBERIKAN BINARY SEMAPHORE DARI ISR)
// ============================================================================
static uint32_t last_interrupt_time = 0;

void IRAM_ATTR isrButtonEmergency() {
    uint32_t current_time = millis();
    // Debounce sederhana 100 ms
    if (current_time - last_interrupt_time > 100) {
        last_interrupt_time = current_time;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // Mengirimkan binary semaphore secara non-blocking dari dalam ISR
        if (xEmergencySemaphore != NULL) {
            xSemaphoreGiveFromISR(xEmergencySemaphore, &xHigherPriorityTaskWoken);
        }

        // Meminta pergantian konteks jika ada task berprioritas lebih tinggi yang terbangun
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

// ============================================================================
// TASK 1: PRODUCER (MENGHASILKAN DATA SENSOR KE QUEUE)
// ============================================================================
void TaskProducer(void *pvParameters) {
    SensorPacket_t packet;
    uint32_t count = 0;

    while (1) {
        if (g_producer_active) {
            count++;
            packet.packet_id    = count;
            packet.temperature  = 25.0f + (float)(rand() % 100) / 10.0f;
            packet.humidity     = 60.0f + (float)(rand() % 200) / 10.0f;
            packet.timestamp_ms = millis();

            // Kirim paket ke antrean (Queue) dengan batas waktu tunggu 50 ms
            if (xQueueSend(xSensorQueue, &packet, pdMS_TO_TICKS(50)) == pdPASS) {
                // Berhasil masuk antrean
                digitalWrite(PIN_LED_ONBOARD, !digitalRead(PIN_LED_ONBOARD));
            } else {
                // Antrean penuh!
                safePrint("[QUEUE FULL] Antrean sensor penuh, paket dibuang!\n");
            }
        }

        // Producer memproduksi data setiap 500 ms (2 Hz)
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// ============================================================================
// TASK 2: CONSUMER (MEMBACA & MENGOLAH DATA DARI QUEUE)
// ============================================================================
void TaskConsumer(void *pvParameters) {
    SensorPacket_t receivedPacket;

    while (1) {
        // Menunggu paket data dari antrean hingga maksimal 1000 ms
        if (xQueueReceive(xSensorQueue, &receivedPacket, pdMS_TO_TICKS(1000)) == pdPASS) {
            // Berhasil mengambil paket dari Queue
            if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE) {
                Serial.printf("[CONSUMER <- QUEUE] ID: #%lu | Suhu: %.1f C | RH: %.1f %% | Delay Antrean: %lu ms | Slot Antrean Sisa: %u\n",
                              receivedPacket.packet_id,
                              receivedPacket.temperature,
                              receivedPacket.humidity,
                              millis() - receivedPacket.timestamp_ms,
                              uxQueueSpacesAvailable(xSensorQueue));
                xSemaphoreGive(xPrintMutex);
            }
        } else {
            // Timeout: Antrean kosong selama 1 detik
            // Tidak perlu panik, task hanya kembali menunggu
        }
    }
}

// ============================================================================
// TASK 3: EMERGENCY ALARM HANDLER (EVENT-DRIVEN VIA BINARY SEMAPHORE)
// ============================================================================
void TaskEmergencyHandler(void *pvParameters) {
    while (1) {
        // Task ini berada dalam status BLOCKED (0% CPU) hingga semaphore diberikan
        if (xSemaphoreTake(xEmergencySemaphore, portMAX_DELAY) == pdTRUE) {
            // Sinyal darurat diterima!
            if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE) {
                Serial.println("\n🚨 =========================================================");
                Serial.println("🚨 [ALARM EVENT]: Sinyal Darurat Terdeteksi via Semaphore!");
                Serial.printf( "🚨 Pemicu: Tombol GPIO %d / Perintah CLI | Waktu: %lu ms\n", PIN_BUTTON_EMERGENCY, millis());
                Serial.println("🚨 Melakukan penanganan darurat (Emergency Shutdown Procedure)...");
                Serial.println("🚨 =========================================================\n");
                xSemaphoreGive(xPrintMutex);
            }

            // Strobo LED cepat tanda alarm
            for (int i = 0; i < 5; i++) {
                digitalWrite(PIN_LED_ONBOARD, HIGH);
                vTaskDelay(pdMS_TO_TICKS(50));
                digitalWrite(PIN_LED_ONBOARD, LOW);
                vTaskDelay(pdMS_TO_TICKS(50));
            }
        }
    }
}

// ============================================================================
// DEMO 2: SIMULASI RACE CONDITION VS PROTEKSI MUTEX
// ============================================================================
void demoWorkerUnsafe(void *pvParameters) {
    int id = (int)pvParameters;
    for (int i = 0; i < 3; i++) {
        // Tanpa Mutex: Dua task mencetak potongan string terpisah secara serentak
        Serial.printf("[UNSAFE Task-%d] Huruf: ", id);
        for (char c = 'A'; c <= 'E'; c++) {
            Serial.print(c);
            delayMicroseconds(200); // Simulasi kerja lambat yang memicu interleaving
        }
        Serial.println(" -> Selesai!");
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL);
}

void demoWorkerSafe(void *pvParameters) {
    int id = (int)pvParameters;
    for (int i = 0; i < 3; i++) {
        // Dengan Mutex: Cetak seluruh blok teks secara atomik
        if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE) {
            Serial.printf("[SAFE Mutex Task-%d] Huruf: ", id);
            for (char c = 'A'; c <= 'E'; c++) {
                Serial.print(c);
                delayMicroseconds(200);
            }
            Serial.println(" -> Selesai!");
            xSemaphoreGive(xPrintMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL);
}

void runRaceConditionDemo() {
    Serial.println("\n=======================================================");
    Serial.println("🔬 EKSPERIMEN: BENTURAN AKSES (RACE CONDITION) VS MUTEX");
    Serial.println("=======================================================");
    
    Serial.println("\n[UJI 1]: Tanpa Mutex (Perhatikan teks yang saling tumpang tindih!)...");
    xTaskCreate(demoWorkerUnsafe, "Unsafe1", 2048, (void*)1, 2, NULL);
    xTaskCreate(demoWorkerUnsafe, "Unsafe2", 2048, (void*)2, 2, NULL);
    vTaskDelay(pdMS_TO_TICKS(1000)); // Tunggu selesai

    Serial.println("\n[UJI 2]: Dengan Mutex (Perhatikan setiap baris tercetak utuh & rapi!)...");
    xTaskCreate(demoWorkerSafe, "Safe1", 2048, (void*)1, 2, NULL);
    xTaskCreate(demoWorkerSafe, "Safe2", 2048, (void*)2, 2, NULL);
    vTaskDelay(pdMS_TO_TICKS(1000)); // Tunggu selesai
    Serial.println("=======================================================\n");
}

// ============================================================================
// DEMO 4: SIMULASI DEADLOCK & PEMULIHAN BERBASIS TIMEOUT
// ============================================================================
void runDeadlockDemo() {
    Serial.println("\n=======================================================");
    Serial.println("⚠️ EKSPERIMEN: SKENARIO DEADLOCK & PENCEGAHAN TIMEOUT");
    Serial.println("=======================================================");
    Serial.println("Skenario: Task 1 memegang Mutex A dan mencoba mengambil Mutex B.");
    Serial.println("          Task 2 memegang Mutex B dan mencoba mengambil Mutex A.");
    Serial.println("Solusi Enjiniring: Menggunakan batas timeout (1000 ms) agar tidak terkunci selamanya!\n");

    // Task 1 mengambil Mutex A
    if (xSemaphoreTake(xMutexA, pdMS_TO_TICKS(500)) == pdTRUE) {
        Serial.println("[Task 1] Berhasil mengunci Mutex A.");

        // Simulasi Task 2 mengambil Mutex B di thread lain
        if (xSemaphoreTake(xMutexB, pdMS_TO_TICKS(500)) == pdTRUE) {
            Serial.println("[Task 2] Berhasil mengunci Mutex B.");

            Serial.println("\n[KONFLIK]: Task 1 sekarang mencoba mengambil Mutex B...");
            // Task 1 mencoba mengambil Mutex B yang sedang dipegang Task 2 dengan timeout 1 detik
            if (xSemaphoreTake(xMutexB, pdMS_TO_TICKS(1000)) == pdFALSE) {
                Serial.println("💥 [TIMEOUT TERCAPAI]: Task 1 gagal mengambil Mutex B setelah 1000 ms!");
                Serial.println("🛡️ [RECOVERY]: Task 1 melepaskan Mutex A secara sukarela untuk mencegah Deadlock!");
                xSemaphoreGive(xMutexA); // Lepaskan kunci agar tidak menggantung sistem
            }

            // Lepaskan Mutex B
            xSemaphoreGive(xMutexB);
            Serial.println("✅ [SISTEM PULIH]: Semua Mutex berhasil dinetralkan kembali.");
        } else {
            xSemaphoreGive(xMutexA);
        }
    }
    Serial.println("=======================================================\n");
}

// ============================================================================
// CETAK MENU BANTUAN INTERAKTIF
// ============================================================================
void printHelpMenu() {
    Serial.println("\n+-------------------------------------------------------------+");
    Serial.println("|   MENU INTERAKTIF LAB WEEK 09: FREERTOS IPC & SINKRONISASI  |");
    Serial.println("+-------------------------------------------------------------+");
    Serial.println("| [1] Toggle Producer Queue (Aktifkan / Nonaktifkan Sensor)   |");
    Serial.println("| [2] Demo Race Condition vs Mutex (Uji Benturan Output Teks) |");
    Serial.println("| [3] Trigger Event Semaphore (Kirim Sinyal Alarm Darurat)    |");
    Serial.println("| [4] Demo Simulasi Deadlock & Pemulihan Timeout              |");
    Serial.println("| [5] Status & Diagnostik Antrean (Queue Metrics & Mutex)     |");
    Serial.println("| [m] Cetak Ulang Menu Bantuan Ini                            |");
    Serial.println("+-------------------------------------------------------------+");
    Serial.print("Ketik angka pilihan Anda [1-5 / m]: ");
}

// ============================================================================
// TASK MENU INTERAKTIF CLI
// ============================================================================
void TaskInteractiveLoop(void *pvParameters) {
    printHelpMenu();

    while (1) {
        if (Serial.available() > 0) {
            char choice = Serial.read();
            // Abaikan newline dan carriage return
            if (choice == '\r' || choice == '\n') continue;

            Serial.printf("%c\n", choice);

            switch (choice) {
                case '1':
                    g_producer_active = !g_producer_active;
                    if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE) {
                        Serial.printf("\n[PRODUCER STATUS] Task Sensor sekarang: %s\n",
                                      g_producer_active ? "AKTIF (Mengirim paket ke Queue)" : "NONAKTIF (Dijeda)");
                        xSemaphoreGive(xPrintMutex);
                    }
                    break;

                case '2':
                    runRaceConditionDemo();
                    break;

                case '3':
                    Serial.println("\n[EVENT MANUAL] Mengirim sinyal darurat via xSemaphoreGive()...");
                    if (xEmergencySemaphore != NULL) {
                        xSemaphoreGive(xEmergencySemaphore);
                    }
                    break;

                case '4':
                    runDeadlockDemo();
                    break;

                case '5':
                    if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE) {
                        Serial.println("\n=======================================================");
                        Serial.println("📊 STATUS METRIK FREERTOS IPC");
                        Serial.println("=======================================================");
                        Serial.printf("  • Kapasitas Queue        : %d paket\n", QUEUE_LENGTH);
                        Serial.printf("  • Paket Menunggu di Queue: %u paket\n", uxQueueMessagesWaiting(xSensorQueue));
                        Serial.printf("  • Ruang Kosong Tersisa   : %u slot\n", uxQueueSpacesAvailable(xSensorQueue));
                        Serial.printf("  • Status Producer Sensor : %s\n", g_producer_active ? "AKTIF" : "DIJEDA");
                        Serial.printf("  • Mutex Print Pelindung  : %s\n", (xPrintMutex != NULL) ? "TERPASANG (OK)" : "NULL");
                        Serial.println("=======================================================\n");
                        xSemaphoreGive(xPrintMutex);
                    }
                    break;

                case 'm':
                case 'M':
                case 'h':
                case 'H':
                    printHelpMenu();
                    break;

                default:
                    Serial.println("[ERROR] Pilihan tidak valid! Ketik 'm' untuk melihat menu.");
                    break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// ============================================================================
// ARDUINO SETUP
// ============================================================================
void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(PIN_LED_ONBOARD, OUTPUT);
    pinMode(PIN_BUTTON_EMERGENCY, INPUT_PULLUP);

    // 1. Inisialisasi Mutex
    xPrintMutex = xSemaphoreCreateMutex();
    xMutexA      = xSemaphoreCreateMutex();
    xMutexB      = xSemaphoreCreateMutex();

    // 2. Inisialisasi Binary Semaphore
    xEmergencySemaphore = xSemaphoreCreateBinary();

    // 3. Inisialisasi Queue (Kapasitas: 10 paket ukuran SensorPacket_t)
    xSensorQueue = xQueueCreate(QUEUE_LENGTH, sizeof(SensorPacket_t));

    // Pasang Hardware Interrupt pada tombol fisik GPIO 18
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_EMERGENCY), isrButtonEmergency, FALLING);

    Serial.println("\n===============================================================");
    Serial.println("🎉 FREERTOS IPC ENGINE BERHASIL DIINISIALISASI");
    Serial.println("   Laboratorium Sistem Tertanam - S1 Teknik Elektro");
    Serial.println("===============================================================");
    Serial.printf("[INIT OK] Mutex Print       : %s\n", (xPrintMutex != NULL) ? "READY" : "FAILED");
    Serial.printf("[INIT OK] Binary Semaphore  : %s (Pin GPIO %d)\n", (xEmergencySemaphore != NULL) ? "READY" : "FAILED", PIN_BUTTON_EMERGENCY);
    Serial.printf("[INIT OK] FreeRTOS Queue    : %s (%d slots x %u bytes)\n",
                  (xSensorQueue != NULL) ? "READY" : "FAILED", QUEUE_LENGTH, sizeof(SensorPacket_t));

    // Membuat Task-Task FreeRTOS
    // Prioritas: Emergency (3) > Consumer (2) > Producer (1) > CLI (1)
    xTaskCreatePinnedToCore(TaskEmergencyHandler, "EmergencyTask", 2048, NULL, 3, &hTaskEmergency, 1);
    xTaskCreatePinnedToCore(TaskConsumer,         "ConsumerTask",  2560, NULL, 2, &hTaskConsumer,  1);
    xTaskCreatePinnedToCore(TaskProducer,         "ProducerTask",  2048, NULL, 1, &hTaskProducer,  1);
    xTaskCreatePinnedToCore(TaskInteractiveLoop,  "TaskCLI",       3072, NULL, 1, &hTaskLoop,      1);
}

// ============================================================================
// ARDUINO LOOP
// ============================================================================
void loop() {
    // Di arsitektur FreeRTOS profesional, seluruh beban kerja ditangani oleh
    // Task-Task independen. loop() dapat diistirahatkan total atau ditunda.
    vTaskDelay(pdMS_TO_TICKS(1000));
}
