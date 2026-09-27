# RENCANA PEMBELAJARAN SEMESTER (RPS) & PANDUAN LENGKAP
## MATA KULIAH: SISTEM TERTANAM (EMBEDDED SYSTEMS)
### PROGRAM STUDI SARJANA (S1) TEKNIK ELEKTRO

---

## 1. IDENTITAS MATA KULIAH

| Parameter | Keterangan |
| :--- | :--- |
| **Nama Mata Kuliah** | Sistem Tertanam (*Embedded Systems*) |
| **Kode Mata Kuliah** | - |
| **Bobot SKS** | - |
| **Beban Jam Perkuliahan** | - |
| **Semester** | V (Tingkat 3 - Ganjil) |
| **Prasyarat Formal** | Rangkaian Digital & Mikroprosesor, Dasar Pemrograman Komputer |
| **Platform Target** | ESP32 (Xtensa Dual-Core 32-bit LX6 / ESP32-S3) |
| **Toolchain & Lingkungan** | VS Code + PlatformIO (Hybrid: Arduino Core Transisi ke ESP-IDF Native C/C++) |
| **Model Pembelajaran** | *Scaffolding (Kupas Bawang)*, *Two-Tier Curriculum*, & *Project-Based Learning* |

---

## 2. DESKRIPSI MATA KULIAH
Mata kuliah ini dirancang untuk mengantarkan mahasiswa S1 Teknik Elektro dari taraf pemula menjadi calon insinyur yang menguasai rekayasa sistem tertanam (*hardware-software co-design*). Pembelajaran dirancang realistis dan bertahap: diawali dengan penanganan kendala fisik/driver di lab, penguatan logika manipulasi bit bahasa C, pemahaman batasan elektrikal pin dan sirkuit penggerak beban, diagnosa sistem saat mengalami kegagalan/crash, sistem operasi waktu nyata (FreeRTOS), komunikasi data serial hingga industri, manajemen daya baterai, keamanan siber firmware, hingga integrasi desain PCB fisik.

---

## 3. STRATEGI OPERASIONAL LAB & MITIGASI MAHASISWA AWAM

Agar perkuliahan berjalan lancar dan ramah bagi mahasiswa dengan latar belakang beragam (SMA maupun SMK), diterapkan 4 kebijakan operasional berikut:

### A. Kebijakan Transisi Framework Bertahap
Mencegah frustrasi akibat sintaks verbose di awal perkuliahan:
* **Fase Transisi (Minggu 1 – 3):** Menggunakan *Arduino Framework di dalam PlatformIO*. Tujuannya agar mahasiswa fokus memahami konsep elektrikal arus, proteksi transistor, kalibrasi ADC, dan manipulasi bit register tanpa terbebani kerumitan konfigurasi *boilerplate* ESP-IDF.
* **Fase Enjiniring Penuh (Minggu 4 – 16):** Mahasiswa diajak "membuka kap mesin", melihat bagaimana layer Arduino sebenarnya memanggil ESP-IDF API, lalu bertransisi penuh ke *FreeRTOS C API dan driver native ESP-IDF*.

### B. Protokol 5 Menit "Hardware Sanity Check"
Untuk mencegah krisis percaya diri (*"Kode saya sama dengan teman tapi kok punya saya tidak jalan?"*), mahasiswa diwajibkan melakukan 3 langkah pemeriksaan fisik mandiri dengan multimeter sebelum menyalahkan kode program:
1. **Cek Rel Daya (3V3 & GND):** Ukur tegangan rel power breadboard dengan multimeter digital (harus berada di rentang 3.25V – 3.35V). Jika drop < 3.0V, periksa kabel USB atau hubungkan catu daya eksternal.
2. **Cek Kontinuitas Jalur:** Breadboard murah sering memiliki jalur tembaga kendor/oksidasi. Uji kontinuitas jumper wire menggunakan mode buzzer multimeter.
3. **Cek Loopback Serial:** Putuskan jalur sensor, pastikan ESP32 merespons perintah *heartbeat* log di terminal untuk memisahkan masalah board vs masalah sensor eksternal.

### C. Mekanisme Asesmen "Uji Mutasi Kode" (Anti-Joki & Anti AI-Plagiarism)
Penilaian praktikum lab tidak hanya menilai *"apakah alatnya berfungsi/menyala"*. Asisten/Dosen akan melakukan **Live Code Mutation**:
* Dosen/Asisten memberikan perubahan parameter secara mendadak di tempat (misal: *"Ubah periodisitas Task Sensor dari 100ms menjadi 350ms dan jelaskan perubahannya pada antrian Queue"*).
* Mahasiswa wajib mendemonstrasikan dan menjelaskan baris kode yang diubah. Ini menjamin mahasiswa benar-benar memahami logika yang dibuatnya dan tidak sekadar menempel kode hasil AI tanpa pemahaman.

### D. Anggaran Hardware Kit Ramah Kantong Mahasiswa
Total pengadaan kit praktikum per kelompok (2 mahasiswa) dirancang sangat ekonomis dan mudah didapatkan di marketplace lokal:
* Board ESP32-WROOM-32D: ± Rp 55.000
* 8-Ch USB Logic Analyzer (24MHz): ± Rp 65.000 (dapat disediakan lab atau per kelompok)
* Paket Sensor (BME280 / MPU-6050, Display OLED, Modul Relay, Kabel): ± Rp 70.000
* **Estimasi Total:** < Rp 190.000 per kelompok (hanya sekitar Rp 95.000 per mahasiswa).

---

## 4. PEDOMAN EMAS PERANGKAT KERAS (ESP32 PIN QUIRKS & GOTCHAS)
*Tabel ini wajib dibagikan sebagai lembar saku mahasiswa dan ditempel di setiap meja laboratorium:*

| Kategori Pin | Nomor Pin GPIO | Aturan Penggunaan & Dampak Fatal |
| :--- | :--- | :--- |
| **DILARANG DIGUNAKAN** | GPIO 6, 7, 8, 9, 10, 11 | **TABU.** Terhubung langsung ke chip internal SPI Flash memori. Dicolok jumper = ESP32 langsung *crash/bootloop* instan! |
| **INPUT ONLY (GPI)** | GPIO 34, 35, 36, 39 | **Hanya bisa input sinyal.** Tidak memiliki sirkuit output dan **TIDAK memiliki resistor pull-up/pull-down internal**. |
| **JEBAKAN ADC2 vs WI-FI** | GPIO 0, 2, 4, 12, 13, 14, 15, 25, 26, 27 | **JANGAN pasang sensor analog di sini jika Wi-Fi aktif!** Sirkuit ADC2 mati saat radio Wi-Fi menyala. Sensor analog **HANYA boleh di ADC1 (GPIO 32–39)**. |
| **STRAPPING PINS (BOOT)** | GPIO 0, 2, 12, 15 | Menentukan mode bootloader. Jangan ditarik paksa ke HIGH/LOW permanen saat startup karena akan menyebabkan board gagal flashing. |
| **PIN PALING AMAN DIPAKAI**| GPIO 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33 | Pin bebas untuk sinyal digital, PWM, I2C, SPI, UART, dan interupsi. |

---

## 5. CAPAIAN PEMBELAJARAN (CPL & CPMK)

* **CPMK-1 (Fondasi & C Embedded):** Menguasai operasi bitwise, pointer, memory map, serta mampu mendiagnosa pesan kegagalan sistem (*backtrace decoding*).
* **CPMK-2 (Elektrikal & Interfacing):** Merancang antarmuka input/output, proteksi beban induktif (transistor switch & flyback diode), serta kalibrasi ADC1 presisi.
* **CPMK-3 (Komunikasi Instrumen):** Mampu mengonfigurasi dan mendiagnosa komunikasi serial (UART, I2C, SPI) serta bus industri menggunakan USB Logic Analyzer.
* **CPMK-4 (Sistem Waktu Nyata):** Merancang arsitektur multi-tasking FreeRTOS yang deterministik, thread-safe (Queue/Mutex), dan bebas dari watchdog timeout (TWDT).
* **CPMK-5 (Integrasi Sistem, Daya, & IoT):** Mengintegrasikan sleep-mode hemat energi, telemetri nirkabel MQTT, serta merealisasikan prototipe fisik terpadu (*Capstone Project*).

---

## 6. RINCIAN SILABUS MINGGUAN (16 MINGGU)

### MINGGU 0 (PRA-KULIAH): ONBOARDING & DRIVER CLINIC (MANDIRI / ASISTENSI)
* Verifikasi hardware: Membedakan kabel data USB vs kabel charger daya (2-wire vs 4-wire).
* Instalasi driver chip komunikasi: Silicon Labs CP210x vs WCH CH340 vs FTDI.
* Setup VS Code + PlatformIO extension, verifikasi compiler toolchain, dan pengenalan serial monitor.

---

### FASE 1: FONDASI C, ELEKTRIKAL PIN, & DIAGNOSTIK ERROR (MINGGU 1 - 3)

#### Minggu 1: Arsitektur 32-bit & Penyegaran Bahasa C untuk Embedded
* **Materi Teori:**
  * Komparasi arsitektur 8-bit (AVR) vs 32-bit SoC (Xtensa Dual-Core ESP32).
  * Struktur memori: Flash (program), SRAM (DRAM untuk variabel, IRAM untuk kode cepat), dan RTC Memory.
  * Operasi Bitwise: Operator `&`, `|`, `^`, `~`, `<<`, `>>`. Teknik bit-masking (setting, clearing, toggling bit register).
  * Struktur data C: Struct, pointer dasar, dan kata kunci `volatile` (mencegah kompilator mengabaikan pembacaan pin fisik).
* **Praktikum / Hands-on:**
  * Latihan bitwise di PlatformIO: Manipulasi register virtual untuk menyalakan/mematikan bit status.
  * Program LED blink non-blocking berbasis state-machine menggunakan fungsi pencatat waktu (`esp_timer_get_time()`).
* **Asesmen:** Kuis interaktif bitwise manipulation dan verifikasi instalasi toolchain.

#### Minggu 2: Karakteristik Elektrikal Pin, Interupsi, & Crash Debugging
* **Materi Teori:**
  * Batasan fisik pin: Batas tegangan 3.3V (mengapa logika 5V membakar pin), arus maksimum per pin ($\le 12\text{ mA}$ aman), pull-up/down internal.
  * Gotchas Pin ESP32: Menghindari pin SPI Flash (GPIO 6-11) dan memahami pin input-only (GPIO 34-39).
  * Mekanisme Hardware Interrupt: Edge triggers (RISING, FALLING, CHANGE), fungsi ISR, atribut `IRAM_ATTR`.
  * **Keahlian Wajib Pemula (Debugging):** Mengenal *Guru Meditation Error*, membaca alamat *Backtrace*, menggunakan *ESP Exception Decoder*, dan sistem logging bertingkat (`ESP_LOGI`, `ESP_LOGW`, `ESP_LOGE`).
* **Praktikum / Hands-on:**
  * Membaca sinyal rotary encoder menggunakan GPIO interrupt dengan debouncing software.
  * Simulasi error: Mahasiswa sengaja membuat kode yang memicu *Null Pointer Dereference* dan *Divide by Zero*, lalu memecahkan lokasi baris kode penyebab crash melalui terminal backtrace.
* **Asesmen:** Laporan praktikum: Analisis sinyal debouncing tombol pada osiloskop/logic analyzer.

#### Minggu 3: Interfacing Beban Daya (Transistor Driver) & Periferal Analog (ADC1 & PWM)
* **Materi Teori:**
  * **Aspek Khas Teknik Elektro:** Mengapa mikrokontroler tidak boleh menggerakkan relay/motor langsung? Rangkaian penggerak transistor (BJT 2N2222 / N-Channel MOSFET 2N7000) sebagai switch saklar.
  * Dioda proteksi *Flyback Back-EMF* (1N4007) pada beban induktif (relay/solenoid).
  * Periferal ADC 12-bit: Aturan emas **HANYA gunakan ADC1 (GPIO 32-39)** karena ADC2 nonaktif saat Wi-Fi menyala.
  * Kalibrasi non-linearitas ADC ESP32 menggunakan eFuse Vref calibration.
  * Modul LEDC (Hardware PWM): Frekuensi, resolusi bit, dan rumus duty cycle untuk kendali motor/LED.
* **Praktikum / Hands-on:**
  * Merangkai driver transistor BJT + dioda flyback untuk mengendalikan modul relay 5V atau motor DC kecil dari pin 3.3V ESP32.
  * Pembacaan sensor tegangan/potensiometer menggunakan ADC1 terkalibrasi; menggerakkan motor via PWM.
* **Asesmen:** Menghitung kurva linieritas ADC dan membandingkan error sebelum vs sesudah kalibrasi.

---

### FASE 2: MEMORI PERSISTEN & KOMUNIKASI INSTRUMEN (MINGGU 4 - 6)

#### Minggu 4: Penyimpanan Persisten (NVS & LittleFS) & Cara Membaca Datasheet
* **Materi Teori:**
  * Memori volatil vs non-volatil: Mengapa kita butuh NVS (*Non-Volatile Storage*) untuk menyimpan parameter konfigurasi, kalibrasi, atau kredensial Wi-Fi.
  * Partisi flash memory ESP32: Mengenal tabel partisi (`partitions.csv`) dan sistem file LittleFS untuk file statis.
  * Literasi Enjiniring: Bedah *Datasheet* komponen (tegangan absolut, arus quiescent, *timing diagram*, dan register I2C/SPI).
* **Praktikum / Hands-on:**
  * Membuat fitur penyimpanan *boot counter* dan nilai ambang batas sensor (*threshold*) ke dalam NVS agar data tidak hilang saat mati listrik.
* **Asesmen:** Latihan membaca datasheet sensor industri (menemukan register address dan rumus konversi data biner).

#### Minggu 5: Protokol Komunikasi Serial Standar & USB Logic Analyzer
* **Materi Teori:**
  * Protokol Serial Fundamental:
    * **UART:** Asinkron, baud rate, start/stop bit, framing error.
    * **I2C:** Sinkron 2 kawat (SDA/SCL), open-drain, pull-up resistor calculation, address matching, ACK/NACK.
    * **SPI:** Sinkron 4 kawat kecepatan tinggi (MOSI, MISO, SCK, CS), SPI Mode (CPOL/CPHA).
  * Proteksi logic level: Level shifter 5V ke 3.3V bidirectional.
* **Praktikum / Hands-on:**
  * Membaca sensor suhu/kelembaban I2C (BME280/AHT10) dan menampilkan data ke layar grafik SPI (OLED/TFT).
  * Menggunakan **8-Channel USB Logic Analyzer** dengan software *PulseView*: Merekam dan mendekode paket data biner I2C/SPI secara visual di layar komputer.
* **Asesmen:** Membuktikan nilai byte heksadesimal yang lewat di kabel sesuai dengan register sensor.

#### Minggu 6: Komunikasi Industri Jarak Jauh (CAN Bus & RS-485 Modbus)
* **Materi Teori:**
  * Keterbatasan I2C/SPI pada kabel panjang: Masalah interferensi derau elektromagnetik (EMI).
  * Konsep sinyal diferensial (*differential signaling*):
    * **RS-485 & Modbus RTU:** Transmisi half-duplex jarak jauh (hingga 1 km), format frame Master-Slave, CRC-16.
    * **CAN Bus 2.0B / TWAI:** Standar otomotif dan otomatisasi pabrik, arbitrasi bit pesan prioritas, terminasi impedansi 120-Ohm.
* **Praktikum / Hands-on:**
  * Jaringan dua board ESP32: Node 1 bertindak sebagai transmitter data sensor melalui modul transceiver CAN (SN65HVD230) atau RS-485 (MAX3485); Node 2 menerima dan memverifikasi data.
* **Asesmen:** Uji pengiriman data dengan kabel twisted-pair melewati sumber interferensi motor listrik.

---

### FASE 3: REAL-TIME OPERATING SYSTEMS (FreeRTOS) (MINGGU 7 - 10)

#### Minggu 7: Konsep Dasar RTOS, Task Scheduling, & Watchdog Timer
* **Materi Teori:**
  * Keterbatasan pemrograman sekuensial *Super-Loop* (`void loop()`) saat menangani multi-sensor dan komunikasi simultan.
  * Konsep Preemptive Multitasking: Bagaimana CPU membagi waktu (*time slicing*) antar-task.
  * Siklus Hidup Task: *Running, Ready, Blocked, Suspended*. Pengaturan prioritas task.
  * Task Stack Size: Menghitung alokasi memori RAM task dan mencegah *Stack Overflow* (`uxTaskGetStackHighWaterMark`).
  * **Gotcha Kritis:** Mengapa ESP32 restart sendiri? Pemahaman **Task Watchdog Timer (TWDT)** dan mengapa `vTaskDelay()` wajib digunakan menggantikan `delay()`.
* **Praktikum / Hands-on:**
  * Membuat 2 task paralel: Task 1 membaca sensor setiap 50 ms; Task 2 mengedipkan LED dan mengupdate display setiap 500 ms.
  * Eksperimen sengaja memicu Watchdog Timeout (membuat task sibuk tanpa *yield*) lalu memperbaikinya.
* **Asesmen:** Profiling konsumsi stack memori pada tiap task di serial monitor.

#### Minggu 8: EVALUASI TENGAH SEMESTER (UTS)
* **Sesi Teori (40 menit):** Analisis diagram timing bus serial, perhitungan register/divider, dan konsep penjadwalan prioritas task.
* **Sesi Praktik Lab Terpandu (100 menit):** *Live coding* modifikasi driver periferal dan implementasi multi-tasking FreeRTOS berbasis lembar kerja terstruktur.

#### Minggu 9: Komunikasi Antar-Task (IPC) & Sinkronisasi Aman
* **Materi Teori:**
  * Bahaya variabel global bersama: *Race Condition* dan korupsi data memori.
  * FreeRTOS Queue: Mengirim paket data antar-task secara thread-safe (prinsip FIFO).
  * Binary Semaphore: Sinkronisasi aksi berbasis pemicu kejadian (*event-driven*).
  * Mutex (*Mutual Exclusion*): Mengunci hak akses periferal fisik bersama (misal: proteksi jalur I2C agar tidak diakses dua task bersamaan).
  * Fenomena konkurensi: *Deadlock* dan pencegahan *Priority Inversion*.
* **Praktikum / Hands-on:**
  * Arsitektur Producer-Consumer: Task Sensor memproduksi data ke Queue; Task Logging mengambil data dari Queue untuk disimpan ke NVS/SD Card.
  * Melindungi akses display OLED bersama menggunakan Mutex.
* **Asesmen:** Menyelesaikan studi kasus sistem yang terkunci (*deadlock*) dan memulihkannya.

#### Minggu 10: Pemrograman Dual-Core ESP32 & Deferred Interrupt Processing
* **Materi Teori:**
  * Arsitektur Symmetric Multiprocessing (SMP): Core 0 (PRO_CPU) dan Core 1 (APP_CPU).
  * Task pinning: Menugaskan task ke core tertentu menggunakan `xTaskCreatePinnedToCore()`.
  * *Deferred Interrupt Processing*: Mengapa komputasi panjang dilarang di dalam ISR? Mendelegasikan pemrosesan dari ISR ke Task menggunakan *Direct Task Notification* (`vTaskNotifyGiveFromISR`).
* **Praktikum / Hands-on:**
  * Core 0 didedikasikan untuk tugas komunikasi data yang intensif; Core 1 didedikasikan untuk komputasi filter sinyal waktu nyata.
* **Asesmen:** Mengukur penurunan waktu latensi interupsi (*interrupt latency*) dengan deferred processing.

---

### FASE 4: DAYA RENDAH, IOT TELEMETRI, & KEAMANAN (MINGGU 11 - 13)

#### Minggu 11: Desain Sistem Bertenaga Baterai (*Low-Power Optimization*)
* **Materi Teori:**
  * Profil arus ESP32: Mode aktif (80-240 mA), Modem-sleep (~20 mA), Light-sleep (~0.8 mA), dan Deep-sleep (~10 µA).
  * Sumber pembangkit bangun (*Wake-up sources*): Timer RTC, GPIO external trigger (EXT0/EXT1).
  * Menyimpan variabel selama Deep Sleep di dalam `RTC_DATA_ATTR` (SRAM RTC).
  * Perhitungan matematis masa pakai baterai Li-Ion (mAh) berdasarkan *Duty Cycle*.
* **Praktikum / Hands-on:**
  * Merancang node sensor nirkabel: Bangun dari Deep Sleep, baca sensor dalam 50 ms, simpan ke RTC memory, kembali tidur lelap selama 10 menit.
  * Mengukur arus Deep Sleep secara riil menggunakan multimeter pada skala mikroampere ($\mu\text{A}$).
* **Asesmen:** Menghitung estimasi umur baterai 18650 pada sistem yang dirancang.

#### Minggu 12: Jaringan Nirkabel (Wi-Fi) & Protokol Telemetri MQTT yang Andal
* **Materi Teori:**
  * Wi-Fi State Machine pada ESP32: Event handler asinkron (`WIFI_EVENT_STA_CONNECTED`, `DISCONNECTED`).
  * Protokol IoT: Mengapa MQTT lebih hemat energi dan bandwidth dibanding HTTP/REST.
  * Format payload data: String, JSON, dan biner.
  * **Kaidah Keandalan:** Membangun *Auto-Reconnect Engine* agar sistem tidak hang saat router Wi-Fi mati.
* **Praktikum / Hands-on:**
  * Menghubungkan ESP32 ke broker MQTT (lokal Mosquitto / cloud EMQX) dan mempublikasikan data sensor secara periodik.
  * Uji ketahanan: Mematikan paksa access point Wi-Fi dan memverifikasi ESP32 berhasil tersambung kembali secara mandiri saat AP aktif.
* **Asesmen:** Implementasi Last Will & Testament (LWT) pada MQTT broker.

#### Minggu 13: Keamanan Sistem Tertanam & Over-The-Air (OTA) Firmware Update
* **Materi Teori:**
  * Aspek keamanan sistem tertanam: Bahaya kredensial plaintext dan modifikasi firmware bajakan.
  * MbedTLS: Enkripsi transmisi MQTT over TLS (Port 8883) menggunakan sertifikat CA root.
  * Konsep Over-The-Air (OTA) Update: Memperbarui firmware tanpa mencolokkan kabel USB.
  * Skema tabel partisi memori (`ota_0`, `ota_1`) dan mekanisme *Anti-Bricking Automatic Rollback*.
* **Praktikum / Hands-on:**
  * Melakukan kompilasi firmware baru dan mengunggahnya secara nirkabel melalui jaringan lokal Wi-Fi.
  * Uji rollback: Mengunggah file firmware rusak (*corrupt*), lalu mengamati ESP32 membatalkan update dan kembali ke partisi firmware lama yang aman.
* **Asesmen:** Demonstrasi OTA update sukses dan verifikasi status partisi aktif di terminal.

---

### FASE 5: KOMPUTASI TEPI, INTEGRASI PCB, & CAPSTONE (MINGGU 14 - 16)

#### Minggu 14: Bluetooth Low Energy (BLE) & Pengantar TinyML (Jalur Pengayaan)
* **Materi Teori:**
  * Dasar protokol Bluetooth Low Energy: Konsep GAP (Advertising) dan GATT (Services & Characteristics).
  * Pengantar TinyML: Menjalankan model machine learning yang sudah terkuantisasi (int8) langsung di dalam mikrokontroler.
* **Praktikum / Hands-on:**
  * Menjadikan ESP32 sebagai server BLE yang mengirimkan data telemetri ke aplikasi smartphone (nRF Connect).
  * Demonstrasi inferensi model klasifikasi sederhana (deteksi pola getaran abnormal pada motor listrik).
* **Asesmen:** Mengukur latensi waktu inferensi model pada ESP32.

#### Minggu 15: Rekayasa Hardware: Skematik, Desain PCB, dan Pencegahan Derau (EMC/EMI)
* **Materi Teori:**
  * Sirkuit minimum modul ESP32 mandiri: Strapping pins, rangkaian auto-reset (EN, DTR, RTS).
  * Perancangan Catu Daya (*Power Integrity*): Mengapa kapasitor decoupling keramik 100nF dan elektrolit 10µF wajib diletakkan sedekat mungkin dengan pin VDD chip untuk meredam lonjakan arus 500 mA saat radio Wi-Fi menyala.
  * Aturan layout PCB: Pemisahan jalur analog dan digital, *ground plane* utuh, dan zona bebas logam (*antenna keep-out zone*).
* **Aktivitas:**
  * *Design Review & Troubleshooting Clinic*: Mahasiswa mempresentasikan skematik sistem, layout PCB (KiCad/EasyEDA), dan diagram alir firmware capstone project.
* **Asesmen:** Rubrik kesiapan desain teknis skematik dan arsitektur firmware.

#### Minggu 16: EVALUASI AKHIR SEMESTER (UAS) - EXPO & DEFENSE CAPSTONE PROJECT
* **Bentuk Evaluasi:**
  * Pameran karya teknik (*Engineering Demo Day*) di laboratorium.
  * Setiap tim (2 mahasiswa) mendemonstrasikan prototipe sistem tertanam fisik yang berfungsi penuh di hadapan dosen dan tim asisten.
  * Sesi tanya jawab teknis (*defense*) mengenai arsitektur firmware, manajemen daya, dan reliabilitas perangkat keras.

---

## 7. SPESIFIKASI CAPSTONE PROJECT (TUGAS AKHIR KELOMPOK)

### Syarat Wajib Proyek (Jalur Inti):
1. **Multitasking:** Menggunakan minimal 3 FreeRTOS tasks yang berkomunikasi via Queue/Semaphore.
2. **Kekebalan Sistem:** Dilengkapi Task Watchdog Timer (TWDT) dan memori persisten NVS.
3. **Proteksi Fisik:** Dilengkapi driver penggerak beban yang benar (transistor + flyback diode).
4. **Komunikasi:** Mengirimkan data telemetri melalui salah satu protokol (MQTT over Wi-Fi, RS-485 Modbus, atau CAN Bus).
5. **Realisasi Fisik:** Diwujudkan dalam modul fisik (protoboard rapi atau PCB kustom), bukan breadboard acak-acakan.

### Pilihan Tema Proyek:
1. **Substation Battery Bank Health Monitor:** Monitoring tegangan multi-sel baterai secara presisi (ADC1), estimasi suhu, dan transmisi via Modbus RS-485 / Wi-Fi.
2. **Smart Industrial Motor Vibration Watcher:** Deteksi dini kerusakan motor induksi berbasis akselerometer I2C/SPI dengan sistem alarm relay dan logging NVS.
3. **Automotive CAN-Bus Logger & Telematics:** Membaca data kecepatan/RPM dari port OBD-II mobil (CAN Bus) dan menyimpannya secara terstruktur ke SD Card.
4. **Off-Grid Solar Energy Station:** Pengukur daya panel surya berbasis low-power sleep mode dengan pengiriman data nirkabel periodik.

---

## 8. BOBOT & KOMPONEN PENILAIAN

*(Komponen dan bobot penilaian akan disesuaikan kemudian sesuai ketentuan Program Studi / Fakultas).*

| Komponen Penilaian | Bobot | Rincian Penilaian |
| :--- | :---: | :--- |
| **Praktikum Lab Mingguan (Hands-on)** | - | - |
| **Kuis Pemahaman Teori & Diagnostik** | - | - |
| **Ujian Tengah Semester (UTS)** | - | - |
| **Capstone Project (UAS)** | - | - |
| **Soft Skills & Etika Enjiniring** | - | - |
| **TOTAL** | **-** | - |

---

## 9. DAFTAR PERALATAN & HARDWARE KIT LABORATORIUM

Setiap meja kerja lab dialokasikan perangkat keras berikut:
* **Board Utama:** ESP32-WROOM-32D Development Board (38-pin).
* **Alat Ukur & Diagnostik:**
  * 8-Channel USB Logic Analyzer 24 MHz (Didukung software open-source *PulseView/Sigrok*).
  * Multimeter Digital presisi dengan rentang ukur mikroampere ($\mu\text{A}$).
* **Komponen & Modul Sensor/Aktuator:**
  * Sensor Lingkungan: BME280 (I2C/SPI) atau AHT10/BMP280.
  * Sensor Gerak: MPU-6050 (I2C Akselerometer & Giroskop).
  * Display: Layar OLED 0.96 inci I2C (SSD1306).
  * Transceiver Industri: 1x Modul CAN SN65HVD230 (3.3V) dan 1x Modul RS-485 MAX3485 (3.3V).
  * Komponen Diskrit Penggerak: Transistor NPN 2N2222, N-Channel MOSFET 2N7000, Dioda 1N4007, Modul Relay 5V, Optocoupler PC817.
  * Pasif & Aksesori: Rotary encoder, push button, potensiometer 10k, LED, breadboard berkualitas baik, dan kabel jumper berkualitas.

---

## 10. DAFTAR REFERENSI

1. **Espressif Systems.** *ESP32 Technical Reference Manual & API Guides*. https://docs.espressif.com/projects/esp-idf/
2. **Barry, Richard.** (2020). *Mastering the FreeRTOS Real Time Kernel: A Hands-On Tutorial Guide*. Real Time Engineers Ltd.
3. **Ganssle, Jack.** (2008). *The Art of Designing Embedded Systems*. Newnes (Rujukan utama untuk debouncing, watchdog, dan keandalan firmware).
4. **Horowitz, Paul & Hill, Winfield.** *The Art of Electronics (3rd Edition)*. Cambridge University Press (Bab: Digital & Microcontroller Interfacing, Power Supply Decoupling).
5. **Monk, Simon.** (2018). *Programming Arduino: Getting Started with Sketches & Beyond* (Untuk jembatan pemahaman pemula).
