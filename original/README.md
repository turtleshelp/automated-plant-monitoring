# Original 2020 code (historical record)

These sketches are kept **as they were in 2020**, with one exception: the LCD
splash screen printed the team members' nicknames, which were replaced with
`"Ali + team"` for publication (marked with a `[2026]` comment).

| File | Author | What it does |
|------|--------|--------------|
| `ali_individual/LDR.ino` | Muhammad Ali Sadiq | My individual part: LDR light control, DHT11 humidity control, water-level enhancement |
| `group_integrated/PLANT_MONITOR.ino` | Group (me + two teammates) | Final integrated system: light, temperature, humidity, soil moisture, water level |

They are **not** the recommended code. Known issues, all fixed in `../src/`:

- Range checks like `else if (150 > ldrStatus < 200)` do not do what they look like in C
  (`(150 > x) < 200` is always true).
- In the group sketch, the water level is calculated from the **soil sensor's** voltage
  (`waterstatus = (SensorVolts / 5) * 100` with `SensorVolts` read from `A3`).
- `soilstatus` is computed twice; a local `int soilstatus` shadows the global one.
- LEDs are driven on pins 0 and 1, which are the Uno's Serial RX/TX, while Serial is in use.
- The DHT data pin (2) is also configured as an LED output.
- `map(soil, 550, 10, 0, 100)` is not clamped: an out-of-range reading gives negative
  percentages (the 2020 data log shows **-86 %** moisture).
- `lcd.begin()` is called on every loop iteration, and `delay(2000)` per screen makes the
  whole loop take about 10 s.
- The splash screen still says "Weed Monitor", left over from a template.
