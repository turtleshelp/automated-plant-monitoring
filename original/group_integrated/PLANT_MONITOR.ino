const int ledPin13 = 13;
const int ledPin12 = 12;
const int ledPin11 = 11;
const int ledPin10 = 10;
const int ledPin9 = 9;
const int ledPin8 = 8;
const int ledPin7 = 7;
const int ledPin6 = 6;
const int ledPin5 = 5;
const int ledPin4 = 4;
const int ledPin3 = 3;
const int ledPin2 = 2;
const int ledPin1 = 1;
const int ledPin0 = 0;
const int ldrPin = A0;
int soilsensor_Pin = A3;
int soilstatus;
int waterPin = A1;
int waterstatus;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#include <DHT.h>
#define DHTPin 2
#define DHTTYPE DHT11
DHT dht(DHTPin, DHTTYPE);

void setup() {

  Serial.begin(9600);
  dht.begin();
  pinMode(ledPin13, OUTPUT);
  pinMode(ledPin12, OUTPUT);
  pinMode(ledPin11, OUTPUT);
  pinMode(ledPin10, OUTPUT);
  pinMode(ledPin9, OUTPUT);
  pinMode(ledPin8, OUTPUT);
  pinMode(ledPin7, OUTPUT);
  pinMode(ledPin6, OUTPUT);
  pinMode(ledPin5, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(soilstatus, OUTPUT);

  lcd.begin();
  lcd.print("Weed Monitor");
  lcd.setCursor (0, 1);
  lcd.print("MSES <3");
  lcd.setCursor (0, 2);
  lcd.print("By");
  lcd.setCursor (0, 3);
  lcd.print("Ali + team"); // [2026] teammates' names removed for publication
  delay(2000);
}

void loop() {
  float h = dht.readHumidity(); //FOR TEMP
  float t = dht.readTemperature(); //FOR HUMID
  int ldrStatus = analogRead(ldrPin); //FOR LDR
  soilstatus = analogRead(soilsensor_Pin); //FOR MOISTURE
  soilstatus = map(soilstatus, 550, 10, 0, 100);

  int SensorValue = analogRead(soilsensor_Pin);
  float SensorVolts = analogRead(soilsensor_Pin) * 0.0048828125;
  int soilstatus = (SensorVolts / 5) * 100;

  int waterstatus = (SensorVolts / 5) * 100;


  //FOR LCD//
  lcd.begin();
  lcd.backlight();


  //FOR LDR//
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("LIGHT INT: ");
  lcd.print(ldrStatus);
  Serial.print(ldrStatus);
  if (ldrStatus > 200) {

    digitalWrite(ledPin13, HIGH);
    digitalWrite(ledPin12, LOW);
    lcd.setCursor (0, 1);
    lcd.print("ITS DARK");
    lcd.setCursor (0, 2);
    lcd.print("LIGHT ON");
    delay (2000);

  } else if (ldrStatus < 150) {

    digitalWrite(ledPin13, LOW);
    digitalWrite(ledPin12, HIGH);
    lcd.setCursor (0, 1);
    lcd.print("ITS BRIGHT");
    lcd.setCursor (0, 2);
    lcd.print("LIGHT OFF");
    delay (2000);
  }
  else if (150 > ldrStatus < 200) {

    digitalWrite(ledPin13, LOW);
    digitalWrite(ledPin12, LOW);
    lcd.setCursor (0, 1);
    lcd.print("ITS OPTIMAL");
    lcd.setCursor (0, 2);
    lcd.print("NO ACTION");
    delay (2000);
  }

  //FOR TEMP//
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("TEMPERATURE: ");
  lcd.print(t);
  lcd.print((char)223);
  lcd.print("C");
  Serial.print(", ");
  Serial.print(t);
  if (t > 29)
  {
    digitalWrite(ledPin11, HIGH);
    digitalWrite(ledPin10, LOW);
    Serial.print("TEMPERATURE EXCEEDED");
    Serial.println();
    Serial.print("FAN ON");
    Serial.println();

    lcd.setCursor(0, 1);
    lcd.print("TEMP EXCEEDED");
    lcd.setCursor(0, 2);
    lcd.print("FAN ON");
    delay(2000);
  }
  else if (t > 27 && t < 29)
  {
    digitalWrite(ledPin11, LOW);
    digitalWrite(ledPin10, HIGH);
    digitalWrite(ledPin9, LOW);
    Serial.print("TEMPERATURE NORMAL");
    Serial.println();

    lcd.setCursor(0, 1);
    lcd.print("TEMP NORMAL");
    lcd.setCursor(0, 2);
    lcd.print("ITS OPTIMAL");
    delay(2000);
  }
  else  {
    digitalWrite(ledPin11, LOW);
    digitalWrite(ledPin10, LOW);
    digitalWrite(ledPin9, HIGH);
    Serial.print("TEMPERATURE TOO LOW");
    Serial.println();
    Serial.print("HEAT LAMP ON");
    Serial.println();

    lcd.setCursor(0, 1);
    lcd.print("TEMP TOO LOW");
    lcd.setCursor(0, 2);
    lcd.print("HEAT LAMP ON");
    lcd.setCursor(0, 3);
    lcd.print("MSES <3");
    delay(2000);
  }

  //FOR HUMIDITY//
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("HUMIDITY: ");
  lcd.print(h);
  lcd.print("%");
  Serial.print(", ");
  Serial.print(h);
  if (h > 50) {

    digitalWrite(ledPin8, HIGH);
    digitalWrite(ledPin7, LOW);
    digitalWrite(ledPin6, LOW);
    lcd.setCursor (0, 1);
    lcd.print("HIGH HUMIDITY");
    lcd.setCursor (0, 2);
    lcd.print("FAN ON");
    delay (2000);

  } else if (h < 0) {

    digitalWrite(ledPin8, LOW);
    digitalWrite(ledPin7, LOW);
    digitalWrite(ledPin6, HIGH);
    lcd.setCursor (0, 1);
    lcd.print("LOW HUMIDITY");
    lcd.setCursor (0, 2);
    lcd.print("SPRAY ON");
    delay (2000);
  }
  else if (0 > h < 50) {

    digitalWrite(ledPin8, LOW);
    digitalWrite(ledPin7, HIGH);
    digitalWrite(ledPin6, LOW);
    lcd.setCursor (0, 1);
    lcd.print("ITS OPTIMAL");
    lcd.setCursor (0, 2);
    lcd.print("NO ACTION");
    delay (2000);
  }

  //FOR MOISTURE//
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("MOISTURE: ");
  lcd.print(soilstatus);
  lcd.print("%");
  Serial.print(", ");
  Serial.print(soilstatus);
  Serial.print("\n");

  if (soilstatus > 50) {

    digitalWrite(ledPin5, HIGH);
    digitalWrite(ledPin4, LOW);
    digitalWrite(ledPin3, LOW);
    lcd.setCursor (0, 1);
    lcd.print("HIGH MOISTURE");
    lcd.setCursor (0, 2);
    lcd.print("WATER OFF");
    delay (2000);

  } else if (soilstatus < 20) {

    digitalWrite(ledPin5, LOW);
    digitalWrite(ledPin4, LOW);
    digitalWrite(ledPin3, HIGH);
    lcd.setCursor (0, 1);
    lcd.print("LOW MOISTURE");
    lcd.setCursor (0, 2);
    lcd.print("WATER ON");
    delay (2000);
  }
  else if (0 > soilstatus < 50) {

    digitalWrite(ledPin5, LOW);
    digitalWrite(ledPin4, HIGH);
    digitalWrite(ledPin3, LOW);
    lcd.setCursor (0, 1);
    lcd.print("ITS OPTIMAL");
    lcd.setCursor (0, 2);
    lcd.print("NO ACTION");
    delay (2000);
  }

  //FOR WATER//
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("WATER LEVEL");
  lcd.print(waterstatus);
  lcd.print("%");
  Serial.print(", ");
  Serial.print(waterstatus);
  Serial.print("\n");

  if (waterstatus > 80) {

    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin0, LOW);
    lcd.setCursor (0, 1);
    lcd.print("HIGH");
    lcd.setCursor (0, 2);
    lcd.print("WATER OFF");
    delay (2000);

  } else if (waterstatus < 20) {

    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin0, HIGH);
    lcd.setCursor (0, 1);
    lcd.print("LOW");
    lcd.setCursor (0, 2);
    lcd.print("WATER ON");
    delay (2000);
  }
  else if (0 > waterstatus < 50) {

    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin0, LOW);
    lcd.setCursor (0, 1);
    lcd.print("MEDUIM");
    lcd.setCursor (0, 2);
    lcd.print("NO ACTION");
    delay (2000);
  }

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}
