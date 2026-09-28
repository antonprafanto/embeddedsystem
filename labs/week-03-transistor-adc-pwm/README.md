# 📝 JOBSHEET PRAKTIKUM MINGGU 3: INTERFACING BEBAN DAYA (TRANSISTOR DRIVER) & PERIFERAL ANALOG (ADC1 & PWM)
### Pengendalian Beban Arus Besar, Akurasi Sensor ADC1 Tanpa Benturan Wi-Fi, dan Modulasi Lebar Pulsa (LEDC) pada ESP32

> **Mata Kuliah:** Sistem Tertanam (*Embedded Systems*)  
> **Target Pengguna:** Mahasiswa S1 Teknik Elektro (Tingkat Pemula / Awam)  
> **Prasyarat:** Telah menyelesaikan [Modul Minggu 0: Onboarding](../week-00-onboarding/README.md), [Modul Minggu 1: Bitwise C](../week-01-bitwise-c/README.md), [Modul Minggu 2: GPIO & Interupsi](../week-02-gpio-interrupts/README.md), serta memahami [Lembar Saku Pinout ESP32](../../docs/esp32_pin_gotchas.md)  
> **Alokasi Waktu:** 170 Menit (Sesi Lab Terpandu / Mandiri)  
> **Target Board:** ESP32 Development Board (ESP32-WROOM-32 / ESP32-S3)  

---

Halo rekan-rekan mahasiswa Teknik Elektro! 👋  
Selamat datang di modul praktikum minggu ketiga. Di dua minggu pertama, kita telah menguasai manipulasi bitwise register, logika interupsi perangkat keras, dan teknik membaca crash log sistem. Sekarang, kita melangkah ke ranah yang menjadi ciri khas sejati seorang insinyur elektro: **menjembatani otak mikrokontroler digital berdaya rendah (3.3V) dengan dunia fisik perangkat keras berdaya besar (5V/12V, motor, relay, solenoid, dan sensor analog kontinu)**.

Modul ini dirancang agar sangat ramah bagi pemula, disajikan dengan bahasa santai namun tetap presisi secara kaidah teknik elektro, serta dilengkapi diagram visual beresolusi tinggi.

---

### 🌟 Mengapa Materi Minggu Ini Sangat Krusial?

Di dunia industri nyata, mikrokontroler tidak pernah bekerja sendirian di atas meja yang tenang. Mikrokontroler harus membaca sensor analog (suhu, tekanan, sudut potensiometer) dan menggerakkan aktuator bertenaga besar (pompa air, konveyor pabrik, kipas pendingin, atau saklar pemutus daya). 

Berikut 3 analogi santai untuk memahami 3 konsep inti minggu ini:

#### 1. Analogi Keran Air Hidrolik (Transistor Driver):
Bayangkan pin GPIO ESP32 seperti jari tangan anak kecil yang hanya kuat menekan tuas katup kecil (arus lemah ≤ 12 mA). Sedangkan motor DC atau koil relay adalah pintu air bendungan raksasa yang membutuhkan dorongan ribuan liter air (arus 150 mA hingga puluhan Ampere). 
Jika jari anak kecil itu dipaksa menahan air bendungan secara langsung, jarinya akan patah seketika (**pin GPIO ESP32 terbakar permanen!**).  
Solusinya? Kita memasang **Transistor BJT (2N2222) atau MOSFET (2N7000)**. Jari anak kecil cukup membuka katup kendali kecil (Basis), dan transistor yang akan mengalirkan air deras dari tandon utama (Kolektor ke Emitor) menuju motor.

#### 2. Analogi Rel Kereta Api & Sinyal Radio (Aturan Emas ADC1 vs ADC2):
ESP32 memiliki dua unit konverter analog-ke-digital: **ADC1** dan **ADC2**.
* **ADC1** diibaratkan rel kereta api jalur khusus industri yang sepenuhnya independen.
* **ADC2** diibaratkan rel kereta api yang berbagi wesel dengan stasiun radio Wi-Fi & Bluetooth!  
Ketika fitur Wi-Fi dinyalakan (untuk mengirim data ke cloud atau MQTT), chip radio ESP32 akan menyita sirkuit ADC2 secara paksa untuk kalibrasi daya antena. Jika program Anda nekat membaca sensor di pin ADC2 saat Wi-Fi aktif, pembacaan analog Anda akan langsung macet, membeku, atau menghasilkan data sampah. Oleh karena itu, aturan emas insinyur ESP32 sedunia: **HANYA gunakan pin ADC1 (GPIO 32 s.d. 39) untuk sensor analog Anda!**

#### 3. Analogi Saklar Lampu Kamar Cepat (Modulasi Lebar Pulsa / PWM):
Bagaimana cara meredupkan lampu pijar atau memperlambat laju putaran motor DC jika mikrokontroler kita hanya mengenal angka digital 0V (Mati) dan 3.3V (Hidup)?  
Mikrokontroler tidak bisa menurunkan tegangan murni menjadi 1.5V layaknya baterai yang melemah. Sebagai gantinya, mikrokontroler menyalakan dan mematikan saklar listrik dengan kecepatan luar biasa tinggi: **5.000 kali dalam satu detik (5 kHz)**.  
Karena inersia mekanik rotor motor dan keterbatasan mata manusia, motor tidak terasa tersentak-sentak hidup-mati, melainkan berputar halus pada kecepatan sedang sesuai rasio perbandingan waktu nyala (*Duty Cycle*).

---

## 🗺️ Alur Praktikum Minggu Ini (Workflow)

```mermaid
flowchart LR
    A["🔌 1. Rangkai Potensio<br>& Driver di Breadboard"] --> B["⚙️ 2. Konfigurasi ADC1<br>& Filter Multisampling"]
    B --> C["📈 3. Setup Hardware PWM<br>Timer LEDC (5 kHz)"]
    C --> D["🧪 4. Uji Pengendalian<br>Beban Daya (Level 1)"]
    D --> E["🛡️ 5. Analisis Back-EMF<br>& Deadband (Level 2 & 3)"]
    E --> F["📤 6. Live Lab Demo &<br>Git Commit / Push"]
```

---

## 🧰 Alat dan Komponen yang Dibutuhkan

Sebelum memulai praktikum, pastikan komponen-komponen berikut sudah tersedia di meja lab Anda:

| No | Komponen / Perangkat | Jumlah | Keterangan Praktis |
|:--:|:---|:--:|:---|
| 1 | **ESP32 DevKit Board** | 1 unit | 30-pin atau 38-pin (ESP32-WROOM-32 / ESP32-S3) |
| 2 | **Kabel Micro-USB / Type-C** | 1-2 unit | Pastikan kabel mendukung transfer data, bukan hanya pengisian daya |
| 3 | **Breadboard 830 Titik** | 1 unit | Papan tempat merangkai komponen tanpa solder |
| 4 | **Potensiometer 10 kΩ** | 1 unit | Tipe putar putaran linier (3 kaki) sebagai input sensor |
| 5 | **Transistor NPN (2N2222 / BC547)** | 1 unit | Saklar penggerak beban daya (alternatif: MOSFET 2N7000) |
| 6 | **Dioda Flyback (1N4007 / 1N4148)** | 1 unit | Dioda penyearah silikon untuk peredam lonjakan induktif |
| 7 | **Resistor 1 kΩ (Cokelat-Hitam-Merah)** | 1 unit | Pembatas arus basis transistor (`R_B`) |
| 8 | **Modul Relay 5V atau Motor DC Kecil (3V-5V)** | 1 unit | Beban daya induktif utama |
| 9 | **LED 5mm + Resistor 330 Ω** | 1 set | (*Opsional*) Beban visual alternatif jika motor belum tersedia |
| 10 | **Sumber Daya 5V Eksternal (Power Bank / Breadboard Power Module)** | 1 unit | Untuk menyuplai motor/relay secara terpisah dari ESP32 |
| 11 | **Kabel Jumper Male-to-Male** | 10-15 buah | Untuk menghubungkan jalur di breadboard |
| 12 | **Multimeter Digital (DMM)** | 1 unit | Untuk mengukur tegangan rel dan hambatan resistor |

---

## 📚 1. FONDASI TEORI ELEKTRO: DRIVER TRANSISTOR & PROTEKSI INDUKTIF

### A. Mengapa Pin GPIO Tidak Boleh Menyetir Beban Arus Besar?
Sirkuit internal pin GPIO ESP32 dibangun dari transistor silikon mikroskopis berskala nanometer. Arus keluaran yang disarankan pabrikan Espressif adalah **maksimal 12 mA per pin** (batas absolut mutlak 20 mA sebelum silikon meleleh).

Mari kita bandingkan kebutuhan arus beban nyata:
* Satu buah LED indikator: membutuhkan arus ~5 mA s.d. 10 mA (**Aman** untuk GPIO).
* Satu koil modul relay 5V standar: membutuhkan arus ~70 mA s.d. 100 mA (**6× lipat melebihi batas aman!**).
* Satu motor DC hobi mini (3V-5V): membutuhkan arus tanpa beban ~150 mA, dan arus lonjakan start-up (*stall current*) mencapai **500 mA s.d. 1.000 mA** (**50× lipat melebihi batas aman!**).

Menghubungkan koil relay atau motor DC langsung ke pin ESP32 akan langsung membakar gerbang silikon output GPIO, memicu drop tegangan sistemik, atau menyebabkan mikrokontroler mengalami restart terus-menerus (*Brownout Reset*).

---

### B. Transistor BJT (2N2222) Sebagai Saklar Elektronik
Transistor Bipolar Junction Transistor (NPN) memiliki 3 terminal: **Basis (B)**, **Kolektor (C)**, dan **Emitor (E)**. 
Dalam elektronika digital, kita mengoperasikan transistor pada dua wilayah kerja ekstrem:
1. **Wilayah Cutoff (Saklar Terbuka / OFF):**  
   Ketika tegangan Basis-Emitor (`V_BE`) < 0.7V (GPIO bernilai `LOW` / 0V), tidak ada arus basis yang mengalir (`I_B = 0`). Jalur Kolektor-Emitor tertutup total layaknya saklar terbuka. Beban tidak mendapatkan arus listrik.
2. **Wilayah Saturasi (Saklar Tertutup Penuh / ON):**  
   Ketika pin GPIO bernilai `HIGH` (3.3V), arus kecil disuntikkan ke terminal Basis melalui resistor pembatas `R_B`. Transistor mengalami kejenuhan penuh (*saturation*), di mana resistansi antara terminal Kolektor dan Emitor turun mendekati nol (`V_CE(sat) ≈ 0.1V - 0.2V`). Arus besar dari power supply mengalir bebas melalui beban menuju Ground.

#### Menghitung Resistor Basis (`R_B`):
Agar transistor benar-benar jenuh (*fully saturated*), arus basis harus dirancang dengan rasio penguatan saturasi (`β_sat` atau `h_FE(sat) ≈ 10` sampai 20):
* Misalkan arus beban kolektor `I_C = 150 mA = 0.15 A`.
* Arus basis yang dibutuhkan: `I_B = I_C / 10 = 15 mA` (atau untuk transistor gain tinggi seperti 2N2222, `I_B ≈ 2.6 mA - 5 mA` sudah cukup membuat saturasi).
* Tegangan drop Basis-Emitor silikon: `V_BE ≈ 0.7 V`.
* Tegangan output pin ESP32: `V_GPIO = 3.3 V`.
* Nilai resistor basis:
  ```text
  R_B = (V_GPIO - V_BE) / I_B 
      = (3.3V - 0.7V) / 0.0026A 
      ≈ 1.000 Ω = 1 kΩ
  ```

Maka, memasang resistor **1 kΩ** di kaki Basis adalah pilihan yang sangat aman dan presisi!

---

### C. Fenomena Back-EMF & Kewajiban Memasang Dioda Flyback (1N4007)
Koil relay, solenoid, dan kumparan rotor motor DC adalah **beban induktif murni** (`L`). Kumparan ini menyimpan energi dalam bentuk medan magnet ketika dialiri arus listrik (`E = ½ × L × I²`).

Ketika transistor tiba-tiba diputus dari kondisi ON ke OFF dalam hitungan mikrodetik, arus listrik dipaksa berhenti seketika (`dt → 0`). Berdasarkan **Hukum Induksi Faraday dan Hukum Lenz**:
```text
V_induksi = -L × (di / dt)
```

Kumparan induktif akan mempertahankan aliran arusnya dengan cara membalik polaritas tegangannya dan memicu lonjakan tegangan balik raksasa (*Inductive Voltage Spike / Back-EMF*) yang dapat mencapai **50 Volt hingga 200 Volt**! Lonjakan tegangan tinggi ini akan langsung menembus batas isolasi transistor (*collector-emitter breakdown voltage*) dan membakar transistor dalam sekejap.

![Skematik Rangkaian Driver Transistor dan Dioda Flyback](images/transistor_driver_flyback_schematic.png)
*Sumber gambar: Diagram teknis orisinal laboratorium Sistem Tertanam.*

#### Peran Sakti Dioda Flyback (Freewheeling Diode):
Kita memasang sebuah dioda silikon (tipe **1N4007**) secara **antiparalel** (terbalik) melintasi terminal beban:
* **Saat Transistor ON:** Katoda dioda terhubung ke +5V dan Anoda terhubung ke Kolektor (tegangan rendah). Dioda berada dalam kondisi *reverse biased* (mati), sehingga tidak mengganggu operasi motor/relay sama sekali.
* **Saat Transistor Tiba-tiba OFF:** Lonjakan tegangan balik dari kumparan membuat potensial di ujung kolektor melesat lebih tinggi daripada +5V. Dioda seketika menjadi *forward biased* (aktif konduktif). Energi medan magnet yang tersimpan disirkulasikan kembali ke dalam kumparan dan terbuang menjadi panas secara aman, tanpa pernah menyentuh transistor ataupun merembet ke mikrokontroler!

> [!CAUTION]
> **PERINGATAN POLARITAS DIODA:**  
> Dioda memiliki gelang perak di salah satu ujung fisiknya yang menandakan terminal **KATODA (-)**. Ujung katoda bergelang perak ini **WAJIB terhubung ke kutub positif (+5V)**. Jika Anda memasangnya terbalik, saat transistor ON akan terjadi hubungan singkat (*short circuit*) langsung antara +5V ke Ground melalui dioda, yang dapat merusak adaptor daya Anda!

---

## 🧭 2. ATURAN EMAS PERIFERAL ANALOG ESP32: ADC1 VS ADC2

Chip ESP32 memiliki konverter analog-ke-digital bertipe SAR (*Successive Approximation Register*) 12-bit, yang mampu membagi tegangan analog rentang 0 s.d. 3.3V menjadi angka biner `0 s.d. 4095` (2¹² - 1). Namun, ada jebakan arsitektur besar yang wajib Anda ketahui:

![Peta Konflik Antara ADC1 dan ADC2 pada ESP32](images/adc1_vs_adc2_conflict_map.png)
*Sumber gambar: Diagram alokasi periferal internal laboratorium Sistem Tertanam.*

### Tabel Komparasi Mutlak: ADC1 vs ADC2

| Parameter | ADC1 (Jalur Aman Terisolasi) | ADC2 (Jalur Konflik Wi-Fi) |
|:---|:---|:---|
| **Pin Fisik GPIO** | **GPIO 32, 33, 34, 35, 36 (VP), 39 (VN)** | GPIO 0, 2, 4, 12, 13, 14, 15, 25, 26, 27 |
| **Bisa Dipakai Saat Wi-Fi Aktif?** | **BISA 100% (Sangat Stabil)** | **TIDAK BISA! (Error / Nilai Beku / Garbage)** |
| **Penyebab Konflik** | Memiliki sirkuit analog independen | Sirkuit ADC digunakan driver RF Wi-Fi untuk kalibrasi daya pancar antena |
| **Rekomendasi Praktikum** | **Gunakan GPIO 34** untuk potensiometer & sensor analog | Jangan pernah pakai untuk sensor jika proyek Anda terhubung ke internet |

### Karakteristik Non-Linearitas & Deadzone ADC ESP32
ADC pada ESP32 terkenal memiliki ketidaklinearan (*non-linearity*) pada kedua ujung ekstremnya:
1. **Deadzone Bawah (0.0V s.d. ~0.15V):** Nilai ADC akan terbaca 0 meskipun tegangan fisik sudah mulai naik di kisaran 0.1V.
2. **Saturasi Atas (~3.15V s.d. 3.3V):** Nilai ADC sudah mencapai batas maksimal 4095 meskipun tegangan belum genap menyentuh 3.3V.
3. **Peredam Derau (Multisampling Filter):** Sinyal analog di breadboard rentan menangkap derau frekuensi tinggi dari komputer atau jala-jala listrik. Untuk mendapatkan pembacaan yang tenang dan presisi, kita menerapkan teknik komputasi **Multisampling (Oversampling)**: membaca nilai pin sebanyak 16 kali secara berurutan dengan jeda singkat, lalu mengambil nilai rata-ratanya:
   ```text
   Nilai_ADC = (1 / N) × Σ analogRead(pin)
   ```

### Kalibrasi Pabrikan eFuse Vref: Rahasia Pembacaan MiliVolt Akurat
Pernahkah Anda mengukur pin 3.3V dengan voltmeter digital dan mendapatkan angka 3.28V, tetapi rumus matematis kasar ESP32 menghasilkan nilai yang meleset?  
Hal ini terjadi karena setiap chip silikon ESP32 memiliki variasi tegangan referensi internal bawaan pabrik (berkisar antara 1000 mV hingga 1200 mV). Untuk menjamin akurasi instrumentasi industri, pabrik Espressif mengukur karakteristik setiap chip sebelum dikemas dan membakar kurva kalibrasi presisi ke dalam memori permanen satu kali tulis bernama **eFuse** (*eFuse Two Point Calibration / Vref Calibration*).

Di lingkungan Arduino-ESP32, Anda dapat langsung memanfaatkan kalibrasi eFuse ini menggunakan fungsi bawaan:
```cpp
// Membaca tegangan fisik dalam satuan miliVolt langsung terkalibrasi pabrikan eFuse:
uint32_t voltage_mv = analogReadMilliVolts(POT_ADC_PIN);
float voltage_volt = voltage_mv / 1000.0;
```
Fungsi `analogReadMilliVolts()` secara otomatis mengompensasi kurva non-linearitas SAR ADC berdasarkan data kalibrasi chip masing-masing, sehingga hasil pengukuran jauh lebih presisi dibandingkan rumus bagi-kali biasa!

---

## ⚡ 3. HARDWARE PWM DENGAN TIMER LEDC

Mikrokontroler ESP32 tidak memiliki periferal PWM berbasis software primitif seperti board mikrokontroler zaman dulu yang membuat CPU tersendat. ESP32 dilengkapi modul periferal khusus bernama **LEDC (LED Controller)** yang beroperasi secara mandiri di tingkat hardware menggunakan clock 80 MHz!

![Visualisasi Bentuk Gelombang PWM LEDC](images/pwm_ledc_duty_cycle_visual.png)
*Sumber gambar: Visualisasi gelombang modulasi lebar pulsa laboratorium Sistem Tertanam.*

### Parameter Kunci LEDC:
1. **Frekuensi PWM (`f_pwm`):**  
   Berapa kali sinyal kotak berulang dalam satu detik. Untuk motor DC dan modulasi LED, frekuensi **5.000 Hz (5 kHz)** adalah standar emas: cukup tinggi agar terbebas dari dengungan frekuensi audio yang terdengar telinga manusia (20 Hz - 20 kHz), namun tidak terlalu tinggi sehingga meminimalkan rugi daya pensaklaran (*switching loss*) pada transistor.
2. **Resolusi Bit (`n`-bit):**  
   Menentukan kehalusan tingkatan kecepatan motor. Pada praktikum ini, kita memilih resolusi **8-bit**, yang menyediakan rentang nilai duty cycle dari **0 hingga 255** (2⁸ - 1).
3. **Rumus Tegangan Efektif Rata-Rata (`V_eff`):**  
   Tegangan keluaran efektif yang diterima oleh beban adalah hasil kali tegangan sumber dengan rasio duty cycle:
   ```text
   V_eff = V_sumber × (Nilai_Register_PWM / 255)
   ```
   * Register = 0 → Duty Cycle 0% → `V_eff = 0.0 V` (Motor Mati).
   * Register = 64 → Duty Cycle 25% → `V_eff ≈ 0.82 V` (Putaran Lambat).
   * Register = 128 → Duty Cycle 50% → `V_eff ≈ 1.65 V` (Putaran Sedang).
   * Register = 191 → Duty Cycle 75% → `V_eff ≈ 2.47 V` (Putaran Cepat).
   * Register = 255 → Duty Cycle 100% → `V_eff = 3.3 V` (Putaran Penuh Maksimum).

---

## 🔌 4. PANDUAN PERAKITAN BREADBOARD (WIRING DIAGRAM)

Sebelum mencolokkan kabel USB ke komputer, mari kita pelajari bentuk fisik komponen agar tidak terjadi kesalahan fatal saat merakit:

### A. Kenali Kaki Fisik Komponen Anda (Pinout Reference)
Kesalahan paling umum yang dialami mahasiswa di meja lab adalah salah menancapkan kaki komponen karena bingung membedakan Emitor, Basis, Kolektor pada transistor atau terbalik memasang polaritas dioda.

Gunakan panduan visual di bawah ini sebelum menancapkan komponen apa pun ke breadboard:

![Panduan Fisik Komponen: Identifikasi Kaki Pinout dan Polaritas](images/component_pinout_reference.png)
*Sumber gambar: Diagram referensi pinout fisik laboratorium Sistem Tertanam dengan foto makro komponen asli (Transistor 2N2222, Dioda 1N4007, Potensiometer 10 kΩ bersitasi Wikimedia Commons: CC BY-SA 4.0 / Public Domain).*

> [!TIP]
> **Aturan 3 Detik Cek Fisik:**
> 1. **Transistor 2N2222:** Posisikan sisi datar bertuliskan kode "2N2222" menghadap langsung ke wajah Anda dengan kaki menjuntai ke bawah. Urutan kakinya dari kiri ke kanan adalah **1: Emitor (E), 2: Basis (B), 3: Kolektor (C)**.
> 2. **Dioda 1N4007:** Cari gelang berwarna perak/abu-abu di bodi silinder hitam. Sisi bergelang perak adalah **Katoda (-)** yang wajib disambung ke rel +5V positif! Sisi bodi hitam polos adalah **Anoda (+)** yang disambung ke kaki Kolektor transistor.
> 3. **Potensiometer 10 kΩ:** Kaki kiri ke 3.3V, kaki tengah (wiper) ke GPIO 34, dan kaki kanan ke GND.

---

### B. Diagram Rangkaian Lengkap di Atas Breadboard

![Panduan Wiring Rangkaian Breadboard Minggu 3](images/breadboard_wiring_week03.png)
*Sumber gambar: Diagram tata letak pengkabelan laboratorium Sistem Tertanam dengan visualisasi foto makro fisik komponen asli bersitasi Wikimedia Commons (ESP32-WROOM-32 DevKit: CC BY-SA 4.0; Potensiometer 10 kΩ: Public Domain karya Chetvorno; Transistor 2N2222: CC BY-SA 3.0 karya F.A.; Dioda 1N4007: CC BY-SA 3.0 karya T.R.W.; Modul Relay 5V: CC BY-SA 4.0).*

### Tabel Pengkabelan Lengkap (Pin-by-Pin Wiring Matrix):

| Bagian | Kaki Komponen | Terhubung Ke Pin Board / Catu Daya | Catatan Pemasangan |
|:---|:---|:---|:---|
| **Potensiometer 10k** | Kaki Kiri (Pin 1) | **Pin 3.3V ESP32** | Catu daya referensi analog |
| | Kaki Tengah (Pin 2 - Wiper) | **GPIO 34 ESP32** | Sinyal analog input ADC1 murni |
| | Kaki Kanan (Pin 3) | **GND ESP32** | Ground referensi analog |
| **Driver Transistor 2N2222** | Basis (Kaki Tengah) | **Salah satu ujung Resistor 1 kΩ** | Jangan tancapkan langsung ke GPIO tanpa resistor! |
| | Ujung lain Resistor 1 kΩ | **GPIO 19 ESP32** | Output sinyal kendali PWM hardware |
| | Emitor (Kaki Kiri) | **GND ESP32 & GND Power Eksternal** | Jalur Ground Bersama (*Common Ground*) |
| | Kolektor (Kaki Kanan) | **Terminal Negatif Beban & Anoda Dioda** | Titik temu saklar pemutus arus |
| **Beban & Dioda Flyback** | Kutub Positif Motor/Relay (+) | **+5V Power Eksternal / Pin VIN ESP32** | Catu daya penggerak beban |
| | Katoda Dioda 1N4007 (Garis Perak) | **+5V Power Eksternal / Pin VIN ESP32** | **WAJIB:** Garis perak menghadap ke kutub positif! |
| | Anoda Dioda 1N4007 (Sisi Hitam) | **Kolektor Transistor 2N2222** | Sisi pembuang lonjakan induktif |
| **Catu Daya Eksternal** | Kutub Negatif Power Supply (GND) | **GND ESP32** | **ATURAN MUTLAK COMMON GROUND!** |

> [!IMPORTANT]
> **ATURAN MUTLAK: COMMON GROUND (TANAH BERSAMA)**  
> Jika Anda menggunakan power supply terpisah untuk menggerakkan motor (misalnya baterai 5V atau adaptor), **kutub Ground (GND) dari power supply tersebut WAJIB dihubungkan menjadi satu dengan pin GND ESP32**.  
> Tanpa jalur Common Ground, arus dari pin GPIO 19 tidak memiliki jalur pulang (*return path*) menuju mikrokontroler, sehingga transistor tidak akan pernah bisa menyala!

---

## 🛠️ 5. PANDUAN PRAKTIKUM LANGKAH-DEMI-LANGKAH (HANDS-ON)

Bagi Anda yang baru pertama kali menggunakan Visual Studio Code dan PlatformIO, ikuti langkah-langkah presisi berikut:

### Langkah 0: Memastikan Driver USB & Port COM Terdeteksi
Sebelum membuka kode, pastikan komputer Anda mengenali board ESP32:
1. Hubungkan kabel USB dari board ESP32 ke laptop/PC Anda.
2. Di Windows, buka **Device Manager** (tekan `Win + X` lalu pilih **Device Manager**).
3. Buka dropdown **Ports (COM & LPT)**. Pastikan muncul perangkat seperti **Silicon Labs CP210x USB to UART Bridge (COMx)** atau **USB-SERIAL CH340 (COMx)**.
4. Jika muncul tanda seru kuning atau perangkat tidak terdeteksi, silakan pasang driver USB sesuai panduan di [Modul Minggu 0: Onboarding](../week-00-onboarding/README.md).

### Langkah 1: Membuka Proyek di Visual Studio Code
1. Jalankan aplikasi **Visual Studio Code**.
2. Klik menu **File** → **Open Folder...** (atau tekan `Ctrl + K, Ctrl + O`).
3. Pilih folder: `labs/week-03-transistor-adc-pwm`.
4. Tunggu beberapa saat hingga bilah status (*Status Bar*) berwarna biru di bagian bawah VS Code memuat ekstensi PlatformIO.

### Langkah 2: Memeriksa File Konfigurasi `platformio.ini`
Pastikan file [platformio.ini](platformio.ini) telah dikonfigurasi dengan baud rate `115200` dan filter decoder:
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
monitor_filters = 
    esp32_exception_decoder
    time
build_type = debug
```

### Langkah 3: Menjalankan Kompilasi & Unggah Kode
Perhatikan deretan ikon kecil di **Bilah Status Bagian Bawah VS Code**:
1. **Kompilasi Program (Build):** Klik ikon centang **`✓`** (atau tekan `Ctrl + Alt + B`). Pastikan terminal menampilkan tulisan hijau `[SUCCESS]`.
2. **Unggah Program (Upload):** Klik ikon panah kanan **`→`** (atau tekan `Ctrl + Alt + U`). Tunggu hingga persentase penulisan flash mencapai 100% dan muncul tulisan `Leaving... Hard resetting via RTS pin...`.
3. **Buka Serial Monitor:** Klik ikon steker listrik colokan **`🔌`** di bilah status bawah (atau gunakan shortcut `Ctrl + Alt + S`).

> [!WARNING]
> **TIPS ANTI-ERROR PORT SIBUK:**  
> Jika saat proses Upload muncul error `could not open port 'COMx': PermissionError(13, 'Access is denied')`, itu artinya terminal Serial Monitor sedang terbuka mengunci port. Cukup klik ikon tong sampah / tempat sampah (*Kill Terminal*) pada jendela terminal Serial Monitor, lalu klik Upload **`→`** kembali!

---

### Langkah 4: Bedah Kode Sumber & Cara Interaksi Serial
Buka file [src/main.cpp](src/main.cpp). Program telah dilengkapi logika kendali terstruktur:
* **Filter Multisampling:** Membaca pin ADC1 (GPIO 34) sebanyak 16 sampel rata-rata untuk kestabilan sinyal.
* **Hardware PWM LEDC:** Menghasilkan sinyal frekuensi 5 kHz pada resolusi 8-bit (0-255) di GPIO 19.
* **Telemetri Non-Blocking:** Mengirimkan data telemetri berkala setiap 250 ms menggunakan fungsi `millis()`.

#### Cara Mengirim Perintah di Serial Monitor VS Code:
Di jendela terminal Serial Monitor PlatformIO, arahkan kursor Anda ke area terminal atau kotak input teks di bagian atas:
1. Ketik huruf **`m`** lalu tekan `Enter` → Beralih antara **Mode Otomatis (Potensiometer)** dan **Mode Manual (Serial)**.
2. Di Mode Manual, ketik angka **`0`** s.d. **`9`** lalu tekan `Enter` → Mengatur duty cycle PWM dari 0% hingga 90%.
3. Ketik huruf **`f`** lalu tekan `Enter` → Mengaktifkan kekuatan penuh 100% (*Full Power*).
4. Ketik huruf **`s`** lalu tekan `Enter` → Mencetak status telemetri terkini.
5. Ketik huruf **`h`** lalu tekan `Enter` → Menampilkan kembali menu bantuan perintah.

### Langkah 5: Membuka Visualisasi Grafik (Serial Plotter)
Untuk melihat bentuk grafik linieritas respon potensiometer secara langsung:
* Jika menggunakan **Arduino IDE**: Klik menu **Tools** → **Serial Plotter** (pastikan baud rate disetel ke `115200`).
* Jika menggunakan **VS Code**: Anda dapat memasang ekstensi gratis **Teleplot** atau menggunakan extension bawaan PlatformIO Device Monitor plotter. Putar potensiometer dan amati bagaimana grafik tegangan analog naik dan turun dengan mulus tanpa patahan tajam!

---

## 🎯 6. TANTANGAN PRAKTIKUM BERTINGKAT (ASSIGNMENTS)

Selesaikan 3 level tantangan berikut untuk membuktikan penguasaan materi Anda:

### 🟢 Level 1: Karakterisasi Respon ADC & Plotting Visual (Wajib Selesai di Kelas)
1. Buka Serial Plotter dan putar knob potensiometer perlahan dari posisi paling kiri (0V), ke tengah (1.65V), hingga mentok ke kanan (3.3V).
2. Catat di lembar kerja laporan Anda:
   * Pada nilai putaran knob berapa motor DC mulai mampu berputar mengatasi gesekan poros mekaniknya (*starting torque*)?
   * Apakah grafik kenaikan nilai ADC terhadap posisi putaran potensiometer terlihat mulus dan linier?

### 🟡 Level 2: Implementasi Histeresis & Deadband Filter (Nilai B)
* **Masalah Industri:** Saat potensiometer berada di dekat posisi nol, sedikit saja hembusan udara atau getaran meja dapat membuat nilai ADC melompat-lompat kecil (misal: 0, 15, 2, 8). Hal ini menyebabkan motor mendengung (*whining noise*) tanpa berputar karena arus tidak cukup kuat menggerakkan rotor, yang menyebabkan transistor cepat panas.
* **Tugas Anda:** Modifikasi fungsi pembacaan di `src/main.cpp` dengan menambahkan algoritma **Deadband (Ambang Batas Bawah)**:
  * Jika nilai ADC mentah `< 150` (sekitar ≈ 0.12 V), paksa nilai duty cycle menjadi **0** murni.
  * Terapkan ambang batas bawah sehingga motor baru mulai menyala jika nilai ADC melampaui 180, dan baru mati jika turun di bawah 150 (histeresis).
* **Contoh Panduan Logika Kode:**
  ```cpp
  const uint16_t DEADBAND_THRESHOLD = 150;
  uint8_t target_duty = 0;

  if (current_adc_raw > DEADBAND_THRESHOLD) {
      // Petakan dari batas ambang hingga nilai maksimum
      target_duty = map(current_adc_raw, DEADBAND_THRESHOLD, 4095, 0, 255);
  } else {
      target_duty = 0; // Matikan total untuk melindungi transistor & motor
  }
  write_pwm_duty(target_duty);
  ```

### 🔴 Level 3: Algoritma Soft-Start & Inrush Current Limiter (Nilai A)
* **Masalah Industri:** Ketika Anda tiba-tiba menyetel duty cycle dari 0% langsung ke 100% (misalnya dengan menekan tombol `f`), motor DC akan menarik arus lonjakan awal (*inrush current*) hingga 5 kali lipat dari arus normalnya! Hal ini sering kali memicu drop tegangan yang membuat ESP32 mengalami restart sendiri (*Brownout*).
* **Tugas Anda:** Buatlah fungsi **`soft_start_pwm(uint8_t target_duty, uint16_t ramp_duration_ms)`** yang menaikkan nilai duty cycle secara bertahap (interpolasi linear atau S-Curve) selama durasi waktu tertentu (misalnya dinaikkan setiap 10 milidetik), sehingga motor berakselerasi dengan halus tanpa lonjakan arus yang membebani rel catu daya.
* **Contoh Panduan Logika Kode:**
  ```cpp
  void soft_start_pwm(uint8_t target_duty, uint16_t step_delay_ms) {
      if (target_duty > current_duty) {
          // Akselerasi bertahap (Ramp-Up)
          for (int d = current_duty; d <= target_duty; d++) {
              write_pwm_duty(d);
              delay(step_delay_ms); // Interval kenaikan tiap tingkat PWM
          }
      } else {
          // Jika menurunkan kecepatan, dapat langsung diaplikasikan
          write_pwm_duty(target_duty);
      }
  }
  ```

---

## ❓ 7. TANYA JAWAB AWAM & PANDUAN TROUBLESHOOTING

| Masalah yang Sering Muncul | Penyebab Utama | Solusi Perbaikan Cepat |
|:---|:---|:---|
| **ESP32 tiba-tiba restart sendiri saat motor mulai berputar kencang.** | Terjadi penurunan tegangan sesaat (*voltage sag / brownout*) pada rel daya 5V/3.3V akibat motor menarik arus awal yang terlalu besar. | 1. Gunakan catu daya terpisah untuk motor.<br>2. Pasang kapasitor elektrolit (100 µF hingga 470 µF) melintasi rel daya motor sebagai penyimpan cadangan energi lokal.<br>3. Pastikan kabel USB Anda berkualitas baik. |
| **Error `PermissionError: Access is denied` saat upload program.** | Port Serial COM sedang terkunci karena Serial Monitor di VS Code masih aktif membuka koneksi. | Klik ikon **tempat sampah** (*Kill Terminal*) pada jendela terminal Serial Monitor VS Code, lalu klik tombol Upload **`→`** kembali. |
| **Relay berderik sangat cepat seperti suara tembakan senapan mesin.** | Anda menghubungkan modul relay ke pin sinyal PWM berfrekuensi tinggi (5 kHz). | Relay adalah saklar mekanik yang lambat (maksimal beralih beberapa kali per detik). **JANGAN beri sinyal PWM berkecepatan tinggi ke relay!** Berikan sinyal logika digital murni `HIGH` (menutup penuh) atau `LOW` (membuka penuh). |
| **Nilai ADC di Serial Monitor melompat-lompat acak padahal potensiometer diam.** | 1. Kaki wiper potensiometer longgar di breadboard.<br>2. Anda lupa menghubungkan kaki ketiga potensiometer ke GND.<br>3. Terjadi *floating input*. | Periksa kembali koneksi ketiga kaki potensiometer di breadboard. Pastikan kaki kiri terpasang kuat ke 3.3V, kaki kanan ke GND, dan kaki tengah ke GPIO 34. |
| **Dioda 1N4007 terasa sangat panas saat disentuh.** | Dioda dipasang terbalik! Katoda (garis perak) dipasang ke ground, bukan ke kutub positif. | **SEGERA CABUT KABEL USB!** Dioda terbalik akan menciptakan korsleting langsung antara +5V dan Ground saat transistor ON. Balikkan arah dioda sehingga garis perak menghadap ke +5V. |
| **Transistor tidak mau menyala meskipun pin GPIO sudah diset HIGH.** | Jalur Ground antara ESP32 dan catu daya motor belum disatukan (*Missing Common Ground*). | Sambungkan pin GND ESP32 ke pin GND sumber daya motor menggunakan seutas kabel jumper. |

---

## 🎤 8. PERTANYAAN UJI PEMAHAMAN LABORATORIUM (LIVE LAB ORAL MUTATION)

Saat sesi evaluasi praktikum, Dosen atau Asisten Laboratorium akan memberikan pertanyaan langsung untuk menguji orisinalitas dan pemahaman konseptual Anda:

1. *"Mengapa kita tidak boleh menghubungkan koil relay 5V langsung ke pin GPIO ESP32 meskipun tegangan koil relay bisa ditarik ke 3.3V?"*  
   **Kunci Jawaban:** Karena keterbatasan arus pin GPIO ESP32 (maksimal 12 mA). Koil relay membutuhkan arus minimal 70-100 mA untuk menarik kontak mekanisnya. Menghubungkannya langsung akan membakar gerbang silikon GPIO.
2. *"Apa yang akan terjadi pada transistor 2N2222 jika kita melepas dioda flyback 1N4007 saat mengendalikan motor DC?"*  
   **Kunci Jawaban:** Saat motor dimatikan tiba-tiba, energi medan magnet yang tersimpan pada kumparan motor akan memicu lonjakan tegangan balik raksasa (Back-EMF puluhan hingga ratusan Volt). Tanpa dioda flyback yang menyirkulasikan sisa energi tersebut, tegangan lonjakan akan menembus batas isolasi Vce transistor dan merusaknya secara permanen.
3. *"Di proyek tugas akhir nanti, Anda akan membuat sistem monitoring kualitas air berbasis IoT dengan sensor pH analog dan koneksi Wi-Fi ke server. Mengapa Anda dilarang keras menancapkan kabel sensor analog ke pin GPIO 2 atau GPIO 15?"*  
   **Kunci Jawaban:** Karena GPIO 2 dan 15 berada di bawah sirkuit internal ADC2. Saat modul radio Wi-Fi aktif mengirim data, hardware ADC2 akan diperebutkan dan disita oleh driver RF untuk kalibrasi daya pancar antena, sehingga pembacaan sensor pH akan macet atau bernilai acak. Sensor analog wajib dipasang ke ADC1 (misal GPIO 34).
4. *"Berapa tegangan efektif yang dirasakan oleh motor jika register PWM disetel pada nilai 128 dengan catu daya 5V?"*  
   **Kunci Jawaban:** `V_eff = 5V × (128 / 255) ≈ 2.5 Volt` (atau 50% dari tegangan penuh).

---

## 📤 9. PANDUAN COMMIT & PUSH KE GITHUB

Setelah seluruh pengujian di breadboard berhasil dan tantangan kode selesai diimplementasikan, simpan seluruh pekerjaan Anda ke repositori GitHub pribadi/tim:

1. Buka terminal terintegrasi di VS Code (**Terminal** → **New Terminal**).
2. Periksa status berkas yang telah Anda modifikasi:
   ```bash
   git status
   ```
3. Tambahkan berkas yang telah diubah ke area staging:
   ```bash
   git add labs/week-03-transistor-adc-pwm/
   ```
4. Lakukan commit dengan pesan terstruktur yang informatif:
   ```bash
   git commit -m "feat(week-03): implement transistor driver, ADC1 precision, and LEDC PWM"
   ```
5. Unggah perubahan Anda ke GitHub:
   ```bash
   git push origin main
   ```
6. Buka halaman GitHub repositori Anda di browser untuk memastikan seluruh kode dan laporan praktikum telah terunggah dengan rapi!

---

*Selamat bereksperimen, jaga keselamatan komponen Anda, dan salam mahasiswa Teknik Elektro! ⚡🔬*
