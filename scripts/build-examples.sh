#!/usr/bin/env bash
# Compile examples/ sketches with arduino-cli.
# Usage: scripts/build-examples.sh [--fqbn <fqbn>] [example ...]
#   No example names -> builds every sketch under examples/.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EXAMPLES_DIR="$REPO_ROOT/examples"
FQBN="arduino:avr:uno"

args=()
while [ "$#" -gt 0 ]; do
    case "$1" in
        --fqbn)
            FQBN="$2"
            shift 2
            ;;
        *)
            args+=("$1")
            shift
            ;;
    esac
done

if ! command -v arduino-cli >/dev/null 2>&1; then
    # Try common install locations before giving up.
    for candidate in \
        "$HOME/bin/arduino-cli" \
        "$HOME/.local/bin/arduino-cli" \
        "/usr/local/bin/arduino-cli"
    do
        if [ -x "$candidate" ]; then
            export PATH="$(dirname "$candidate"):$PATH"
            break
        fi
    done
fi

if ! command -v arduino-cli >/dev/null 2>&1; then
    echo "error: arduino-cli not found on PATH (see CONTRIBUTING.md for install steps)" >&2
    exit 1
fi

if [ "${#args[@]}" -gt 0 ]; then
    examples=("${args[@]}")
else
    examples=()
    for dir in "$EXAMPLES_DIR"/*/; do
        examples+=("$(basename "$dir")")
    done
fi

status=0
for name in "${examples[@]}"; do
    sketch="$EXAMPLES_DIR/$name"
    if [ ! -d "$sketch" ]; then
        echo "error: no such example: $name" >&2
        status=1
        continue
    fi
    echo "==> Building $name ($FQBN)"
    if ! arduino-cli compile --fqbn "$FQBN" --library "$REPO_ROOT" --output-dir "$sketch/build" "$sketch"; then
        status=1
    fi
done

exit "$status"
