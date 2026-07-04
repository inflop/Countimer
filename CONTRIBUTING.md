# Contributing to Countimer

Thanks for taking the time to contribute! Countimer is a small, dependency-free
Arduino library, so the process is deliberately lightweight.

## Reporting bugs

Open an [issue](https://github.com/inflop/Countimer/issues) and include:

- the board and core you are using (e.g. Uno / `arduino:avr`, ESP32 / `esp32:esp32`),
- the library version (see `library.properties` or the Library Manager),
- a **minimal sketch** that reproduces the problem — ideally a stripped-down variant
  of one of the sketches in [`examples/`](examples/),
- what you expected to happen and what actually happened.

Timing issues are the most common category. Before reporting drift, please check the
["Accuracy and calibration"](README.md#accuracy-and-calibration) section of the README —
hardware clock drift (ceramic resonator boards) is expected and can be compensated
with `setCalibration()`.

## Suggesting features

Open an issue first and describe the use case. Keep in mind the library's scope:
a single, cooperative, non-blocking timer class driven by `millis()` from `loop()`.
Features requiring interrupts, threading, or external dependencies are out of scope.

## Pull requests

1. Fork the repository and create your branch from **`dev`** (pull requests are merged
   into `dev`, not `master`).
2. Make your changes. The entire implementation lives in
   `src/Countimer.h` / `src/Countimer.cpp`.
3. Verify that the examples still compile (see below).
4. Open the pull request against `dev` with a short description of *why* the change
   is needed.

### Keep the public API in sync

When you add, remove, or change a public method, update **all four** places in the
same pull request:

- `src/Countimer.h` — declaration,
- `src/Countimer.cpp` — definition,
- `README.md` — the API reference table (and usage snippets if relevant),
- `keywords.txt` — Arduino IDE highlighting.

Also add an entry to `CHANGELOG.md` (the format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/)).

### Building and testing

There is no host-side build or unit test runner — the library is verified by
compiling the example sketches:

```sh
arduino-cli compile --fqbn arduino:avr:uno --library . examples/Basic
arduino-cli compile --fqbn arduino:avr:uno --library . examples/Advanced
arduino-cli compile --fqbn arduino:avr:uno --library . examples/CountimerTest
```

CI runs `arduino-lint` (strict, library-manager mode) and compiles all examples for
`arduino:avr:uno` and `esp32:esp32:esp32` on every push and pull request — make sure
it passes.

To exercise behavior on real hardware, upload `examples/CountimerTest` and drive the
three demo timers over the serial monitor (`S`/`P`/`R`/`T`).

### Claude Code skills

If you work on this library with [Claude Code](https://claude.com/claude-code), the
repository ships project skills in `.claude/skills/` that automate the procedures
described in this document:

- **`/verify-examples`** — bootstraps `arduino-cli` if missing and compiles every
  sketch in `examples/` (the "Building and testing" steps above).
- **`/check-api-sync`** — audits that every public method is consistent across
  `Countimer.h`, `Countimer.cpp`, the README API table and `keywords.txt`
  (the "Keep the public API in sync" checklist above).
- **`/release`** — walks the release preconditions and tagging steps
  (the "Releasing" section below).

The skills automate this document, not replace it — the manual steps remain the
source of truth for contributors not using Claude Code.

### Code style

- Match the existing style: tabs for indentation, `_underscorePrefix` for private
  members, in-class member initialization.
- Be careful around the timing logic in `run()` — counting is based on the real
  measured `millis()` delta, and the time reference point is moved *before* callbacks
  run. Do not "simplify" it back to decrementing by the nominal interval; that was
  the original drift bug (see `CHANGELOG.md` 1.1.0).
- Guard any new callback invocation against `nullptr`.

## Releasing (maintainers)

Bump `version` in `library.properties`, update `CHANGELOG.md`, and create a matching
git tag — the version lives in both places and they must stay in sync.

## License

By contributing, you agree that your contributions will be licensed under the
[MIT License](LICENSE).
