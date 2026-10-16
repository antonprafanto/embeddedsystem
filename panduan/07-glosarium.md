# Glosarium

> Kamus singkat istilah yang muncul di materi Sistem Tertanam. Istilah asing yang belum diserap ditulis miring dan diberi padanan Indonesia bila ada. Kolom **Dibahas di** menunjuk pertemuan tempat istilah itu dijelaskan paling lengkap.

Cara memakai: tekan **Ctrl + F** (atau **Cmd + F** di Mac) lalu ketik istilah yang kamu cari.

**Lompat ke huruf:** [A](#a) · [B](#b) · [C](#c) · [D](#d) · [E](#e) · [F](#f) · [G](#g) · [H](#h) · [I](#i) · [J](#j) · [K](#k) · [L](#l) · [M](#m) · [N](#n) · [O](#o) · [P](#p) · [Q](#q) · [R](#r) · [S](#s) · [T](#t) · [U](#u) · [V](#v) · [W](#w)

[P01]: ../pertemuan-01-pengenalan/README.md
[P02]: ../pertemuan-02-dasar-pemrograman/README.md
[P03]: ../pertemuan-03-gpio-output/README.md
[P04]: ../pertemuan-04-gpio-input/README.md
[P05]: ../pertemuan-05-waktu-interupsi/README.md
[P06]: ../pertemuan-06-adc/README.md
[P07]: ../pertemuan-07-pwm-beban/README.md
[P09]: ../pertemuan-09-komunikasi-serial/README.md
[P10]: ../pertemuan-10-sensor-aktuator/README.md
[P11]: ../pertemuan-11-fsm-debugging/README.md
[P12]: ../pertemuan-12-freertos/README.md
[P13]: ../pertemuan-13-wifi-mqtt/README.md
[P14]: ../pertemuan-14-daya-keandalan/README.md
[P15]: ../pertemuan-15-integrasi-sistem/README.md

## A

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **ADC** (*Analog-to-Digital Converter*) | Bagian mikrokontroler yang mengubah tegangan analog menjadi angka. ADC ESP32 12 bit menghasilkan angka 0–4095. | [P06] |
| **ADC1 / ADC2** | Dua unit ADC di ESP32. ADC1 ada di GPIO 32–39; ADC2 tidak bisa dipakai saat Wi-Fi aktif, jadi sensor analog sebaiknya di ADC1. | [P06] |
| ***Active-high* / *active-low*** | Cara menyalakan beban: *active-high* menyala saat pin `HIGH`, *active-low* menyala saat pin `LOW`. | [P03] |
| **Aktuator** | Komponen yang mengubah sinyal listrik menjadi aksi fisik, misalnya LED, *buzzer*, servo, motor, dan *relay*. | [P10] |
| **Alamat I2C** | Nomor 7 bit yang membedakan perangkat di satu bus I2C, misalnya OLED SSD1306 di `0x3C` dan MPU6050 di `0x68`. | [P09] |
| **Anoda / katode** | Kaki positif / negatif dioda dan LED. Pada LED, kaki panjang adalah anoda dan sisi rata adalah katode. | [P03] |
| **Antrean** (*queue*) | Struktur FreeRTOS untuk mengirim data antar-*task* secara aman, seperti antrean di loket. | [P12] |
| **Atenuasi** | Peredam di depan ADC agar tegangan yang lebih besar bisa diukur. Bawaan ESP32 11 dB (±150–3.100 mV). | [P06] |

## B

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| ***Backtrace*** | Daftar alamat fungsi yang sedang dipanggil saat program *crash*; membantu menemukan baris penyebabnya. | [P11] |
| ***Baud rate*** | Kecepatan komunikasi UART dalam bit per detik, misalnya 115200. Kedua pihak harus memakai nilai yang sama. | [P09] |
| **Bit / byte** | Bit = satu digit biner (0 atau 1). Byte = 8 bit. | [P02] |
| ***Board*** (papan) | Papan rangkaian siap pakai berisi mikrokontroler dan pendukungnya, misalnya ESP32 DevKit V1. | [P01] |
| ***Bootloader*** | Program kecil yang berjalan pertama kali saat cip menyala, lalu memuat *firmware* utama. | [P01] |
| ***Bouncing*** (pantulan) | Kontak tombol mekanis yang bergetar sesaat sehingga satu tekanan terbaca sebagai beberapa tekanan. | [P04] |
| ***Breadboard*** (papan rangkai) | Papan berlubang untuk merangkai komponen tanpa menyolder. | [P03] |
| ***Broker*** | Server MQTT yang menerima pesan dari pengirim dan meneruskannya ke pelanggan topik. | [P13] |
| ***Brownout*** | Kondisi tegangan catu turun di bawah batas aman; ESP32 akan *reset* untuk melindungi diri. | [P14] |
| ***Build*** | Proses mengompilasi kode sumber menjadi *firmware* yang siap diunggah. | [P01] |
| ***Busy-wait*** | Menunggu dengan perulangan kosong yang terus memakai CPU. Sebaiknya diganti jeda yang tidak memblokir. | [P12] |
| ***Buzzer*** | Komponen penghasil bunyi. *Buzzer* aktif berbunyi sendiri; *buzzer* pasif butuh sinyal PWM sehingga bisa memainkan nada. | [P07] |

## C

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **Cip** (*chip*) | Komponen sirkuit terpadu (IC), misalnya cip ESP32 di dalam modul ESP32-WROOM-32. | [P01] |
| ***Client ID*** | Nama unik setiap klien MQTT. Dua klien dengan ID sama akan saling memutus. | [P13] |
| ***Crash*** | Program berhenti mendadak karena galat fatal; di ESP32 biasanya muncul pesan *Guru Meditation Error* lalu *reset*. | [P11] |
| **CPU** | Bagian yang menjalankan instruksi program. ESP32 punya dua inti CPU hingga 240 MHz. | [P01] |

## D

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| ***Datasheet*** (lembar data) | Dokumen resmi pabrikan berisi spesifikasi, batas aman, pewaktuan, dan contoh rangkaian sebuah komponen. | [P10] |
| ***Deadlock*** | Dua *task* saling menunggu sumber daya yang dipegang pihak lain sehingga keduanya macet selamanya. | [P12] |
| ***Debouncing*** | Teknik mengabaikan pantulan tombol, misalnya dengan menunggu sinyal stabil ±50 ms. | [P04] |
| ***Debugging*** | Proses mencari dan memperbaiki kesalahan program. | [P11] |
| ***Deep sleep*** (tidur dalam) | Mode hemat daya ESP32 yang mematikan hampir semua bagian; bangun dari *deep sleep* sama seperti *reset*. | [P14] |
| **DHCP** | Layanan jaringan yang memberi alamat IP secara otomatis kepada perangkat. | [P13] |
| **DHT22** | Sensor suhu dan kelembapan digital dengan protokol satu kabel. Dibaca paling cepat tiap 2 detik. | [P10] |
| **Digital** | Sinyal yang hanya punya dua keadaan, `HIGH` (±3,3 V di ESP32) atau `LOW` (0 V). | [P03] |
| **Dioda *flyback*** | Dioda yang dipasang paralel terbalik dengan kumparan (*relay*, motor) untuk meredam tegangan induksi balik saat arus diputus. | [P07] |
| ***Duty cycle*** (siklus kerja) | Persentase waktu sinyal PWM bernilai `HIGH` dalam satu periode. | [P07] |

## E

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **eFuse** | Memori sekali tulis di cip ESP32 yang menyimpan data pabrik, termasuk data kalibrasi ADC. | [P06] |
| ***Embedded system*** (sistem tertanam) | Sistem komputer kecil yang tertanam di dalam perangkat untuk tugas khusus, misalnya pengendali mesin cuci. | [P01] |
| **EXT0 / EXT1** | Sumber bangun dari *deep sleep* lewat pin RTC GPIO: EXT0 untuk satu pin, EXT1 untuk beberapa pin. | [P14] |

## F

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| ***Fail-safe*** | Rancangan yang otomatis jatuh ke keadaan aman saat terjadi galat, misalnya *relay* mati bila data sensor tidak valid. | [P14] |
| ***Firmware*** | Program yang tertanam di memori mikrokontroler dan berjalan saat perangkat menyala. | [P01] |
| ***Flash*** | Memori tak-mudah-hilang tempat *firmware* dan data NVS disimpan. Isinya tetap ada walau catu dicabut. | [P01] |
| ***Floating*** (mengambang) | Pin input yang tidak tersambung ke apa pun sehingga nilainya acak. Diatasi dengan resistor *pull-up* atau *pull-down*. | [P04] |
| **FreeRTOS** | Sistem operasi waktu nyata yang dipakai ESP32 untuk menjalankan beberapa *task* sekaligus. | [P12] |
| **Frekuensi** | Banyaknya siklus per detik, dalam hertz (Hz). Frekuensi PWM 1 kHz berarti 1.000 periode per detik. | [P07] |
| **FSM** (*Finite State Machine*, mesin keadaan hingga) | Cara merancang program sebagai sejumlah keadaan dan aturan perpindahan antarkeadaan. | [P11] |

## G

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **Galat** (*error*) | Kesalahan, baik saat kompilasi maupun saat program berjalan. | [P02] |
| **GND** (*ground*) | Titik acuan 0 V. Semua perangkat yang saling berkomunikasi harus memakai GND bersama. | [P03] |
| **GPIO** (*General Purpose Input/Output*) | Pin serbaguna yang bisa diatur sebagai input atau output digital. | [P03] |
| ***Guru Meditation Error*** | Pesan *crash* khas ESP32, berisi jenis galat (mis. `LoadProhibited`), isi register, dan *backtrace*. | [P11] |

## H

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **HC-SR04** | Sensor jarak ultrasonik. Jarak dihitung dari lamanya gema: jarak (cm) ≈ lebar pulsa ECHO (µs) ÷ 58. | [P10] |
| ***Header*** (berkas `.h`) | Berkas berisi deklarasi fungsi, konstanta, dan tipe yang dipakai bersama oleh beberapa berkas `.cpp`. | [P11] |
| ***Hibernation*** | Mode daya paling hemat ESP32 (±5 µA cip) dengan bagian yang aktif lebih sedikit daripada *deep sleep*. | [P14] |
| ***High water mark*** | Sisa *stack* paling sedikit yang pernah dimiliki sebuah *task*; dipakai untuk memilih ukuran *stack* yang aman. | [P12] |
| **Histeresis** | Memakai dua ambang berbeda (naik dan turun) agar keluaran tidak berkedip-kedip saat nilai sensor berada di sekitar batas. | [P06] |

## I

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **I2C** | Bus komunikasi serial dua kabel (SDA dan SCL) yang bisa dipakai banyak perangkat sekaligus dengan alamat berbeda. | [P09] |
| **IDE** (*Integrated Development Environment*) | Aplikasi untuk menulis, mengompilasi, dan mengunggah program, misalnya VS Code + PlatformIO. | [P01] |
| **Interupsi** | Sinyal yang membuat CPU menunda pekerjaan sejenak untuk menjalankan fungsi khusus (ISR), lalu kembali lagi. | [P05] |
| **IoT** (*Internet of Things*) | Perangkat fisik yang terhubung ke internet untuk mengirim data atau dikendalikan dari jauh. | [P13] |
| `IRAM_ATTR` | Penanda agar fungsi (biasanya ISR) disimpan di RAM internal sehingga bisa dijalankan cepat dan aman. | [P05] |
| **ISR** (*Interrupt Service Routine*) | Fungsi yang dijalankan saat interupsi terjadi. Harus singkat dan tidak boleh memakai `delay()` atau `Serial.print()`. | [P05] |

## J

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **JSON** | Format teks untuk data terstruktur, misalnya `{"suhu":27.5}`. Sering dipakai sebagai isi pesan MQTT. | [P13] |

## K

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **Kalibrasi** | Menyesuaikan hasil ukur dengan acuan yang benar, misalnya membandingkan bacaan ADC dengan multimeter. | [P06] |
| ***Keep alive*** | Selang waktu klien MQTT mengirim tanda "masih hidup" ke *broker*. | [P13] |
| **Kompilasi** (*compile*) | Menerjemahkan kode C/C++ menjadi kode mesin yang dimengerti mikrokontroler. | [P02] |
| ***Keypad*** | Papan tombol matriks (mis. 4×4) yang dibaca baris demi kolom. | [P11] |

## L

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **Larik** (*array*) | Kumpulan data sejenis yang diakses dengan indeks mulai dari 0. | [P02] |
| **LDR** (*Light Dependent Resistor*) | Resistor yang hambatannya turun saat cahaya bertambah terang. | [P06] |
| **LED** (*Light Emitting Diode*) | Dioda yang memancarkan cahaya. Selalu butuh resistor pembatas arus. | [P03] |
| **LEDC** | Periferal PWM di ESP32 (16 kanal) untuk LED, *buzzer*, servo, dan beban lain. | [P07] |
| ***Library*** (pustaka) | Kumpulan kode siap pakai, misalnya untuk OLED atau DHT22. Dipasang lewat `lib_deps` (PlatformIO) atau `libraries.txt` (Wokwi). | [P10] |
| ***Light sleep*** | Mode hemat daya ESP32 (±0,8 mA cip) yang membekukan CPU tetapi menjaga isi RAM. | [P14] |
| ***Log level*** | Tingkat pesan *log* (`ERROR`, `WARN`, `INFO`, `DEBUG`, `VERBOSE`) yang bisa disaring saat kompilasi. | [P11] |
| `loop()` | Fungsi Arduino yang dijalankan berulang-ulang selamanya setelah `setup()`. | [P01] |
| **LWT** (*Last Will and Testament*) | Pesan "wasiat" yang dikirim *broker* MQTT atas nama klien bila klien terputus tanpa pamit. | [P13] |

## M

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **Mikrokontroler** | Komputer kecil dalam satu cip (CPU, memori, dan periferal), misalnya ESP32. | [P01] |
| `millis()` / `micros()` | Fungsi yang mengembalikan waktu sejak board menyala dalam milidetik / mikrodetik. Dasar teknik tanpa `delay()`. | [P05] |
| **Modul** | Rangkaian kecil siap pakai, misalnya modul ESP32-WROOM-32 atau modul *relay*. | [P01] |
| **MPU6050** | Sensor akselerometer dan giroskop yang terhubung lewat I2C. | [P09] |
| **MQTT** | Protokol IoT ringan berpola *publish/subscribe* lewat perantara *broker*. | [P13] |
| ***Mutex*** | Kunci FreeRTOS agar hanya satu *task* memakai sumber daya bersama (mis. `Serial`) pada satu waktu. | [P12] |

## N

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| ***Non-blocking*** | Gaya kode yang tidak menunggu dengan `delay()`, sehingga program tetap bisa mengerjakan hal lain. | [P05] |
| **NTC** (termistor) | Resistor peka suhu yang hambatannya turun saat suhu naik. Suhu dihitung dengan persamaan Beta. | [P06] |
| **NVS** (*Non-Volatile Storage*) | Penyimpanan kunci–nilai di *flash* ESP32, diakses lewat pustaka `Preferences`. Isinya tetap ada saat catu dicabut. | [P14] |

## O

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **OLED SSD1306** | Layar kecil 128×64 piksel yang dikendalikan lewat I2C. | [P09] |
| ***Open-drain*** | Jenis keluaran yang hanya bisa menarik ke GND; butuh resistor *pull-up* untuk logika `HIGH`. Dipakai di bus I2C. | [P09] |
| ***Overflow*** (luapan) | Nilai melebihi batas tipe datanya lalu "berputar" kembali, misalnya `uint8_t` 255 + 1 = 0. | [P02] |

## P

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| ***Payload*** | Isi data sebuah pesan, misalnya teks JSON yang dikirim lewat MQTT. | [P13] |
| **Pembagi tegangan** | Dua resistor seri untuk menurunkan tegangan: V<sub>out</sub> = V<sub>in</sub> × R2 ÷ (R1 + R2). | [P06] |
| **Peta pin** (*pinout*) | Gambar atau tabel fungsi setiap pin board. Lihat [Lembar Saku Pinout ESP32](05-pinout-esp32.md). | [P01] |
| **Pin *strapping*** | Pin yang dibaca saat *boot* untuk menentukan mode cip (GPIO 0, 2, 5, 12, 15). Jangan ditarik paksa saat menyala. | [P03] |
| **PlatformIO** | Ekstensi VS Code untuk mengompilasi, mengunggah, dan memantau program mikrokontroler. | [P01] |
| ***Polling*** | Memeriksa keadaan secara berulang-ulang. Kebalikan dari interupsi. | [P05] |
| **Potensiometer** | Resistor dengan kenop putar; dipakai sebagai pembagi tegangan yang bisa diatur. | [P06] |
| ***Preemption*** | Kemampuan penjadwal menghentikan *task* berprioritas rendah agar *task* berprioritas lebih tinggi bisa berjalan. | [P12] |
| ***Publish / subscribe*** | Pola MQTT: pengirim menerbitkan (*publish*) pesan ke topik; penerima berlangganan (*subscribe*) topik itu. | [P13] |
| ***Pull-up* / *pull-down*** | Resistor yang menarik pin input ke 3,3 V (*pull-up*) atau ke GND (*pull-down*) agar tidak mengambang. | [P04] |
| **PWM** (*Pulse Width Modulation*) | Sinyal kotak yang lebar pulsanya diatur untuk mengendalikan kecerahan LED, kecepatan motor, atau sudut servo. | [P07] |

## Q

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **QoS** (*Quality of Service*) | Tingkat jaminan pengiriman pesan MQTT: 0 (paling banyak sekali), 1 (minimal sekali), 2 (tepat sekali). | [P13] |
| ***Queue*** | Lihat **Antrean**. | [P12] |

## R

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| ***Race condition*** | Hasil program bergantung pada urutan kerja dua *task*/ISR yang berebut data yang sama. | [P12] |
| **RAM / SRAM** | Memori kerja yang isinya hilang saat catu dicabut. ESP32 punya SRAM 520 KB. | [P01] |
| ***Real-time*** (waktu nyata) | Sistem yang wajib merespons dalam batas waktu tertentu. | [P12] |
| **Register** | Lokasi memori khusus di dalam cip untuk mengatur periferal, misalnya register keluaran GPIO. | [P03] |
| ***Relay*** | Sakelar elektromekanis yang dikendalikan kumparan, untuk menyalakan beban bertegangan/berarus besar. | [P07] |
| **Resistor** | Komponen penghambat arus. Nilainya dibaca dari gelang warna, misalnya merah-merah-cokelat = 220 Ω. | [P03] |
| **Resolusi** | Banyaknya tingkat nilai yang bisa dibedakan: ADC 12 bit = 4.096 tingkat; PWM 8 bit = 256 tingkat. | [P06] |
| ***Retained message*** | Pesan MQTT yang disimpan *broker* dan langsung dikirim ke pelanggan baru topik itu. | [P13] |
| ***Rollover*** | Penghitung yang kembali ke 0 setelah mencapai batasnya, misalnya `millis()` setelah ±49,7 hari. | [P05] |
| **RSSI** | Kuat sinyal Wi-Fi yang diterima, dalam dBm. Makin mendekati 0, makin kuat. | [P13] |
| **RTC** (*Real-Time Clock*) | Bagian ESP32 yang tetap hidup saat *deep sleep*, termasuk memori RTC (`RTC_DATA_ATTR`). | [P14] |

## S

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **Sensor** | Komponen yang mengubah besaran fisik (cahaya, suhu, jarak) menjadi sinyal listrik. | [P10] |
| **Serial Monitor** | Jendela untuk melihat (dan mengirim) teks lewat UART antara komputer dan board. | [P01] |
| **Servo** | Motor yang sudutnya dikendalikan lebar pulsa PWM 50 Hz. | [P07] |
| `setup()` | Fungsi Arduino yang dijalankan sekali saat board menyala atau di-*reset*. | [P01] |
| **SPI** | Bus serial cepat empat kabel (MOSI, MISO, SCK, CS) dengan satu kabel CS per perangkat. | [P09] |
| ***Stack*** (tumpukan) | Memori sementara milik sebuah *task* untuk variabel lokal dan pemanggilan fungsi. Di ESP-IDF ukurannya ditulis dalam byte. | [P12] |
| ***Stack overflow*** | *Stack* habis karena terlalu kecil atau rekursi tanpa henti; menyebabkan *crash*. | [P11] |
| ***State*** (keadaan) | Satu kondisi dalam FSM, misalnya MERAH, KUNING, HIJAU pada lampu lalu lintas. | [P11] |
| **STA / AP** | Mode Wi-Fi ESP32: STA (*station*) bergabung ke jaringan yang ada; AP (*access point*) membuat jaringan sendiri. | [P13] |

## T

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| ***Task*** (tugas) | Satu alur program yang dijadwalkan FreeRTOS; tiap *task* punya *stack* dan prioritas sendiri. | [P12] |
| ***Tick*** | Satuan waktu penjadwal FreeRTOS. Di ESP32 Arduino 1 *tick* = 1 ms. | [P12] |
| ***Timer*** (pewaktu) | Penghitung perangkat keras yang bisa memicu interupsi secara berkala dengan tepat. | [P05] |
| **Tipe data** | Jenis nilai yang disimpan variabel, misalnya `int`, `float`, `bool`, `uint8_t`. | [P02] |
| **Topik** (*topic*) | "Alamat" pesan MQTT bertingkat, misalnya `kuliah-st/NIM/sensor`. Mendukung *wildcard* `+` dan `#`. | [P13] |
| **Transistor NPN** | Komponen penguat/sakelar; arus kecil di basis mengendalikan arus besar di kolektor. | [P07] |

## U

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| **UART** | Komunikasi serial asinkron dua kabel (TX dan RX) dengan format bingkai seperti 8N1. Dipakai `Serial`. | [P09] |
| **Unggah** (*upload*) | Mengirim *firmware* dari komputer ke board lewat kabel USB. | [P01] |
| **USB-UART** | Cip penerjemah USB ke UART di board (CP2102 atau CH340), butuh *driver* di komputer. | [P01] |

## V

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| `volatile` | Kata kunci C/C++ untuk variabel yang bisa diubah di luar alur normal (mis. oleh ISR), agar kompilator selalu membacanya ulang. | [P05] |
| **V<sub>f</sub>** (tegangan maju) | Tegangan yang "dimakan" LED saat menyala, mis. ±2 V untuk LED merah. | [P03] |

## W

| Istilah | Arti singkat | Dibahas di |
| --- | --- | :---: |
| ***Watchdog*** | Pengawas yang me-*reset* sistem bila program macet dan lupa "memberi makan" tepat waktu. | [P14] |
| **Wi-Fi** | Jaringan nirkabel. ESP32 hanya mendukung pita 2,4 GHz. | [P13] |
| ***Wildcard*** | Karakter pengganti pada topik MQTT: `+` untuk satu tingkat, `#` untuk semua tingkat di bawahnya. | [P13] |
| ***Wiring*** | Sambungan kabel antara board dan komponen. | [P03] |
| **Wokwi** | Simulator elektronika di peramban untuk mencoba program ESP32 tanpa perangkat keras. Lihat [Simulasi dengan Wokwi](03-simulasi-wokwi.md). | [P01] |
| **Wokwi-GUEST** | Jaringan Wi-Fi tiruan di Wokwi (tanpa sandi, kanal 6) untuk mencoba program IoT. | [P13] |

---

[⬅️ Mengatasi Masalah](06-mengatasi-masalah.md) · [🏠 Beranda](../README.md)
