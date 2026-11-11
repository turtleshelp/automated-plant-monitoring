/*
 * Automated Plant Growing & Monitoring System
 * -------------------------------------------
 * Refactored in 2026 from the 2020 APU course-project code (see ../../original/).
 * Same sensors and thresholds as 2020; bugs fixed, non-blocking loop, and a
 * relay-driven irrigation pump added as a clearly marked 2026 extension.
 *
 * Board : Arduino Uno
 * Inputs: LDR module (A0), water-level sensor (A1), soil-moisture sensor (A2),
 *         DHT11/DHT22 temperature + humidity (D2)
 * Output: 20x4 I2C LCD (A4/A5), indicator LEDs, relay module -> pump (D8)
 * Serial: one CSV line every 2 s at 9600 baud (works with Excel Data Streamer,
 *         as used for the 2020 data log)
 *
 * Libraries: "DHT sensor library" (Adafruit) + "Adafruit Unified Sensor",
 *            "LiquidCrystal I2C" (Frank de Brabander)
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include "control.h"

// ---- Configuration ----------------------------------------------------------
// The 2020 hardware used a DHT11. Wokwi only simulates the DHT22.
#define DHT_TYPE DHT22

const uint8_t PIN_DHT   = 2;
const uint8_t PIN_LDR   = A0;
const uint8_t PIN_TANK  = A1;
const uint8_t PIN_SOIL  = A2;

const uint8_t PIN_LED_GROW_LAMP = 3;  // stands in for the dimmable grow lamp
const uint8_t PIN_LED_FAN       = 4;  // stands in for the fan
const uint8_t PIN_LED_HEAT_LAMP = 5;  // stands in for the heat lamp
const uint8_t PIN_LED_SPRAY     = 6;  // stands in for the water spray
const uint8_t PIN_LED_REFILL    = 7;  // tank low warning
const uint8_t PIN_RELAY_PUMP    = 8;  // 2026 extension: relay -> irrigation pump

// Sensor calibration (raw ADC). Soil values are from the 2020 sketch
// (map(raw, 550, 10, 0, 100)); recalibrate for your own probe.
const int SOIL_RAW_DRY  = 550;
const int SOIL_RAW_WET  = 10;
const int TANK_RAW_EMPTY = 0;
const int TANK_RAW_FULL  = 1023;

const unsigned long SAMPLE_PERIOD_MS = 2000;  // DHT sensors need >= 2 s
const unsigned long PAGE_PERIOD_MS   = 3000;  // LCD page rotation

// ---- Globals ------------------------------------------------------------------
LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(PIN_DHT, DHT_TYPE);

plant::Readings readings = {0, NAN, NAN, 0, 0};
plant::Outputs  outputs;
unsigned long lastSampleMs = 0;
unsigned long lastPageMs   = 0;
uint8_t page = 0;

// ---- Helpers ------------------------------------------------------------------
// Print exactly 20 characters on a row so old text is overwritten without
// clearing the display (avoids flicker).
void lcdRow(uint8_t row, const char* text) {
  char buf[21];
  snprintf(buf, sizeof(buf), "%-20s", text);
  lcd.setCursor(0, row);
  lcd.print(buf);
}

// Format a float with one decimal (AVR snprintf has no %f).
void fmt1(char* out, size_t n, float v) {
  if (plant::isNan(v)) { snprintf(out, n, "--.-"); return; }
  int t10 = (int)(v * 10.0f + (v >= 0 ? 0.5f : -0.5f));
  int whole = t10 / 10;
  int frac = abs(t10 % 10);
  if (t10 < 0 && whole == 0) snprintf(out, n, "-0.%d", frac);
  else snprintf(out, n, "%d.%d", whole, frac);
}

const char* onOff(bool b) { return b ? "ON " : "OFF"; }

void readSensors() {
  readings.light        = analogRead(PIN_LDR);
  readings.temperatureC = dht.readTemperature();
  readings.humidity     = dht.readHumidity();
  readings.soilPct      = plant::adcToPct(analogRead(PIN_SOIL), SOIL_RAW_DRY, SOIL_RAW_WET);
  readings.tankPct      = plant::adcToPct(analogRead(PIN_TANK), TANK_RAW_EMPTY, TANK_RAW_FULL);
}

void applyOutputs() {
  digitalWrite(PIN_LED_GROW_LAMP, outputs.growLamp);
  digitalWrite(PIN_LED_FAN,       outputs.fan);
  digitalWrite(PIN_LED_HEAT_LAMP, outputs.heatLamp);
  digitalWrite(PIN_LED_SPRAY,     outputs.spray);
  digitalWrite(PIN_LED_REFILL,    outputs.refill);
  digitalWrite(PIN_RELAY_PUMP,    outputs.pump);
}

void printCsv() {
  char t[8], h[8];
  fmt1(t, sizeof(t), readings.temperatureC);
  fmt1(h, sizeof(h), readings.humidity);
  char line[80];
  snprintf(line, sizeof(line), "%d,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d",
           readings.light, t, h, readings.soilPct, readings.tankPct,
           outputs.growLamp, outputs.fan, outputs.heatLamp,
           outputs.spray, outputs.pump, outputs.refill);
  Serial.println(line);
}

void drawLcd() {
  char t[8], h[8], row[24];
  fmt1(t, sizeof(t), readings.temperatureC);
  fmt1(h, sizeof(h), readings.humidity);

  // Row 0-1: readings (always visible)
  snprintf(row, sizeof(row), "T:%sC  H:%s%%", t, h);
  lcdRow(0, row);
  snprintf(row, sizeof(row), "L:%-4d S:%3d%% W:%3d%%", readings.light,
           readings.soilPct, readings.tankPct);
  lcdRow(1, row);

  // Row 2-3: actuator states, two pages
  if (page == 0) {
    snprintf(row, sizeof(row), "Lamp:%s  Fan :%s", onOff(outputs.growLamp), onOff(outputs.fan));
    lcdRow(2, row);
    snprintf(row, sizeof(row), "Heat:%s  Spray:%s", onOff(outputs.heatLamp), onOff(outputs.spray));
    lcdRow(3, row);
  } else {
    snprintf(row, sizeof(row), "Pump:%s", onOff(outputs.pump));
    lcdRow(2, row);
    lcdRow(3, outputs.refill ? "TANK LOW - REFILL" : "Tank OK");
  }
}

// ---- Arduino entry points ---------------------------------------------------
void setup() {
  Serial.begin(9600);
  dht.begin();

  const uint8_t outs[] = {PIN_LED_GROW_LAMP, PIN_LED_FAN, PIN_LED_HEAT_LAMP,
                          PIN_LED_SPRAY, PIN_LED_REFILL, PIN_RELAY_PUMP};
  for (uint8_t i = 0; i < sizeof(outs); i++) {
    pinMode(outs[i], OUTPUT);
    digitalWrite(outs[i], LOW);
  }

  lcd.init();
  lcd.backlight();
  lcdRow(0, "Plant Monitor");
  lcdRow(1, "APU MSES 2020");
  lcdRow(2, "Refactored 2026");
  lcdRow(3, "");

  Serial.println(F("light,temp_c,humidity,soil_pct,tank_pct,lamp,fan,heat,spray,pump,refill"));
  lastSampleMs = millis();  // splash stays up until the first sample (2 s)
  lastPageMs = millis();
}

void loop() {
  unsigned long now = millis();

  if (now - lastSampleMs >= SAMPLE_PERIOD_MS) {
    lastSampleMs = now;
    readSensors();
    outputs = plant::update(readings, outputs);
    applyOutputs();
    printCsv();
    drawLcd();
  }

  if (now - lastPageMs >= PAGE_PERIOD_MS) {
    lastPageMs = now;
    page ^= 1;
    drawLcd();
  }
}
