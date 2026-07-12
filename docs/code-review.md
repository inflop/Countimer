# Code review — Countimer

Przegląd obszarów do poprawy w `src/Countimer.h` / `src/Countimer.cpp`, uporządkowany od realnych bugów po kosmetykę. Odnośniki w formacie `plik:linia` wskazują aktualne miejsce w kodzie.

Uwaga: zachowania obecne już w wersji 1.0.0 (reset `_currentCountTime` do punktu startowego w `stop()` po naturalnym zakończeniu, pominięcie `_callback` na ostatnim ticku) uznano za zamierzone i usunięto z tej listy.

## 🔴 Bugi (zachowanie niezgodne z intencją)

### 1. Potencjalne przepełnienie w `countUp()`
`_currentCountTime + elapsed < _countTime` (`src/Countimer.cpp:218`) — dodawanie `uint32_t` może overflownąć przy dużym `elapsed` (np. długa blokada w `loop()`), dając fałszywie `true` i pomijając completion. Bezpieczniej: `_countTime - _currentCountTime > elapsed` (różnica zawsze nieujemna w tym trybie). Wprowadzone przy przepisaniu liczenia na elapsed-based (commit `1d6daaa`) — nie istniało w 1.0.0. Scenariusz czysto teoretyczny (wymaga ~8 dni bez wywołania `run()` przy liczniku bliskim maksimum), ale poprawka jest darmowa.

## 🟡 Design / API

### 2. Ciche obcinanie wartości w `setCounter`
`src/Countimer.cpp:22-36` ucina `hours > 999`, `minutes > 59` itd. bez sygnalizacji. Programista może konfigurować timer 1500 godzin i nigdy nie zauważyć, że dostał 999. Rozważ zwrócić `bool` (lub przynajmniej zostawić w dokumentacji, że jest clamp).

### 3. `setInterval` po `setCounter` nie czyści trybu
Jeśli wywołasz najpierw `setCounter(..., COUNT_DOWN, ...)`, a potem `setInterval(cb, 1000)` chcąc tryb „tylko interwał", `_countType` pozostaje `COUNT_DOWN` i `run()` nadal próbuje liczyć. Brakuje metody typu `clearCounter()` albo parametru w `setInterval` ustawiającego `COUNT_NONE`.

### 4. Brak walidacji interwału
`setInterval(cb, 0)` → `elapsed >= 0` zawsze prawda → callback strzela co iterację `loop()`. Sensowny minimum to kilka ms; warto wymusić ≥ 1 (por. domyślne `_interval = 1`).

### 5. Zwracanie `char*` zamiast `const char*`
`getCurrentTime()` / `getCurrentTimeWithMillis()` zwracają modyfikowalny wskaźnik do wewnętrznego bufora — użytkownik może nadpisać stan obiektu. `const char*` byłoby bezpieczniejsze i pozwoliłoby też oznaczyć metody `const` (dziś nie mogą, bo piszą do bufora — można bufor leniwie przebudowywać albo przyjąć, że `const` + mutable cache).

### 6. Luki w API
Brakuje getterów: `getInterval()`, `getCountType()`, `getStartCountTime()`. Utrudnia introspekcję i pisanie testów.

## 🟢 Czytelność / utrzymanie

### 7. Float na AVR
`_calibration` i `_calibrationRemainder` (`src/Countimer.h:106-109`) to `float`. Na AVR każda operacja z `run()` (linia 181) ciągnie soft-float, a biblioteka ma `architectures=*`, więc AVR jest częstym celem. Wystarczy fixed-point (np. mnożnik `×1000` lub `×65536` i reszta jako int32). Mniejszy kod, deterministyczniejsze zachowanie.

### 8. Magic numbers
`1000`, `3600`, `60` rozsiane po `setCounter` i getterach (linie 38, 64, 67, 69, 74, 79). Stałe `MS_PER_SECOND`, `SEC_PER_MINUTE`, `SEC_PER_HOUR` poprawiłyby czytelność i zmniejszyły ryzyko literówki.

### 9. Czytelność operatorów w getterach
`_currentCountTime / 1000 % 3600 / 60` (`src/Countimer.cpp:69`) opiera się na łączności lewostronnej — działa, ale dodanie nawiasów `((_currentCountTime / 1000) % 3600) / 60` ułatwia czytanie i review.

### 10. Duplikacja „reset do origin"
`stop()` i `reset()` robią bardzo podobną rzecz dwoma ścieżkami (`stop()` używa `_countTime` + wyjątek dla COUNT_UP; `reset()` używa `_startCountTime`). Te ścieżki można ujednolicić — `_startCountTime` jest już poprawne dla obu trybów (dla COUNT_UP ustawiane na 0 w `setCounter`), więc `stop()` mógłby po prostu przypisywać `_startCountTime` bez gałęzi warunkowej.

### 11. Dwa bufory znakowe na instancję
`_formatted_time[10]` + `_formatted_time_ms[13]` = 23 bajty/instancja. Można wspólny bufor `[13]` i przebudowywać w jednym miejscu (uwaga: wskaźniki z obu getterów zaczną wtedy aliasować ten sam bufor — do odnotowania w dokumentacji).

## Testowanie / CI

### 12. Brak testów hostowych i CI
Jak notuje `AGENTS.md` — nie ma żadnego test runnera. `millis()` da się mockować prostą warstwą (`virtual` albo wstrzykiwany wskaźnik na funkcję czasu, ew. makro), więc można zbudować natywne testy (Catch2/Unity) weryfikujące kluczowe ścieżki: completion w obu trybach, overshoot, kalibrację, overflow `millis()`, pauzę/wznowienie. To jednorazowa inwestycja, która trwale zabezpieczy semantykę licznika (m.in. ścieżkę z punktu 1) przed regresjami.

---

Najbardziej polecam zacząć od **12** (testy jako siatka zabezpieczająca), a następnie **1** (darmowa poprawka warunku) i **4** (jednolinijkowy clamp).
