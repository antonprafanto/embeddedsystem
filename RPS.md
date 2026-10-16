# Rencana Pembelajaran Semester (RPS) — Sistem Tertanam

> Dokumen ini merangkum capaian, jadwal, dan cara penilaian mata kuliah. Bagian yang bertanda *(sesuaikan)* dapat disesuaikan dengan ketentuan program studi.

## 1. Identitas Mata Kuliah

| | |
| --- | --- |
| Nama mata kuliah | Sistem Tertanam (*Embedded Systems*) |
| Program studi | S1 Teknik Elektro |
| Bobot SKS | *(sesuaikan)* — disarankan 3 SKS (2 teori + 1 praktikum) |
| Dosen pengampu | Anton Prafanto |
| Prasyarat yang disarankan | Rangkaian Listrik, Elektronika Dasar, Sistem Digital, Algoritma dan Pemrograman |
| Perangkat keras | ESP32 DevKit V1 dan kit komponen (lihat [Daftar Komponen](panduan/04-daftar-komponen.md)) |
| Perangkat lunak | Visual Studio Code + PlatformIO (Arduino-ESP32 3.3), simulator Wokwi |

## 2. Deskripsi Singkat

Mata kuliah ini membekali mahasiswa dengan kemampuan merancang dan memprogram sistem tertanam berbasis mikrokontroler ESP32. Pembelajaran dimulai dari konsep dasar dan bahasa C/C++, berlanjut ke antarmuka input/output digital dan analog, pewaktuan dan interupsi, komunikasi serial, pemanfaatan sensor dan aktuator, perancangan *firmware* terstruktur dengan *finite state machine* dan FreeRTOS, konektivitas IoT (Wi-Fi dan MQTT), hingga manajemen daya dan keandalan. Mata kuliah ditutup dengan proyek akhir terintegrasi yang didemonstrasikan dan dipresentasikan.

## 3. Capaian Pembelajaran Mata Kuliah (CPMK)

| Kode | Setelah mengikuti mata kuliah ini, mahasiswa mampu … |
| --- | --- |
| CPMK-1 | menjelaskan konsep sistem tertanam, arsitektur mikrokontroler ESP32, dan alur pengembangan *firmware*. |
| CPMK-2 | menulis program C/C++ untuk mengendalikan GPIO, pewaktuan, dan interupsi secara benar dan aman. |
| CPMK-3 | mengantarmukakan sensor dan aktuator analog/digital beserta perhitungan kelistrikannya. |
| CPMK-4 | menerapkan komunikasi serial (UART, I2C, SPI) serta memanfaatkan *library* dan lembar data (*datasheet*). |
| CPMK-5 | merancang *firmware* yang terstruktur, andal, dan hemat daya (FSM, FreeRTOS, *deep sleep*, *watchdog*). |
| CPMK-6 | membangun sistem IoT sederhana dan mempresentasikan proyek sistem tertanam yang terintegrasi. |

## 4. Rencana Pertemuan

| Pertemuan | Topik | CPMK | Praktikum | Penilaian |
| :---: | --- | :---: | --- | --- |
| 1 | Mengenal sistem tertanam, ESP32, dan alat kerja | 1 | Halo ESP32 (Serial), LED berkedip | Kuis 1 |
| 2 | Dasar pemrograman C/C++ untuk mikrokontroler | 2 | Variabel dan tipe data, operator dan bit, percabangan dan perulangan, fungsi dan larik, kalkulator Serial | Laporan praktikum |
| 3 | GPIO output: mengendalikan LED | 2, 3 | LED eksternal, lampu lalu lintas, LED berjalan | Laporan praktikum |
| 4 | GPIO input: membaca tombol | 2, 3 | Tombol + LED, *toggle* dengan *debouncing*, penghitung tekanan | Kuis 2 |
| 5 | Waktu dan interupsi | 2 | Dua LED tanpa `delay()`, interupsi tombol, *timer* perangkat keras, gim reaksi | Laporan praktikum |
| 6 | Input analog: ADC | 3 | Voltmeter potensiometer, lampu otomatis LDR, termometer NTC | Laporan praktikum |
| 7 | PWM dan mengendalikan beban | 3 | LED redup-terang, *dimmer*, servo, *buzzer*, *relay* dengan transistor | Kuis 3 |
| 8 | **Ujian Tengah Semester** | 1–3 | Ujian teori + praktik | UTS |
| 9 | Komunikasi serial: UART, I2C, SPI | 4 | Perintah Serial, pemindai I2C, OLED, MPU6050 | Laporan praktikum |
| 10 | Sensor, aktuator, *library*, dan *datasheet* | 4 | DHT22 + OLED, HC-SR04, sensor parkir | Tugas membaca *datasheet* |
| 11 | *Firmware* rapi: FSM dan *debugging* | 5 | Lampu lalu lintas FSM, kunci pintu *keypad*, membaca *crash* | Laporan praktikum |
| 12 | *Multitasking* dengan FreeRTOS | 5 | Dua *task*, *queue* sensor, *mutex* | Kuis 4 |
| 13 | IoT: Wi-Fi dan MQTT | 6 | Pindai Wi-Fi, kirim data MQTT, kendali LED dari internet | Laporan praktikum |
| 14 | Hemat daya dan keandalan | 5 | *Deep sleep* + *timer*, bangun dengan tombol, simpan pengaturan di NVS, *watchdog* | Proposal proyek akhir |
| 15 | Integrasi sistem dan persiapan proyek akhir | 6 | Uji perangkat keras kit, stasiun monitor mini terintegrasi, konsultasi proyek | Laporan kemajuan proyek |
| 16 | **Ujian Akhir Semester**: demo dan presentasi proyek | 1–6 | Demo proyek akhir | UAS (proyek) |

## 5. Metode Pembelajaran

- **Kelas konsep** dengan analogi, demonstrasi langsung, dan kuis singkat.
- **Praktikum terbimbing** dua jalur: simulator Wokwi dan board asli.
- **Pembelajaran berbasis proyek** pada paruh kedua semester, berujung pada proyek akhir berkelompok.
- **Uji pemahaman langsung**: saat praktikum, dosen/asisten dapat meminta mahasiswa mengubah sebagian kode dan menjelaskannya.

## 6. Komponen Penilaian *(usulan, sesuaikan)*

| Komponen | Bobot usulan | Keterangan |
| --- | :---: | --- |
| Laporan dan demo praktikum | 25% | Per praktikum: kode berjalan, penjelasan, jawaban pertanyaan |
| Kuis | 10% | 4 kuis singkat |
| Ujian Tengah Semester | 25% | Teori (konsep dan perhitungan) + praktik |
| Proyek akhir (UAS) | 35% | Proposal (10%), kemajuan (10%), demo + presentasi + laporan akhir (80%) dari nilai proyek |
| Keaktifan dan etika | 5% | Kehadiran, kerapian kerja, kejujuran akademik |

Rubrik proyek akhir dijelaskan di [Proyek Akhir](proyek-akhir/README.md) dan [Pertemuan 16](pertemuan-16-uas/README.md).

## 7. Kebijakan Kelas

- **Integritas akademik.** Memakai bantuan teman, internet, atau AI diperbolehkan untuk belajar, tetapi setiap mahasiswa wajib mampu menjelaskan seluruh kode yang dikumpulkan dan mencantumkan bantuan AI yang dipakai di laporan. Kode yang tidak bisa dijelaskan dianggap bukan karya sendiri.
- **Keselamatan kerja.** Selalu cabut catu daya sebelum mengubah rangkaian. Laporkan komponen yang panas atau berbau gosong.
- **Kehadiran praktikum** mengikuti ketentuan program studi *(sesuaikan)*.

## 8. Referensi Utama

1. Espressif Systems. (2025). *ESP32 Series Datasheet*. <https://documentation.espressif.com/esp32_datasheet_en.pdf>
2. Espressif Systems. (2026). *Arduino-ESP32 Documentation*. <https://docs.espressif.com/projects/arduino-esp32/en/latest/>
3. Marwedel, P. (2021). *Embedded System Design* (4th ed.). Springer (akses terbuka). <https://doi.org/10.1007/978-3-030-60910-8>
4. Lee, E. A., & Seshia, S. A. (2017). *Introduction to Embedded Systems: A Cyber-Physical Systems Approach* (2nd ed.). MIT Press. <https://ptolemy.berkeley.edu/books/leeseshia/>
5. Barry, R. (2016, diperbarui daring). *Mastering the FreeRTOS™ Real Time Kernel: A Hands-On Tutorial Guide* (FreeRTOS Kernel Book). <https://github.com/FreeRTOS/FreeRTOS-Kernel-Book>
6. Dokumentasi PlatformIO <https://docs.platformio.org> dan Wokwi <https://docs.wokwi.com>.

---

[🏠 Beranda](README.md)
