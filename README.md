# Automated Plant Growing & Monitoring System

An Arduino Uno system that watches a plant's light, temperature, humidity, soil moisture and water tank, switches a grow lamp, fan, heat lamp, water spray and irrigation pump to keep it in an optimal range, and shows the readings on a 20x4 LCD.

> **Originally developed 2020 (APU course project); refactored from the surviving code and report in 2026.**
> The original code is kept unchanged in [`original/`](original/). The code in [`src/`](src/) is a 2026 rewrite with the same sensors and thresholds. The relay-driven pump is a 2026 addition, marked as such throughout.

**▶ [Run it in your browser on Wokwi](https://wokwi.com/projects/480434156220494849)** (no hardware or install needed)

[![Wokwi simulation](media/2026-wokwi-lcd.jpg)](https://wokwi.com/projects/480434156220494849)

## Motivation

Manually checking and watering plants is time-consuming and easy to get wrong. The course brief was to design a microcontroller control system that reads input parameters, drives outputs and displays both. Our group chose an automated plant monitor: sense the growing conditions and react automatically, so the plant needs less human attention and less water is wasted.

## My role

This was a **group project of three students** (Microprocessor Systems & Embedded Software module, B.Eng. Mechatronics, Asia Pacific University, Kuala Lumpur, Aug–Sep 2020). Each member built one subsystem and the group integrated them.

**My part (individual component):**
- **Light control:** LDR light sensor; grow lamp switched on when dark, off when bright, unchanged in between.
- **Humidity control:** DHT11 sensor; water spray on when the air is too dry, fan on when too humid.
- **Enhancement:** a water-level sensor on the spray's water tank, so the user knows when to refill.
- Interface design, Arduino code, Proteus simulation, hardware test and my section of the report.

**Teammates' parts:** temperature control (fan / heat lamp) and soil moisture, plus joint integration, testing and presentation.

**2026 (solo):** refactored the code, fixed the bugs listed in [`original/README.md`](original/README.md), separated the decision logic so it can be unit-tested, added the relay/pump extension, and built the Wokwi simulation and wiring diagram.

## Tech stack

| | |
|---|---|
| Microcontroller | Arduino Uno (ATmega328P) |
| Sensors | DHT11 temperature + humidity (DHT22 in Wokwi), LDR light sensor, soil-moisture sensor, water-level sensor |
| Outputs | 20x4 I2C LCD, LED indicators standing in for lamp / fan / heat lamp / spray, relay module + pump (2026) |
| Language | Arduino C/C++ |
| Tools | Proteus (2020 simulation), Excel Data Streamer (2020 logging), Wokwi (2026 simulation), g++ (host unit tests) |

## Architecture

| Block diagram (2020 report) | Control flow (2020 report) |
|---|---|
| ![Block diagram](media/2020-block-diagram.png) | ![Flowchart](media/2020-flowchart.png) |

**Control rules** (thresholds from the 2020 project; inside each "optimal" band the output keeps its last state):

| Input | Condition | Action |
|---|---|---|
| Light (raw ADC, higher = darker) | > 200 | Grow lamp ON |
| | < 150 | Grow lamp OFF |
| Humidity | > 60 % | Fan ON, spray OFF |
| | < 50 % | Spray ON |
| Temperature | > 29 °C | Fan ON |
| | < 27 °C | Heat lamp ON |
| Water tank | < 20 % | Refill warning (clears above 30 %), pump locked out |
| Soil moisture *(2026: drives pump)* | < 20 % | Pump ON |
| | > 50 % | Pump OFF |

The decision logic lives in [`src/plant_monitor/control.h`](src/plant_monitor/control.h) with no Arduino dependencies; the sketch only reads sensors, calls `plant::update()` and writes outputs. The loop is non-blocking (`millis()`), samples every 2 s, prints a CSV line to Serial and rotates the bottom half of the LCD every 3 s.

### Wiring

![Wiring diagram](docs/wiring.png)

| Uno pin | Connected to |
|---|---|
| D2 | DHT22 / DHT11 data |
| A0 | LDR module AO |
| A1 | Water-level sensor (potentiometer in Wokwi) |
| A2 | Soil-moisture sensor (potentiometer in Wokwi) |
| A4 / A5 | LCD I2C SDA / SCL (address 0x27) |
| D3 – D7 | LEDs + 220 Ω: grow lamp, fan, heat lamp, spray, refill |
| D8 | Relay module IN (2026), relay switches the pump |

## Results

**2020 hardware test:** the system ran on a real Arduino Uno with a DHT11 and a 20x4 I2C LCD; the actuators were represented by LEDs. Photos from the report:

| High humidity → fan | Dark → lamp on | Empty tank |
|---|---|---|
| ![](media/2020-hardware-lcd-humidity.jpg) | ![](media/2020-hardware-lcd-light.jpg) | ![](media/2020-hardware-lcd-water-level.jpg) |

**2020 data log** (Serial → Excel Data Streamer, about 30 s at room conditions): light ≈ 277, 28.1 °C, 76–77 % RH. The moisture column reads **-86 %**, which is a bug: the soil reading was not clamped (see [`original/README.md`](original/README.md)).

![Data log](media/2020-data-log.png)

The report also contains a Proteus results table. It does not fully agree with the code's thresholds (for example, light 123 is listed as "lamp on"), so I have not reproduced it here; the hardware photos above match the code.

**2026 Wokwi simulation:** the refactored sketch compiles and runs in Wokwi. With the default sensor values (24 °C, 40 % RH, dark, soil dry, tank 68 %) it turns on the grow lamp, heat lamp, spray and, through the relay, the pump:

![Relay driving the pump LED in Wokwi](media/2026-wokwi-relay-pump.jpg)

**Unit tests:** `tests/test_control.cpp` checks every threshold, the hold-in-band behaviour, fail-safe on DHT read errors, the pump lockout and the clamping that fixes the -86 % bug.

What could **not** be reproduced: no real pump, fan or lamp was ever driven (2020 or 2026), and the soil and water sensors in Wokwi are potentiometers, so the calibration values are from the 2020 code, not new measurements.

## How to run

### Run it in the Wokwi simulator (no hardware)

**In the browser:** open the saved project at <https://wokwi.com/projects/480434156220494849> and press ▶.

To rebuild it from this repo instead:
1. Open <https://wokwi.com/projects/new/arduino-uno>.
2. Replace `sketch.ino` with [`src/plant_monitor/plant_monitor.ino`](src/plant_monitor/plant_monitor.ino).
3. Add a new file `control.h` with the contents of [`src/plant_monitor/control.h`](src/plant_monitor/control.h).
4. Replace `diagram.json` with [`wokwi/diagram.json`](wokwi/diagram.json).
5. In **Library Manager** add `DHT sensor library` and `LiquidCrystal I2C` (or add a `libraries.txt` with [these lines](wokwi/libraries.txt)).
6. Press ▶. Click the DHT22, the LDR module or the two potentiometers while it runs to change the readings.

**In VS Code** (Wokwi extension + [arduino-cli](https://arduino.github.io/arduino-cli/)):
```bash
arduino-cli core install arduino:avr
arduino-cli lib install "DHT sensor library" "Adafruit Unified Sensor" "LiquidCrystal I2C"
arduino-cli compile --fqbn arduino:avr:uno --output-dir build src/plant_monitor
```
Then open `wokwi/wokwi.toml` and run **Wokwi: Start Simulator**.

### On real hardware
Wire it as in the table above, change `#define DHT_TYPE DHT22` to `DHT11` if that is your sensor, recalibrate `SOIL_RAW_DRY` / `SOIL_RAW_WET` for your probe and upload with the Arduino IDE. Open the Serial Monitor at 9600 baud for the CSV log. A real pump needs its own supply on the relay contacts, not the Uno's 5 V.

### Unit tests (any PC with g++)
```bash
cd tests
g++ -std=c++11 -Wall -I../src/plant_monitor test_control.cpp -o test_control && ./test_control
```

## Repository structure

```
src/plant_monitor/   refactored sketch + control.h (decision logic)
tests/               host-side unit tests for control.h
wokwi/               diagram.json, libraries.txt, wokwi.toml
docs/                wiring diagram (SVG + PNG)
media/               figures and photos from the 2020 report, 2026 simulation screenshots
original/            unmodified 2020 code (my individual sketch + the group's integrated sketch)
```

## Status

Complete as a portfolio piece. The 2020 hardware no longer exists; the 2026 version is verified in simulation and by unit tests only.

## What I'd improve

- Replace the bang-bang lamp with PWM dimming (the 2020 design called for a *dimmable* lamp but only switched it).
- Capacitive soil sensor instead of a resistive one (resistive probes corrode), with a proper two-point calibration routine.
- Log to an SD card or send data over Wi-Fi (ESP32) instead of a PC-tethered serial log; the 2020 report proposed a GSM SMS alert for low water, which was never built.
- Run the pump for a timed pulse and wait for the water to soak in, instead of watering until the sensor reads "wet".

## Credits

- Muhammad Ali Sadiq: light and humidity subsystems, water-level enhancement, 2026 refactor.
- Two teammates at Asia Pacific University: temperature and soil-moisture subsystems, group integration.
- Libraries: [Adafruit DHT sensor library](https://github.com/adafruit/DHT-sensor-library), [LiquidCrystal I2C](https://github.com/johnrickman/LiquidCrystal_I2C).

Licensed under the [MIT License](LICENSE).
