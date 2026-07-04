---
name: check-api-sync
description: Use when a public method is added, removed, or its signature changed, before committing API changes, or when asked to audit consistency between the header, cpp, README and keywords.txt.
---

# Check public API sync

## Overview

Every public method must be consistent across **four** files: `src/Countimer.h`
(declaration), `src/Countimer.cpp` (definition), `README.md` (API reference table),
`keywords.txt` (IDE highlighting). Compiling examples does NOT catch a missing
definition unless an example happens to call the method — `isCounterRunning()` shipped
declared-but-undefined exactly this way (see CHANGELOG 1.1.0). This audit does.

## Procedure

1. **Extract the source of truth.** Read the `public:` section of `src/Countimer.h`.
   List every method name (both `setCounter` overloads count as one name) and the
   `CountType` enum values.

2. **Definitions.** For each method name:

   ```sh
   rg "Countimer::<name>\b" src/Countimer.cpp
   ```

   No hit = declared but never defined (link error only when first called). For
   overloads, count hits: 2× `setCounter` declarations need 2 definitions.

3. **keywords.txt.** Each method name present as `KEYWORD2` (tab-separated), the class
   and `CountType` as `KEYWORD1`, `timer_callback` as `KEYWORD2`, enum values
   (`COUNT_NONE`, `COUNT_UP`, `COUNT_DOWN`) as `LITERAL1`.

4. **README API table.** Each method has a row in the "API reference" section and the
   signature text (return type, parameter types, `const`) matches the header exactly.

5. **Reverse direction.** Every `KEYWORD2`/`LITERAL1` entry in `keywords.txt` and every
   row of the README table still exists in the header — catches leftovers after a
   removal or rename.

6. **Report.** One line per method: name + `h / cpp / README / keywords` status.
   List every mismatch with file and what's wrong. All-green = in sync.

## When changing the API

Fix all four files in the same commit, plus a `CHANGELOG.md` entry. Then run the
verify-examples skill — the audit checks presence and signatures, not that the code
compiles.

## Common mistakes

| Mistake | Consequence |
|---|---|
| Trusting example compilation to catch a missing definition | Linker only resolves what is called — audit step 2 exists because of this. |
| Checking only the new method | Renames/removals leave stale entries — always run step 5. |
| Comparing README loosely | `String` vs `char*`, missing `const` — the table must match the header verbatim. |
