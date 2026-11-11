const int ledPin13 = 13;
const int ledPin12 = 12;
const int ledPin8 = 8;
const int ledPin7 = 7;
const int ledPin6 = 6;
const int ledPin1 = 1;
const int ledPin0 = 0;
const int ldrPin = A0;
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
  pinMode(ledPin8, OUTPUT);
  pinMode(ledPin7, OUTPUT);
  pinMode(ledPin6, OUTPUT);
  pinMode(ldrPin, INPUT);


  lcd.begin();
  lcd.print("Weed Monitor");
  lcd.setCursor (0, 1);
  lcd.print("MSES <3");
  lcd.setCursor (0, 2);
  lcd.print("By");
  lcd.setCursor (0, 3);
  lcd.print("Ali"); // [2026] splash text edited for publication
  delay(2000);
}

void loop() {
  float h = dht.readHumidity(); //FOR TEMP
  float t = dht.readTemperature(); //FOR HUMID
  int ldrStatus = analogRead(ldrPin); //FOR LDR

  int SensorValue = analogRead(waterPin);
  float SensorVolts = analogRead(waterPin) * 0.0048828125;
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

 

  //FOR HUMIDITY//
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("HUMIDITY: ");
  lcd.print(h);
  lcd.print("%");
  Serial.print(", ");
  Serial.print(h);
  if (h > 60) {

    digitalWrite(ledPin8, HIGH);
    digitalWrite(ledPin7, LOW);
    digitalWrite(ledPin6, LOW);
    lcd.setCursor (0, 1);
    lcd.print("HIGH HUMIDITY");
    lcd.setCursor (0, 2);
    lcd.print("FAN ON");
    delay (2000);

  } else if (h < 50) {

    digitalWrite(ledPin8, LOW);
    digitalWrite(ledPin7, LOW);
    digitalWrite(ledPin6, HIGH);
    lcd.setCursor (0, 1);
    lcd.print("LOW HUMIDITY");
    lcd.setCursor (0, 2);
    lcd.print("SPRAY ON");
    delay (2000);
  }
  else if (50 > h < 60) {

    digitalWrite(ledPin8, LOW);
    digitalWrite(ledPin7, HIGH);
    digitalWrite(ledPin6, LOW);
    lcd.setCursor (0, 1);
    lcd.print("ITS OPTIMAL");
    lcd.setCursor (0, 2);
    lcd.print("NO ACTION");
    delay (2000);
  }



  //FOR WATER//
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print("WATER LEVEL ");
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
