// non blynk

#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>

#define DHTPIN 2
#define DHTTYPE DHT22
#define LDR_PIN A0
#define TRIG_PIN 4
#define ECHO_PIN 5
#define LED_LAMP 6
#define KIPAS_PIN 7

#define LDR_THRESHOLD 600     
#define JARAK_THRESHOLD 50    
#define TEMP_THRESHOLD 30     

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_LAMP, OUTPUT);
  pinMode(KIPAS_PIN, OUTPUT);
}

float bacaJarak() {
  digitalWrite(TRIG_PIN, LOW);
  digitalWrite(TRIG_PIN, HIGH);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();
  float jarak = bacaJarak();

  Serial.println("=== SMART HOME STATUS ===");
  Serial.print("LDR: "); Serial.println(ldrValue);
  Serial.print("Suhu: "); Serial.print(suhu); Serial.println(" °C");
  Serial.print("Kelembapan: "); Serial.print(kelembapan); Serial.println(" %");
  Serial.print("Jarak (cm): "); Serial.println(jarak);
  Serial.println("=========================");

  if (ldrValue > LDR_THRESHOLD && jarak < JARAK_THRESHOLD) {
    digitalWrite(LED_LAMP, HIGH);
    Serial.println("💡 Lampu menyala (karena gelap & ada orang).");
  } else {
    digitalWrite(LED_LAMP, LOW);
  }

  if (suhu > TEMP_THRESHOLD) {
    digitalWrite(KIPAS_PIN, HIGH);
    tone(KIPAS_PIN, 1000);
    Serial.println("🌬️ Kipas AKTIF karena suhu tinggi.");
  } else {
    digitalWrite(KIPAS_PIN, LOW);
    noTone(KIPAS_PIN);
  }

  delay(2000);
}

// blynk esp32

#include <WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>

// ===== GANTI DENGAN KREDENSIAL KAMU =====
#define BLYNK_TEMPLATE_ID "TMPL6DsWd0Qmt"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN "Alox1gUHrXWqYU8Nfnf0ptWG4v46yhnh"

char ssid[] = "Nuju Coffee Kedaton";
char pass[] = "zozogakmahallagi";

// ===== KONFIGURASI PIN =====
#define DHTPIN 4
#define DHTTYPE DHT22
const int pirPin = 2;
const int ledGerak = 13;
const int ledSuhuTinggi = 12;

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

BlynkTimer timer;

void kirimDataKeBlynk() {
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();
  int motion = digitalRead(pirPin);

  // Kirim data ke Blynk
  Blynk.virtualWrite(V0, suhu);
  Blynk.virtualWrite(V1, kelembapan);
  Blynk.virtualWrite(V2, motion == HIGH ? "Terdeteksi" : "Tidak");

  // LED fisik
  digitalWrite(ledGerak, motion == HIGH ? HIGH : LOW);
  digitalWrite(ledSuhuTinggi, suhu > 35 ? HIGH : LOW);

  // LCD
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(suhu);
  lcd.print((char)223);
  lcd.print("C H:");
  lcd.print(kelembapan);
  lcd.print("% ");

  lcd.setCursor(0, 1);
  lcd.print("Gerak: ");
  lcd.print(motion == HIGH ? "TERDETEKSI " : "TIDAK     ");

  // Serial
  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.print(" C, Kelembapan: ");
  Serial.print(kelembapan);
  Serial.print(" %, Gerakan: ");
  Serial.print(motion == HIGH ? "Terdeteksi" : "Tidak");
  Serial.print(", Suhu Tinggi: ");
  Serial.println(suhu > 35 ? "YA" : "TIDAK");
}

void setup() {
  Serial.begin(115200);

  pinMode(pirPin, INPUT);
  pinMode(ledGerak, OUTPUT);
  pinMode(ledSuhuTinggi, OUTPUT);

  dht.begin();
  lcd.begin(16, 2);
  lcd.backlight();

  WiFi.begin(ssid, pass);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(1000L, kirimDataKeBlynk);
}

void loop() {
  Blynk.run();
  timer.run();
}
