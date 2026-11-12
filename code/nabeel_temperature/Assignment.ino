#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2,1 ,0, 4,5,6,7,3, POSITIVE);
int val;
int tempPin = 1;
int heat = 5;
int ledR = 6;
int ledG = 7;
int fan = 2;

void setup() 
{
  lcd.begin(20,4);
  Serial.begin(9600);
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(heat, OUTPUT);
}

void loop() 
{
  val = analogRead(tempPin);
  float mv =(val/1024.0)*5000;
  float cel = mv/10;
  float farh = (cel*9)/5 + 32;


  Serial.print("Temperature = ");
  Serial.print(cel);
  Serial.print(" degree celcius");
  Serial.println();
  
  lcd.setCursor(0,0);
  lcd.print("TEMPERATURE =");
  lcd.setCursor(14,0);
  lcd.print(cel);
  delay(2000);

if(cel > 29)
{
digitalWrite(ledR, HIGH);
digitalWrite(ledG, LOW);
digitalWrite(fan, HIGH);
digitalWrite(heat, LOW);
Serial.print("TEMPERATURE EXCEEDED");
Serial.println();
Serial.print("FAN ON");
Serial.println();
lcd.clear();
lcd.setCursor(0,1);
lcd.print("TEMP EXCEEDED");
lcd.setCursor(0,2);
lcd.print("FAN ON");
}
else if(cel> 27 && cel < 29)
{
digitalWrite(ledG, HIGH);
digitalWrite(ledR, LOW);
digitalWrite(fan, LOW);
digitalWrite(heat, LOW);
Serial.print("TEMPERATURE NORMAL");
Serial.println();
lcd.clear();
lcd.setCursor(0,1);
lcd.print("TEMP NORMAL");
lcd.setCursor(0,2);
lcd.print("hope you like");
}
else 
{
digitalWrite(ledR, HIGH);
digitalWrite(ledG, LOW);
digitalWrite(fan, LOW);
digitalWrite(heat, HIGH);
Serial.print("TEMPERATURE TOO LOW");
Serial.println();
Serial.print("HEAT LAMP ON");
Serial.println();
lcd.clear();
lcd.setCursor(0,1);
lcd.print("TEMP TOO LOW");
lcd.setCursor(0,2);
lcd.print("HEAT LAMP ON");
lcd.setCursor(0,3);
lcd.print("MSES <3");

}
}



  
