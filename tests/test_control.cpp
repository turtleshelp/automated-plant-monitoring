// Host-side unit tests for src/plant_monitor/control.h
// Build & run:  g++ -std=c++11 -Wall -I../src/plant_monitor test_control.cpp -o test_control && ./test_control
#include <cmath>
#include <cstdio>
#include "control.h"

using namespace plant;

static int failures = 0;
#define CHECK(cond)                                                      \
  do {                                                                   \
    if (!(cond)) { std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); failures++; } \
  } while (0)

static Readings optimal() { return Readings{175, 28.0f, 55.0f, 35, 60}; }

int main() {
  Outputs off;

  // adcToPct: clamping and inverted mapping (the 2020 -86 % bug)
  CHECK(adcToPct(1015, 550, 10) == 0);    // dry/disconnected probe no longer negative
  CHECK(adcToPct(10, 550, 10) == 100);
  CHECK(adcToPct(280, 550, 10) == 50);
  CHECK(adcToPct(512, 0, 1023) == 50);
  CHECK(adcToPct(-5, 0, 1023) == 0);
  CHECK(adcToPct(5, 7, 7) == 0);

  // Everything optimal from an all-off state -> nothing switches on
  Outputs o = update(optimal(), off);
  CHECK(!o.growLamp && !o.fan && !o.heatLamp && !o.spray && !o.pump && !o.refill);

  // Light: dark -> lamp on, optimal band holds, bright -> off
  Readings r = optimal();
  r.light = 295;  o = update(r, off);  CHECK(o.growLamp);
  r.light = 175;  o = update(r, o);    CHECK(o.growLamp);   // hold
  r.light = 123;  o = update(r, o);    CHECK(!o.growLamp);
  r.light = 175;  o = update(r, o);    CHECK(!o.growLamp);  // hold
  r.light = 200;  o = update(r, off);  CHECK(!o.growLamp);  // boundary is exclusive

  // Humidity: high -> fan on, spray off; low -> spray on
  r = optimal();
  r.humidity = 73; o = update(r, off); CHECK(o.fan && !o.spray);
  r.humidity = 15; o = update(r, o);   CHECK(!o.fan && o.spray);
  r.humidity = 55; o = update(r, o);   CHECK(!o.fan && o.spray);  // spray holds

  // Temperature: hot -> fan; cold -> heat lamp; normal -> neither
  r = optimal();
  r.temperatureC = 31; o = update(r, off); CHECK(o.fan && !o.heatLamp);
  r.temperatureC = 25; o = update(r, o);   CHECK(!o.fan && o.heatLamp);
  r.temperatureC = 28; o = update(r, o);   CHECK(!o.fan && !o.heatLamp);

  // Fan is OR of temperature and humidity demand
  r = optimal(); r.temperatureC = 31; r.humidity = 40;
  o = update(r, off); CHECK(o.fan && o.spray);

  // DHT failure (NaN) -> spray, heat lamp and fan fail safe (off)
  r = optimal(); r.temperatureC = NAN; r.humidity = NAN;
  Outputs allOn; allOn.fan = allOn.spray = allOn.heatLamp = true;
  o = update(r, allOn); CHECK(!o.fan && !o.spray && !o.heatLamp);

  // Soil -> pump with hysteresis
  r = optimal();
  r.soilPct = 10; o = update(r, off); CHECK(o.pump);
  r.soilPct = 35; o = update(r, o);   CHECK(o.pump);   // hold while watering
  r.soilPct = 60; o = update(r, o);   CHECK(!o.pump);
  r.soilPct = 35; o = update(r, o);   CHECK(!o.pump);  // hold while drying

  // Tank low -> refill warning and pump lockout, clears above 30 %
  r = optimal(); r.soilPct = 10;
  r.tankPct = 10; o = update(r, off); CHECK(o.refill && !o.pump);
  r.tankPct = 25; o = update(r, o);   CHECK(o.refill && !o.pump);  // hysteresis
  r.tankPct = 40; o = update(r, o);   CHECK(!o.refill && o.pump);

  if (failures == 0) std::printf("All control tests passed.\n");
  return failures == 0 ? 0 : 1;
}
