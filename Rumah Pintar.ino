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
