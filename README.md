# Countimer

[![CI](https://github.com/inflop/Countimer/actions/workflows/ci.yml/badge.svg)](https://github.com/inflop/Countimer/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/inflop/Countimer)](https://github.com/inflop/Countimer/releases)
[![License: MIT](https://img.shields.io/github/license/inflop/Countimer)](LICENSE)
[![arduino-library-badge](https://www.ardu-badge.com/badge/Countimer.svg)](https://www.ardu-badge.com/Countimer)
[![Claude Code](https://img.shields.io/badge/Claude%20Code-assisted-D97757?logo=claude&logoColor=white)](https://claude.com/claude-code)

A simple, non-blocking Arduino timer and counter library. Count down, count up, or just
call a function on a fixed interval — without `delay()`, interrupts, or external dependencies.

## Features

- **Non-blocking** — driven entirely by `millis()` from your `loop()`; no `delay()`, no interrupts.
- **Three work modes:**
  - **Count-down** from a set time (up to 999:59:59), with a callback when it reaches zero.
  - **Count-up** from zero to a set time, with a callback when it completes.
  - **Interval-only** — call a function every _n_ milliseconds, no counter at all.
- **Two callbacks per timer** — one fired every interval tick, one fired once on completion.
- Full control: `start()`, `pause()` (resumable), `restart()`, `reset()`, `stop()`.
- Accuracy independent of count length and `loop()` load — counting is based on the real
  measured `millis()` delta, so busy sketches don't accumulate drift.
- Optional [calibration](#accuracy-and-calibration) of the hardware clock drift.
- No dependencies, works on any architecture (AVR, ESP8266/ESP32, SAMD, RP2040, …).

## Installation

**Arduino IDE (Library Manager):** open *Sketch → Include Library → Manage Libraries…*,
search for **Countimer** and click *Install*.

**arduino-cli:**

```sh
arduino-cli lib install Countimer
```

**Manually:** download this repository and place it in your Arduino `libraries/` folder
(e.g. `~/Documents/Arduino/libraries/Countimer`), then restart the IDE.

## Quick start

A 10-second count-down that prints the remaining time every second:

```cpp
#include <Countimer.h>

Countimer timer;

void setup() {
    Serial.begin(9600);

    // Count down from 00h:00m:10s and call onComplete() when finished.
    timer.setCounter(0, 0, 10, timer.COUNT_DOWN, onComplete);

    // Call printTime() every 1000 ms while the timer is running.
    timer.setInterval(printTime, 1000);

    // Nothing happens until the timer is started.
    timer.start();
}

void loop() {
    // run() is the heartbeat — it must be called on every loop() iteration.
    timer.run();
}

void printTime() {
    Serial.println(timer.getCurrentTime());  // e.g. "00:00:07"
}

void onComplete() {
    Serial.println("Complete!");
}
```

## Usage

### Work modes

The mode is selected by the `CountType` passed to `setCounter()` — or by not calling
`setCounter()` at all:

```cpp
// Count-down: 5 minutes -> 00:00:00, then onComplete() fires once.
timer.setCounter(0, 5, 0, timer.COUNT_DOWN, onComplete);

// Count-up: 00:00:00 -> 5 minutes, then onComplete() fires once.
timer.setCounter(0, 5, 0, timer.COUNT_UP, onComplete);

// Interval-only: no counter, just call blink() every 500 ms until stopped.
// (Only setInterval() is configured — no setCounter().)
timer.setInterval(blink, 500);
```

Hours are clamped to 999, minutes and seconds to 59, so the longest count is `999:59:59`.

The 3-argument `setCounter(hours, minutes, seconds)` overload re-programs the time of an
already configured timer while keeping its mode and completion callback.

For sub-second precision, pass an extra `milliseconds` (0-999) argument:

```cpp
// Count down 1.5 s, then onComplete() fires once.
timer.setCounter(0, 0, 1, 500, timer.COUNT_DOWN, onComplete);
```

### Controlling the timer

```cpp
timer.start();    // start, or resume after pause()
timer.pause();    // freeze; time spent paused is NOT counted
timer.restart();  // reset to the initial time and start again
timer.reset();    // reset to the initial time and stay stopped, ready to start()
timer.stop();     // finish: mark completed and reset to the initial time
```

Calling `start()` repeatedly (e.g. on every `loop()` iteration) is harmless — it only acts
on an actual stopped-to-running transition.

### Reading the time and state

```cpp
timer.getCurrentTime();            // "HH:MM:SS" as char*
timer.getCurrentTimeWithMillis();  // "HH:MM:SS.mmm" as char*
timer.getCurrentHours();           // uint16_t
timer.getCurrentMinutes();         // uint8_t
timer.getCurrentSeconds();         // uint8_t
timer.getCurrentMilliseconds();    // uint16_t (0-999)

timer.isCounterRunning();   // true while counting
timer.isStopped();          // true when stopped or paused
timer.isCounterCompleted(); // true once the count has finished
```

> **Note:** `getCurrentTime()` returns a pointer to an internal buffer that is overwritten
> on the next call — print or copy it immediately, don't store the pointer.

## Accuracy and calibration

The counter is driven by the real measured `millis()` delta, so its accuracy does not degrade
with longer count times, regardless of how busy your `loop()` is.

The remaining error source is the hardware clock itself — boards with a ceramic resonator
(most Uno/Nano clones) can drift up to ~0.5% against a real clock. You can compensate for it
with a one-time, per-board calibration:

```cpp
// Run a long count (e.g. 1 hour) against a reference clock, then:
// factor = real_elapsed_seconds / timer_indicated_seconds
timer.setCalibration(1.001);  // timer was running ~3.6s/h too slow
```

The default factor is `1.0` (no correction).

## API reference

| Method | Description |
|---|---|
| `void setCounter(uint16_t hours, uint8_t minutes, uint8_t seconds, CountType countType, timer_callback onComplete)` | Configure the count time, mode (`COUNT_DOWN` / `COUNT_UP` / `COUNT_NONE`) and the function called once when the count completes. |
| `void setCounter(uint16_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds, CountType countType, timer_callback onComplete)` | Same as above, with an additional sub-second component (0-999 ms). |
| `void setCounter(uint16_t hours, uint8_t minutes, uint8_t seconds)` | Re-program the count time of an already configured timer. |
| `void setCounter(uint16_t hours, uint8_t minutes, uint8_t seconds, uint16_t milliseconds)` | Same as above, with an additional sub-second component (0-999 ms). |
| `void setInterval(timer_callback callback, uint32_t interval)` | Call `callback` every `interval` milliseconds while the timer is running. |
| `void setCalibration(float factor)` | Correct hardware `millis()` drift; `factor = real_elapsed_time / timer_indicated_time`, default `1.0`. |
| `void run()` | Heartbeat — must be called on every `loop()` iteration. |
| `void start()` | Start the timer, or resume it after `pause()`. |
| `void pause()` | Pause the timer; paused time is not counted. Resume with `start()`. |
| `void restart()` | Reset to the initial time and start again. |
| `void reset()` | Reset to the initial time and leave the timer stopped (not completed), ready to `start()`. |
| `void stop()` | Finish the count: mark it completed and reset to the initial time. |
| `char* getCurrentTime()` | Current time formatted as `HH:MM:SS` (pointer to an internal buffer). |
| `char* getCurrentTimeWithMillis()` | Current time formatted as `HH:MM:SS.mmm` (pointer to an internal buffer, separate from `getCurrentTime()`'s). |
| `uint16_t getCurrentHours() const` | Current hours component. |
| `uint8_t getCurrentMinutes() const` | Current minutes component. |
| `uint8_t getCurrentSeconds() const` | Current seconds component. |
| `uint16_t getCurrentMilliseconds() const` | Current sub-second component (0-999). |
| `bool isCounterRunning() const` | `true` while the counter is running. |
| `bool isStopped() const` | `true` when the timer is stopped or paused. |
| `bool isCounterCompleted() const` | `true` once the count has finished. |

`timer_callback` is a plain `void (*)(void)` function pointer.

## Examples

| Sketch | Shows |
|---|---|
| [`examples/Basic`](examples/Basic/Basic.ino) | Minimal count-down timer — the quick start above as a sketch. |
| [`examples/Advanced`](examples/Advanced/Advanced.ino) | Count-up, pause/resume over serial, state queries, re-programming the time, calibration. |
| [`examples/CountimerTest`](examples/CountimerTest/CountimerTest.ino) | All three modes running side by side, controlled with serial keys `S`/`P`/`R`/`T`. |

Don't have a board handy? Every example above can also be run in the
[Wokwi](https://wokwi.com/) simulator (e.g. via the "Wokwi Simulator" VS Code extension) using
the `wokwi.toml`/`diagram.json` in its folder — see
[CONTRIBUTING.md](CONTRIBUTING.md#running-examples-in-the-wokwi-simulator) for the full steps.

## Compatibility

The library only depends on `millis()`, so it runs on any Arduino-compatible core
(`architectures=*`). CI compiles the examples for AVR (`arduino:avr:uno`) and
ESP32 (`esp32:esp32:esp32`).

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for release history.

## Contributing

Bug reports and pull requests are welcome on
[GitHub](https://github.com/inflop/Countimer/issues) — see
[CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

This library is released under the [MIT License](LICENSE).
