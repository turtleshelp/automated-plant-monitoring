# Original 2020 code (historical record)

`ali_individual/LDR.ino` is my 2020 sketch for light (LDR), humidity (DHT11) and water-level
monitoring on an Arduino Uno with a 20x4 I2C LCD. It is kept **as it was in 2020**, except for
one splash-screen line edited for publication (marked with a `[2026]` comment).

It is **not** the recommended code. Known issues, all fixed in `../src/`:

- Range checks like `else if (150 > ldrStatus < 200)` do not do what they look like in C
  (`(150 > x) < 200` is always true).
- LEDs are written on pins 0 and 1, which are the Uno's Serial RX/TX, while Serial is in use,
  and those pins are never set as outputs.
- `lcd.begin()` is called on every loop iteration, and `delay(2000)` per screen makes the loop
  take several seconds, so readings update slowly.
- The temperature is read but never used.
- The splash screen still says "Weed Monitor", left over from a template.

A soil-mapping bug in the 2020 system (unclamped `map(raw, 550, 10, 0, 100)`, which logged
-86 % moisture) is also fixed in the 2026 version.
