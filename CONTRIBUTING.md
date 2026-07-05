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

Or use the bundled scripts, which do the same for every sketch under `examples/`
(pass a sketch name to build just one, `--fqbn`/`-Fqbn` to target another board):

```sh
./scripts/build-examples.sh          # Linux/macOS
./scripts/build-examples.ps1         # Windows
```

CI runs `arduino-lint` (strict, library-manager mode) and compiles all examples for
`arduino:avr:uno` and `esp32:esp32:esp32` on every push and pull request — make sure
it passes.

To exercise behavior on real hardware, upload an example and drive it over the serial
monitor — `examples/CountimerTest` uses keys `S`/`P`/`R`/`T` to start/pause/restart/stop
its three demo timers; see the header comment in each `.ino` for its own keys.

### Running examples in the Wokwi simulator

[Wokwi](https://wokwi.com/) is a browser/VS Code simulator for embedded hardware. It runs
the library's actual compiled firmware (the `.hex`/`.elf` from `arduino-cli`) against a
virtual Arduino Uno — including a virtual serial monitor — so you can exercise an example's
real runtime behavior without a physical board. Every sketch under `examples/` (`Basic`,
`Advanced`, `CountimerTest`) has a `wokwi.toml` + `diagram.json` pair set up for this.

1. Install the **Wokwi Simulator** extension in VS Code (from the Marketplace). The
   extension requires an activated license before it will run a simulation — a free
   **Community License** is enough, no paid plan needed.
2. One-time setup: run `Wokwi: Request a New License` from the Command Palette
   (`Ctrl+Shift+P`) and confirm via the link in the email it sends — the diagram editor
   then shows a "Community License" badge instead of blocking simulation.
3. Build the firmware for the example you want to simulate — Wokwi loads a prebuilt
   `.hex`, it does not compile the sketch itself:

   ```sh
   ./scripts/build-examples.sh CountimerTest    # Linux/macOS
   .\scripts\build-examples.ps1 CountimerTest   # Windows
   ```

4. Open `examples/<name>/diagram.json` in VS Code.
5. Click the ▶ **Start Simulation** button in the diagram editor's toolbar (top-right of
   the tab), or run `Wokwi: Start Simulator` from the Command Palette.
6. Interact through the virtual serial monitor that opens alongside the simulation —
   type a key (e.g. `S`) and click *Send*, same as with a real board.
7. After editing `src/` or the sketch, repeat step 3 before restarting the simulation —
   otherwise Wokwi keeps running the previously built firmware.

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
