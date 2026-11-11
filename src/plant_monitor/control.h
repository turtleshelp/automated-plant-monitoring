// control.h - pure decision logic for the plant monitor.
//
// No Arduino dependencies, so it can be unit-tested on a PC (see tests/).
// Thresholds are the ones used in the 2020 course project; each controller
// keeps its previous state inside the "optimal" band (the report: "within
// 150-200 no change will occur"), which acts as hysteresis.
#pragma once

namespace plant {

// ---- Thresholds (from the 2020 project) ------------------------------------
// Light: raw ADC value from the LDR divider. HIGHER value = DARKER.
const int LIGHT_DARK_ABOVE   = 200;  // > 200  -> dark   -> grow lamp ON
const int LIGHT_BRIGHT_BELOW = 150;  // < 150  -> bright -> grow lamp OFF

const float HUMIDITY_HIGH_ABOVE = 60.0f;  // > 60 % -> fan ON,  spray OFF
const float HUMIDITY_LOW_BELOW  = 50.0f;  // < 50 % -> spray ON, fan OFF

const float TEMP_HIGH_ABOVE = 29.0f;  // > 29 C -> fan ON
const float TEMP_LOW_BELOW  = 27.0f;  // < 27 C -> heat lamp ON

const int SOIL_DRY_BELOW = 20;  // < 20 % -> irrigation pump ON
const int SOIL_WET_ABOVE = 50;  // > 50 % -> irrigation pump OFF

const int TANK_LOW_BELOW = 20;  // < 20 % -> refill warning, pump locked out
const int TANK_OK_ABOVE  = 30;  // warning clears again above 30 % (2026)

// ---- Types -----------------------------------------------------------------
struct Readings {
  int   light;        // raw ADC 0..1023 (higher = darker)
  float temperatureC; // NaN if the DHT read failed
  float humidity;     // %RH, NaN if the DHT read failed
  int   soilPct;      // 0..100 %
  int   tankPct;      // 0..100 %
};

struct Outputs {
  bool growLamp = false;
  bool fan      = false;  // driven by temperature OR humidity
  bool heatLamp = false;
  bool spray    = false;
  bool pump     = false;  // 2026 extension: relay-driven irrigation pump
  bool refill   = false;  // tank low warning
};

// ---- Helpers ---------------------------------------------------------------
inline int clampPct(long v) { return v < 0 ? 0 : (v > 100 ? 100 : (int)v); }

// Linear map of a raw ADC value to 0..100 %, clamped. Works for inverted
// sensors too (rawAt0 > rawAt100). The 2020 code used map() without clamping,
// which is how a disconnected soil probe logged as -86 %.
inline int adcToPct(int raw, int rawAt0, int rawAt100) {
  if (rawAt0 == rawAt100) return 0;
  long pct = (long)(raw - rawAt0) * 100L / (long)(rawAt100 - rawAt0);
  return clampPct(pct);
}

inline bool isNan(float x) { return x != x; }

// ---- Controller --------------------------------------------------------------
// Returns the new output state given the readings and the previous state.
inline Outputs update(const Readings& r, const Outputs& prev) {
  Outputs o = prev;

  // Light (Ali, 2020)
  if (r.light > LIGHT_DARK_ABOVE)        o.growLamp = true;
  else if (r.light < LIGHT_BRIGHT_BELOW) o.growLamp = false;

  // Humidity (Ali, 2020) and temperature (team, 2020) share the fan.
  // Spray holds its state in the optimal band; the fan is only requested
  // while a reading is actually too high.
  bool fanForHumidity = false;
  bool fanForTemp     = false;
  if (!isNan(r.humidity)) {
    if (r.humidity > HUMIDITY_HIGH_ABOVE)     { o.spray = false; fanForHumidity = true; }
    else if (r.humidity < HUMIDITY_LOW_BELOW) { o.spray = true; }
  } else {
    o.spray = false;  // sensor fault: fail safe
  }
  o.heatLamp = false;
  if (!isNan(r.temperatureC)) {
    if (r.temperatureC > TEMP_HIGH_ABOVE)     fanForTemp = true;
    else if (r.temperatureC < TEMP_LOW_BELOW) o.heatLamp = true;
  }
  o.fan = fanForHumidity || fanForTemp;

  // Water tank (Ali's enhancement, 2020; hysteresis added 2026)
  if (r.tankPct < TANK_LOW_BELOW)     o.refill = true;
  else if (r.tankPct > TANK_OK_ABOVE) o.refill = false;

  // Soil moisture -> irrigation pump (2026 extension; 2020 version lit an LED)
  if (r.soilPct < SOIL_DRY_BELOW)      o.pump = true;
  else if (r.soilPct > SOIL_WET_ABOVE) o.pump = false;
  if (o.refill) o.pump = false;  // never run the pump dry

  return o;
}

}  // namespace plant
