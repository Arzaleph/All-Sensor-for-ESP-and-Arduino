#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// === Konstanta dan Pin ===
#define DHTPIN 2
#define DHTTYPE DHT11
#define MQ2PIN A0
// #define FLAME_PIN 3
#define LED_HIJAU 4
#define LED_KUNING 5
#define LED_MERAH 8
#define BUZZERPIN 9

// === Inisialisasi Sensor dan LCD ===
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === Batas Sensor ===
const float batasSuhu = 50.0;
const int batasGasBahaya = 400;
const int batasGasWaspada = 250;

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  pinMode(MQ2PIN, INPUT);
  // pinMode(FLAME_PIN, INPUT);
  pinMode(LED_HIJAU, OUTPUT);
  pinMode(LED_KUNING, OUTPUT);
  pinMode(LED_MERAH, OUTPUT);
  pinMode(BUZZERPIN, OUTPUT);

  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_KUNING, LOW);
  digitalWrite(LED_MERAH, LOW);
  digitalWrite(BUZZERPIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistem Kebakaran");
  delay(2000);
}

void loop() {
  float suhu = dht.readTemperature();
  int gas = analogRead(MQ2PIN);
  // int flame = digitalRead(FLAME_PIN); // 0 = api terdeteksi

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.print(" °C | Gas: ");
  Serial.print(gas);
  // Serial.print(" | Flame: ");
  // Serial.println(flame == 0 ? "TERDETEKSI" : "TIDAK");

  // Reset indikator
  digitalWrite(LED_HIJAU, LOW);
  digitalWrite(LED_KUNING, LOW);
  digitalWrite(LED_MERAH, LOW);
  digitalWrite(BUZZERPIN, LOW);

  String status = "AMAN";

  if (suhu >= batasSuhu || gas >= batasGasBahaya ) {
    digitalWrite(LED_MERAH, HIGH);
    tone(BUZZERPIN, 500);
    status = "BAHAYA!";
  } else if (gas >= batasGasWaspada || suhu >= 35) {
    digitalWrite(LED_KUNING, HIGH);
    status = "WASPADA";
    noTone(BUZZERPIN);
  } else {
    digitalWrite(LED_HIJAU, HIGH);
    noTone(BUZZERPIN);
  }

  // Tampilkan ke LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S:");
  lcd.print(suhu, 1);
  lcd.print((char)223); // simbol derajat
  lcd.print("C G:");
  lcd.print(gas);

  lcd.setCursor(0, 1);
  lcd.print("Status: ");
  lcd.print(status);
  
  delay(2000);
}
