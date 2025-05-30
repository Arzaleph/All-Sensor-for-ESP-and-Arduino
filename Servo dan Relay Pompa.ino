//Servo

#include <Servo.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 5
const int pinServo = 3;
const int pinSoil = A0;
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define buzzerPin 4

void setup() {
  lcd.backlight();
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  myservo.attach(pinServo);
  myservo.write(0);
}

void loop() {
  float pembacaan = analogRead(pinSoil);
  float kelembapanTanah = (100 - ((pembacaan/1023.00)*100));
  float kelembapanUdara = dht.readHumidity();
  Serial.print("Kelembapan tanah: ");
  Serial.println(kelembapanTanah);
  Serial.print("Kelembapan udara: ");
  Serial.println(kelembapanUdara);

  if (kelembapanTanah < 10){
    myservo.write(90);
    Serial.println("Kran terbuka");
  } else {
    myservo.write(0);
    Serial.println("Kran tertutup");
  }

  delay(2000);
  lcd.setCursor(0,0);

  if (kelembapanUdara > 70){
    lcd.print("Kel. Udara:");
    lcd.print(kelembapanUdara);
    lcd.print((char)223);
    lcd.setCursor(0, 1);
    lcd.print("Bukalah Jendela!");
    tone(buzzerPin, 800);
  } else {
    lcd.print("Kel. Udara:");
    lcd.print(kelembapanUdara);
    lcd.print((char)223);
    lcd.setCursor(0, 1);
    lcd.print("Tutuplah Jendela!");
    noTone(buzzerPin);
  }
}








// Relay Pompa BLYNK


#define BLYNK_TEMPLATE_ID "TMPL6qC47Qp7F"
#define BLYNK_TEMPLATE_NAME "ESP8266 WIFI"
#define BLYNK_AUTH_TOKEN "kxWkFPUYNlnsXSXoHgNVW3-whnpgQ5Vk"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// WiFi credentials
char ssid[] = "why?";
char pass[] = "nctdream";

// DHT setup
#define DHTPIN D2         // Pin untuk sensor DHT11 (misalnya D2 atau GPIO4 di ESP8266)
#define DHTTYPE DHT11
#define LED_TEMP D4       // Pin untuk LED indikator suhu (misalnya D4 atau GPIO2)
DHT dht(DHTPIN, DHTTYPE);

// Soil Moisture Sensor setup
#define SOIL_SENSOR_PIN A0  // Pin analog untuk sensor kelembapan tanah
const int AIR_VALUE = 790;  // Nilai kalibrasi sensor di udara kering (sesuaikan!)
const int WATER_VALUE = 390;// Nilai kalibrasi sensor di air (sesuaikan!)

// Blynk Virtual Pins
#define V_TEMPERATURE V0    // Pin virtual untuk Suhu
#define V_HUMIDITY V1       // Pin virtual untuk Kelembapan Udara
#define V_SOIL_MOISTURE V2  // Pin virtual untuk Kelembapan Tanah

BlynkTimer timer;

void checkDHT() {
  float suhu = dht.readTemperature();
  float hum = dht.readHumidity();

  if (!isnan(suhu) && !isnan(hum)) {
    Blynk.virtualWrite(V_TEMPERATURE, suhu);
    Blynk.virtualWrite(V_HUMIDITY, hum);
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" *C, Kelembaban Udara: ");
    Serial.print(hum);
    Serial.println(" %");

    // Logika untuk LED indikator suhu
    if (suhu > 32) { // Misalnya, nyalakan LED jika suhu di atas 32°C
      digitalWrite(LED_TEMP, HIGH);
    } else {
      digitalWrite(LED_TEMP, LOW);
    }
  } else {
    Serial.println("Gagal membaca data dari DHT");
  }
}

void checkSoilMoisture() {
  int soilRawValue = analogRead(SOIL_SENSOR_PIN);
  // Pastikan AIR_VALUE lebih besar dari WATER_VALUE jika sensor membaca lebih tinggi saat kering,
  // atau sebaliknya jika sensor membaca lebih rendah saat kering.
  // Untuk sensor umum, nilai lebih tinggi berarti lebih kering.
  int soilMoisturePercent = map(soilRawValue, AIR_VALUE, WATER_VALUE, 0, 100);
  
  // Batasi persentase antara 0 dan 100
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);

  Blynk.virtualWrite(V_SOIL_MOISTURE, soilMoisturePercent);

  Serial.print("Kelembaban Tanah (Raw): ");
  Serial.print(soilRawValue);
  Serial.print(", Kelembaban Tanah (%): ");
  Serial.print(soilMoisturePercent);
  Serial.println(" %");
}

void setup() {
  Serial.begin(9600); // Atau 115200 untuk debug yang lebih cepat jika diperlukan
  pinMode(LED_TEMP, OUTPUT);
  digitalWrite(LED_TEMP, LOW); // Pastikan LED mati saat awal

  dht.begin();
  
  Serial.println("Connecting to WiFi and Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  // Cek koneksi Blynk
  int connectTimeout = 0;
  while (Blynk.connected() == false && connectTimeout < 30) { // Coba selama 15 detik
    delay(500);
    Serial.print(".");
    connectTimeout++;
  }

  if (Blynk.connected()) {
    Serial.println("\nConnected to Blynk!");
  } else {
    Serial.println("\nFailed to connect to Blynk. Check token, WiFi, or internet.");
    // Anda bisa menambahkan loop di sini atau logika lain jika gagal konek
  }

  // Atur interval timer untuk fungsi-fungsi
  timer.setInterval(2000L, checkDHT);            // Tiap 2 detik cek suhu dan kelembaban udara
  timer.setInterval(2500L, checkSoilMoisture);  // Tiap 2.5 detik cek kelembaban tanah
}

void loop() {
  if (Blynk.connected()) {
    Blynk.run();
  } else {
    // Coba sambungkan kembali jika koneksi terputus
    Serial.println("Blynk disconnected. Attempting to reconnect...");
    Blynk.connect(); // Mencoba menyambung kembali ke Blynk
    delay(5000); // Tunggu sebelum mencoba lagi
  }
  timer.run();
}












// arduino untuk ldr dan soilmoisture

// Pin untuk sensor kelembaban tanah (Analog)
const int soilMoisturePin = A0;
// Pin untuk LDR (Analog)
const int ldrPin = A1;
// Pin untuk relay (Digital)
const int relayPin = 7;

// Nilai ambang batas kelembaban tanah (sesuaikan dengan sensor Anda)
// Semakin rendah nilainya, semakin kering tanahnya
const int thresholdLembab = 400; // Contoh nilai, perlu dikalibrasi

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Matikan relay saat awal (HIGH untuk relay modul aktif LOW)
                                 // atau LOW jika relay modul Anda aktif HIGH
  Serial.println("Sistem Monitoring dan Otomatisasi Pengairan Siap");
}

void loop() {
  // Baca nilai sensor kelembaban tanah
  int nilaiKelembaban = analogRead(soilMoisturePin);
  Serial.print("Kelembaban Tanah: ");
  Serial.print(nilaiKelembaban);

  // Baca nilai LDR
  int nilaiCahaya = analogRead(ldrPin);
  Serial.print(", Intensitas Cahaya: ");
  Serial.println(nilaiCahaya);

  // Logika untuk mengontrol pompa air
  if (nilaiKelembaban > thresholdLembab) { // Jika tanah kering (nilai analog sensor tinggi saat kering)
    digitalWrite(relayPin, LOW); // Nyalakan pompa air (LOW untuk relay modul aktif LOW)
    Serial.println("Tanah KERING. Pompa Menyala.");
  } else {
    digitalWrite(relayPin, HIGH); // Matikan pompa air (HIGH untuk relay modul aktif LOW)
    Serial.println("Tanah LEMBAB. Pompa Mati.");
  }

  delay(2000); // Tunggu 2 detik sebelum pembacaan berikutnya
}







// ldr, sm, untuk esp8266 ke blynk
#define BLYNK_TEMPLATE_ID "TMPL6qC47Qp7F"
#define BLYNK_TEMPLATE_NAME "ESP8266 WIFI"
#define BLYNK_AUTH_TOKEN "kxWkFPUYNlnsXSXoHgNVW3-whnpgQ5Vk"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h> // Library untuk sensor DHT

// Ganti dengan kredensial WiFi Anda
char ssid[] = "NAMA_WIFI_ANDA";
char pass[] = "PASSWORD_WIFI_ANDA";

// Ganti dengan Auth Token Blynk Anda
char auth[] = "AUTH_TOKEN_BLYNK_ANDA";

// Pin untuk sensor kelembaban tanah (Analog)
const int soilMoisturePin = A0;

// Pin untuk sensor DHT22 (Digital)
#define DHTPIN D2 // Contoh menggunakan pin D2 (GPIO4). Anda bisa ganti sesuai kebutuhan.
// Tipe sensor DHT (DHT11, DHT21, DHT22)
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Pin untuk relay (Digital)
const int relayPin = D1; // Contoh menggunakan pin D1 (GPIO5) di NodeMCU/Wemos

// Nilai ambang batas kelembaban tanah (sesuaikan dengan sensor Anda)
const int thresholdLembab = 500;

BlynkTimer timer;

// Fungsi untuk mengirim data sensor ke Blynk
void sendSensorData() {
  // Baca nilai sensor kelembaban tanah
  int soilMoistureValue = analogRead(soilMoisturePin);
  Serial.print("Kelembaban Tanah: ");
  Serial.print(soilMoistureValue);

  // Baca suhu dan kelembaban dari DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Baca dalam Celsius (default)

  // Cek apakah pembacaan berhasil (NaN artinya gagal)
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(" Gagal membaca dari sensor DHT!");
  } else {
    Serial.print(", Suhu: ");
    Serial.print(temperature);
    Serial.print(" *C");
    Serial.print(", Kelembaban Udara: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Kirim data ke Blynk
    Blynk.virtualWrite(V1, temperature);    // Kirim data suhu ke V1
    Blynk.virtualWrite(V2, humidity);       // Kirim data kelembaban udara ke V2
  }

  Blynk.virtualWrite(V0, soilMoistureValue); // Kirim data kelembaban tanah ke V0

  // Logika untuk mengontrol pompa air
  if (soilMoistureValue > thresholdLembab) { // Jika tanah kering (nilai ADC tinggi saat kering untuk beberapa sensor)
    digitalWrite(relayPin, LOW); // Nyalakan pompa air (asumsi relay aktif LOW)
    Serial.println("Status Pompa: Tanah KERING. Pompa Menyala.");
    Blynk.setProperty(V0, "color", "#D3435C"); // Ubah warna gauge kelembaban tanah di Blynk menjadi merah
  } else {
    digitalWrite(relayPin, HIGH); // Matikan pompa air (asumsi relay aktif HIGH untuk mati)
    Serial.println("Status Pompa: Tanah LEMBAB. Pompa Mati.");
    Blynk.setProperty(V0, "color", "#23C48E"); // Ubah warna gauge kelembaban tanah di Blynk menjadi hijau
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Matikan relay saat awal (HIGH untuk relay modul aktif LOW)

  Serial.println("Memulai Sensor DHT...");
  dht.begin(); // Mulai sensor DHT

  Serial.println("Menghubungkan ke WiFi...");
  Blynk.begin(auth, ssid, pass);
  Serial.println("Terhubung ke Blynk!");

  // Atur timer untuk memanggil sendSensorData() setiap 2 detik (atau sesuai kebutuhan)
  timer.setInterval(2000L, sendSensorData); // 2000L untuk 2 detik. Untuk DHT, pembacaan jangan terlalu cepat.
}

void loop() {
  Blynk.run();
  timer.run(); // Jalankan timer Blynk
}

// Opsional: Jika Anda ingin kontrol manual dari Blynk untuk relay
// BLYNK_WRITE(V3) { // Misalkan V3 adalah tombol untuk relay
//   int pinValue = param.asInt(); // Dapatkan nilai dari tombol (0 atau 1)
//   if (pinValue == 1) {
//     digitalWrite(relayPin, LOW); // Nyalakan pompa
//     Serial.println("Relay ON dari Blynk");
//   } else {
//     digitalWrite(relayPin, HIGH); // Matikan pompa
//     Serial.println("Relay OFF dari Blynk");
//   }
// }
