# 📌 LEMBAR SAKU PIN ESP32 (PINOUT & HARDWARE GOTCHAS)
### Panduan Cepat Mahasiswa S1 Teknik Elektro

> [!WARNING]
> **SIMPAN PANDUAN INI DI HP/LAPTOP ANDA!**  
> 80% kerusakan mikrokontroler atau kegagalan program pada semester awal disebabkan oleh salah memilih pin atau mengabaikan karakteristik elektrikal ESP32.

---

## 1. ATURAN EMAS PIN ESP32 (GOLDEN RULES)

```
        ┌─────────────────────────────────────────────────────────────┐
        │                 ESP32-WROOM-32 (38 PINS)                    │
        │                                                             │
        │   [TABU!]   GPIO 6 - 11  ==> Flash SPI (JANGAN DISENTUH)   │
        │   [INPUT!]  GPIO 34 - 39 ==> Input Saja, Tanpa Pull-up      │
        │   [WI-FI!]  ADC2 Pins    ==> MATI TOTAL saat Wi-Fi Aktif    │
        │   [AMAN!]   ADC1 Pins    ==> GPIO 32 - 39 (Sensor Analog)   │
        └─────────────────────────────────────────────────────────────┘
```

---

## 2. TABEL DIAGNOSTIK PIN SECARA LENGKAP

| Nomor GPIO | Fungsi Utama | Kategori | Catatan Penting & Pantangan |
| :---: | :---: | :---: | :--- |
| **GPIO 0** | Strapping Pin | ⚠️ Perlu Hati-Hati | Terhubung ke tombol BOOT. Harus ditarik HIGH saat booting normal. |
| **GPIO 1** | UART0 TXD | ⚠️ Khusus Serial | Port upload firmware & serial debug. Jangan gunakan untuk sensor/LED. |
| **GPIO 2** | Strapping / On-board LED | ⚠️ Perlu Hati-Hati | Terhubung ke LED biru internal pada banyak board. Harus LOW saat flashing. |
| **GPIO 3** | UART0 RXD | ⚠️ Khusus Serial | Port upload firmware & serial debug. Jangan gunakan untuk sensor/LED. |
| **GPIO 4** | ADC2_CH0 / Touch 0 | ⚠️ Konflik Wi-Fi | Jangan gunakan untuk sensor analog jika Wi-Fi aktif. |
| **GPIO 5** | VSPI CS / Strapping | 🟢 Aman Digunakan | Default CS untuk bus SPI utama. Mengeluarkan sinyal PWM saat boot. |
| **GPIO 6 - 11** | Integrated SPI Flash | ⛔ **TABU / DILARANG** | **JANGAN PERNAH DIHUBUNGKAN KE KABEL APAPUN!** Chip akan langsung crash/bootloop. |
| **GPIO 12** | MTDI / Strapping | ⚠️ Perlu Hati-Hati | Menentukan tegangan Flash (1.8V vs 3.3V). Jika ditarik HIGH saat boot, ESP32 gagal start. |
| **GPIO 13** | ADC2_CH4 / HSPI ID | ⚠️ Konflik Wi-Fi | Jangan gunakan untuk sensor analog jika Wi-Fi aktif. |
| **GPIO 14** | ADC2_CH6 / HSPI CLK | ⚠️ Konflik Wi-Fi | Jangan gunakan untuk sensor analog jika Wi-Fi aktif. |
| **GPIO 15** | Strapping / HSPI CMD | ⚠️ Perlu Hati-Hati | Output sinyal debug saat boot. Harus ditarik ke level yang tepat. |
| **GPIO 16 - 17**| UART2 (TX2 / RX2) | 🟢 **Sangat Aman** | Pin ideal untuk komunikasi serial eksternal (Modbus RS-485 / GPS). |
| **GPIO 18** | VSPI SCK | 🟢 **Sangat Aman** | Pin Clock default bus SPI (Display TFT/OLED). |
| **GPIO 19** | VSPI MISO | 🟢 **Sangat Aman** | Jalur input data bus SPI (SD Card). |
| **GPIO 21** | I2C SDA | 🟢 **Sangat Aman** | Jalur Data default bus I2C (Sensor BME280 / MPU6050). |
| **GPIO 22** | I2C SCL | 🟢 **Sangat Aman** | Jalur Clock default bus I2C (Sensor BME280 / MPU6050). |
| **GPIO 23** | VSPI MOSI | 🟢 **Sangat Aman** | Jalur output data bus SPI (Display / SD Card). |
| **GPIO 25** | DAC1 / ADC2_CH8 | 🟢 Aman (DAC) | Digital-to-Analog Converter 8-bit (Pembangkit sinyal analog riil). |
| **GPIO 26** | DAC2 / ADC2_CH9 | 🟢 Aman (DAC) | Digital-to-Analog Converter 8-bit kedua. |
| **GPIO 27** | ADC2_CH7 / Touch 7 | ⚠️ Konflik Wi-Fi | Jangan gunakan untuk sensor analog jika Wi-Fi aktif. |
| **GPIO 32** | **ADC1_CH4** / Touch 9 | 🟢 **Pin Sensor Terbaik** | Aman untuk sensor analog, tidak terpengaruh Wi-Fi. |
| **GPIO 33** | **ADC1_CH5** / Touch 8 | 🟢 **Pin Sensor Terbaik** | Aman untuk sensor analog, tidak terpengaruh Wi-Fi. |
| **GPIO 34** | **ADC1_CH6** | 🟡 **Input Saja** | Input analog/digital. **TIDAK BISA OUTPUT & TIDAK ADA PULL-UP.** |
| **GPIO 35** | **ADC1_CH7** | 🟡 **Input Saja** | Input analog/digital. **TIDAK BISA OUTPUT & TIDAK ADA PULL-UP.** |
| **GPIO 36** | **ADC1_CH0 (SENSOR_VP)**| 🟡 **Input Saja** | Input analog/digital. **TIDAK BISA OUTPUT & TIDAK ADA PULL-UP.** |
| **GPIO 39** | **ADC1_CH3 (SENSOR_VN)**| 🟡 **Input Saja** | Input analog/digital. **TIDAK BISA OUTPUT & TIDAK ADA PULL-UP.** |

---

## 3. MENGAPA ADC2 TIDAK BISA DIGUNAKAN BERSAMA WI-FI?

ESP32 memiliki dua modul Analog-to-Digital Converter:
* **ADC1 (8 Channel: GPIO 32 - 39):** Memiliki sirkuit mandiri.
* **ADC2 (10 Channel: GPIO 0, 2, 4, 12-15, 25-27):** Sirkuitnya digunakan bersama dengan modul radio frekuensi Wi-Fi.

> [!CAUTION]
> Begitu perintah `WiFi.begin()` atau driver Wi-Fi ESP-IDF aktif, pemanggilan pembacaan analog pada pin ADC2 akan menghasilkan nilai `ESP_ERR_TIMEOUT` atau pembacaan acak.  
> **Kaidah Tetap:** Seluruh sensor analog (suhu, arus, potensiometer, sensor tegangan) **WAJIB** dipasang pada **GPIO 32, 33, 34, 35, 36, atau 39**.

---

## 4. BATASAN ELEKTRIKAL FISIK (AGAR BOARD TIDAK RUSAK)

1. **Level Tegangan Logika:**
   * Logika HIGH ESP32 adalah **3.3 Volt**.
   * Jangan pernah menghubungkan output sensor 5V (misal: sensor jarak ultrasonik HC-SR04 model lama atau sensor jarak optik) langsung ke pin GPIO ESP32. Gunakan **Logic Level Shifter** atau pembagi tegangan resistor (*voltage divider*).
2. **Arus Maksimum per Pin:**
   * Arus output aman per pin GPIO adalah **12 mA** (maksimum absolut 40 mA).
   * Menyalakan buzzer aktif, motor DC kecil, atau koil relay secara langsung akan membakar jalur tembaga mikrokontroler.

---

## 5. RANGKAIAN PROTEKSI BEBAN DAYA (STANDAR ELEKTRO)

Untuk menyalakan beban induktif (Relay 5V / Motor DC / Solenoid), gunakan sirkuit saklar transistor berikut:

```
            +5V (Catu Daya Eksternal)
             │
             ├───[ Beban: Koil Relay / Motor ]
             │       │
             │       ├───[|<─ Dioda 1N4007 ] (Dioda Proteksi Flyback)
             │       │    (Katoda ke +5V, Anoda ke Kolektor)
             │       │
             │    ┌──┴──┐
             │    │  C  │
  GPIO ESP32 ─────┤ B   │ Transistor NPN (2N2222 / BC547)
  (via 1kΩ)       │  E  │
                  └──┬──┘
                     │
                    GND (Common Ground dengan ESP32)
```

> [!IMPORTANT]
> **Fungsi Dioda 1N4007:** Saat transistor dimatikan, medan magnet pada koil relay runtuh dan menghasilkan lonjakan tegangan balik ratusan volt (*Flyback Back-EMF*). Dioda ini meredam lonjakan tegangan tersebut agar transistor dan ESP32 tidak terbakar!
