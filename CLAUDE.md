# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Countimer is a single-class Arduino library (`category=Timing`) for non-blocking timers and counters. The entire implementation is `src/Countimer.h` / `src/Countimer.cpp`. There is no host-side build, test runner, or CI — it is consumed as an Arduino IDE library.

## Building / testing

- No standalone build. Verify changes by compiling an example sketch with the Arduino toolchain, e.g.:
  `arduino-cli compile --fqbn arduino:avr:uno examples/CountimerTest/CountimerTest.ino`
  (requires the library be findable — run from a checkout placed under the Arduino `libraries/` folder, or use `--library .`).
- To exercise behavior, upload `examples/CountimerTest/CountimerTest.ino` to a board and drive it over the serial monitor (keys `S`/`P`/`R`/`T` start/pause/restart/stop all three demo timers).
- `keywords.txt` provides Arduino IDE editor highlighting; keep it in sync when adding/removing public methods.

## Architecture

Everything is driven by a single cooperative, non-blocking loop — there is no threading or interrupts.

- **`run()` is the heartbeat.** The user must call `run()` on every `loop()` iteration. It compares `millis() - _previousMillis` against `_interval` and only acts once an interval has elapsed. It is a no-op while `_isCounterCompleted` or `_isStopped` is true — so `start()` must be called to make a configured timer actually advance.
- **Counting is elapsed-time based, not tick based.** Each tick advances the counter by the real measured `millis()` delta (scaled by `_calibration`, with a fractional-ms remainder carried in `_calibrationRemainder`), and `_previousMillis` is moved *before* callbacks run — so neither check overshoot nor callback duration accumulates as drift. Don't "simplify" this back to decrementing by `_interval`; that was the original drift bug. `setCalibration(float)` compensates residual hardware `millis()` drift (default 1.0).
- **Mode is selected by `_countType`** (`COUNT_NONE` / `COUNT_UP` / `COUNT_DOWN`, set via `setCounter`). `run()` dispatches to `countDown(elapsed)`, `countUp(elapsed)`, or a bare `callback()`:
  - `COUNT_DOWN`: `_currentCountTime` decrements by the elapsed delta until it runs out, then `stop()` + `complete()`.
  - `COUNT_UP`: starts at 0, increments toward `_countTime`, then `stop()` + `complete()`.
  - `COUNT_NONE`: no counter — just fires `_callback` every `_interval` (an interval-only timer). Set up with `setInterval()` alone, no `setCounter()`.
- **Two callbacks, different roles:** `_callback` (via `setInterval`) fires each tick; `_onComplete` (the 5th arg to `setCounter`) fires once when the count finishes. Both are guarded against NULL.
- **Time is stored in milliseconds** (`_currentCountTime`, `_countTime`, `_startCountTime`). `getCurrentHours/Minutes/Seconds()` derive H/M/S on demand; `getCurrentTime()` formats into the fixed `_formatted_time[10]` buffer as `HH:MM:SS` and returns that pointer (not a copy).
- **State semantics:** `pause()` only sets `_isStopped` (resumable via `start()`). `stop()` marks completed and resets `_currentCountTime` to its origin. `restart()` resets to `_startCountTime` and starts. Inputs to `setCounter` are clamped to `COUNTIMER_MAX_HOURS` (999) and `COUNTIMER_MAX_MINUTES_SECONDS` (59).
- **`start()` resets `_previousMillis` only when transitioning from stopped to running** — this both excludes paused time from the count and keeps the documented pattern of calling `start()` on every `loop()` iteration harmless. `restart()` forces this reset by setting `_isStopped = true` before delegating to `start()`.

## Notes

- The public API is consistent across `src/Countimer.h` (declarations), `src/Countimer.cpp` (definitions), `README.md`, and `keywords.txt`. When adding/removing a public method, update all four so they stay in sync.
- `version` lives in both `library.properties` and the `1.0.0` git tag; bump both together on release.
