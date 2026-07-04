---
name: verify-examples
description: Use when changes to src/ or examples/ need compile verification before committing, when asked to build or verify the library, or when arduino-cli is not installed on this machine ("arduino-cli not found").
---

# Verify examples (compile with arduino-cli)

## Overview

The library has no host-side build or unit tests — verification means compiling every
sketch in `examples/` with `arduino-cli`. Do this locally before every commit that
touches `src/` or `examples/`; never defer to CI what can be checked in seconds here.

## Bootstrap (once per machine)

Check first: `arduino-cli version`. If missing, install (Windows):

```powershell
winget install --id ArduinoSA.CLI --accept-source-agreements --accept-package-agreements
```

The MSI installs to `C:\Program Files\Arduino CLI\arduino-cli.exe` and `winget` does not
refresh the current shell's `PATH` — in the installing session invoke the binary by that
full path; new shells will have it on `PATH`.

Then install the AVR core (once):

```sh
arduino-cli core update-index
arduino-cli core install arduino:avr
```

## Compile all examples

Run from the repository root. `--library .` makes the checkout's `src/` findable without
copying it into the Arduino `libraries/` folder:

```sh
arduino-cli compile --fqbn arduino:avr:uno --library . examples/Basic
arduino-cli compile --fqbn arduino:avr:uno --library . examples/Advanced
arduino-cli compile --fqbn arduino:avr:uno --library . examples/CountimerTest
```

If a new sketch was added under `examples/`, compile it too — and add it to
`sketch-paths` in `.github/workflows/ci.yml` in the same change.

## Success criteria

Every compile exits 0 and prints the memory usage summary (localized — e.g. English
"Sketch uses … bytes" or Polish "Szkic używa … bajtów"); check the exit code, not the
text. Report flash/RAM usage per sketch; a single failure means the change is not
verified — do not commit.

## Common failures

| Symptom | Cause / fix |
|---|---|
| `arduino-cli: command not found` after install | Stale `PATH` in current shell — use the full path from Bootstrap above. |
| `Platform 'arduino:avr' not found` | Run the two `core` commands from Bootstrap. |
| `Countimer.h: No such file or directory` | Not run from repo root, or `--library .` missing. |
| `undefined reference to 'Countimer::…'` | Method declared in `src/Countimer.h` but not defined in `src/Countimer.cpp` — fix the source, see the check-api-sync skill. |

## Notes

- CI additionally compiles for `esp32:esp32:esp32`. Installing the ESP32 core locally
  (~2 GB) is optional; AVR locally + ESP32 in CI is the accepted split.
- Uno is the strictest target (2 KB RAM) — if it compiles there, size regressions are
  unlikely elsewhere.
