# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2026-07-04

### Fixed

- **Counting drift accumulating over long count times.** The counter now advances by the
  real measured `millis()` delta instead of the nominal interval, and the time reference
  point is moved *before* user callbacks run. Previously, both the check overshoot
  (the loop rarely hits the interval boundary exactly) and the callback execution time
  (e.g. `Serial.print` at 9600 baud) were silently lost on every tick, so the error grew
  linearly with the count duration — easily tens of seconds per hour on a busy `loop()`.
  Accuracy is now independent of count length and `loop()` load.
- **Paused time leaking into the count.** `start()` now resets the time reference point
  when resuming, so time spent paused is no longer counted. Calling `start()` on every
  `loop()` iteration (the documented usage pattern) remains safe — the reset only happens
  on an actual stopped-to-running transition.
- **`restart()` while running.** The counter now restarts from a fresh time reference
  point instead of inheriting the previous tick's remainder.
- **`isCounterRunning()` missing implementation.** The method was declared in the header
  and documented in the README, but never defined — calling it failed at link time.
- **Potential crash when `run()` was called without `setInterval()` configured.**
  The interval and completion callback pointers were uninitialized; they now default
  to `NULL` and are guarded before invocation.

### Added

- `setCalibration(float factor)` — optional one-time, per-board correction of hardware
  `millis()` drift (e.g. ceramic resonator tolerance, up to ~0.5% on many Uno/Nano
  clones). Measure `factor = real_elapsed_time / timer_indicated_time` against a
  reference clock; default `1.0` means no correction. Fractional milliseconds are
  carried between ticks, so the correction itself introduces no rounding drift.
- "Accuracy and calibration" section in the README.
- `LICENSE` file (MIT).
- GitHub Actions CI: `arduino-lint` (strict, library-manager mode) plus example
  compilation for `arduino:avr:uno` and `esp32:esp32:esp32`.
- "Installation" and "License" sections in the README.
- `keywords.txt`: `CountType` (KEYWORD1) and `COUNT_NONE`/`COUNT_UP`/`COUNT_DOWN`
  (LITERAL1) for Arduino IDE highlighting.
- `library.properties`: real library description in `paragraph` and the `includes` field.
- `examples/Basic` — minimal count-down sketch for getting started.
- `examples/Advanced` — count-up with pause/resume over serial, state queries,
  re-programming the count time and calibration.
- `CONTRIBUTING.md` — bug report / pull request guidelines, API sync checklist,
  build-and-test instructions and release steps.
- Claude Code project skills in `.claude/skills/`: `verify-examples` (bootstrap
  `arduino-cli` and compile all examples locally), `check-api-sync` (audit public API
  consistency across the header, cpp, README and `keywords.txt`) and `release`
  (release preconditions and tagging procedure).

### Changed

- README rewritten to current library standards: badges (release, license, Library
  Manager), feature list, quick start, per-mode usage snippets, a full API reference
  table matching the header signatures, examples/compatibility/contributing sections.
  The quick-start sketch now calls `start()` once in `setup()` instead of conditionally
  on every `loop()` iteration.
- `keywords.txt` updated with `setInterval` and `setCalibration` for Arduino IDE
  highlighting.
- Non-breaking code cleanups: getters are now `const`, `nullptr` instead of `NULL`,
  `snprintf` instead of `sprintf` in `getCurrentTime()`, simplified
  `getCurrentSeconds()` arithmetic, in-class member initialization (empty constructor
  and destructor removed), single include guard instead of guard + `#pragma once`,
  removed redundant `Arduino.h` include from the `.cpp`.
- Example sketch: includes the library with `<Countimer.h>`, uses `LED_BUILTIN` and
  configures it with `pinMode()` in `setup()`.
- Completion timing semantics: a countdown now completes on the tick where the remaining
  time no longer exceeds the measured delta (up to one interval early), instead of one
  full tick after reaching zero (up to one interval late). Either way the boundary error
  is below one interval and does not accumulate.

## [1.0.0]

Initial release: count-up / count-down / interval-only timer with `start()`, `stop()`,
`pause()`, `restart()` and per-tick plus on-complete callbacks.
