# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ShelfClock is an ESP32-based smart LED shelf clock with a 7-segment display built from WS2812B LEDs. It features multiple display modes (clock, date, countdown, scoreboard, stopwatch, lightshows, scrolling text, party games, HA value display), a web interface with live LED preview, MQTT integration and Home Assistant integration (MQTT discovery).

**Hardware (this build)**: ESP-WROOM-32, WS2812B LEDs on **GPIO 15** (`LED_PIN` in globals.h - the upstream default is 4, do not revert!), **9 LEDs per segment** (the original instructions use 7; the wiring order/topology is identical, only the count differs). 37 segments + 14 spotlights (2 per digit column, wired zig-zag from the left) = 347 LEDs total.

**Removed from this fork**: DHT sensor, DS3231 RTC (time comes from NTP only), FFT/music visualizer, RTTTL buzzer hardware. Their libraries were dropped from platformio.ini.

**Key Libraries**: FastLED, AutoConnect, ArduinoJson, PubSubClient (MQTT), home-assistant-integration (ArduinoHA), NonBlockingRTTTL

## Build and Development Commands

PlatformIO CLI on this machine: `%USERPROFILE%\.platformio\penv\Scripts\pio.exe`

```bash
pio run                    # build firmware -> .pio/build/espwroom32/firmware.bin
pio run --target buildfs   # build filesystem image -> .pio/build/espwroom32/littlefs.bin
pio run --target upload    # flash firmware via USB
pio run --target uploadfs  # flash filesystem via USB
pio run --target monitor   # serial monitor (112500 baud, NOT 115200!)
```

**OTA updates** (preferred by the user): upload `firmware.bin` at `http://shelfclock/update` and `littlefs.bin` at `http://shelfclock/updatefs` (both linked in the web UI nav). During an OTA upload the display intentionally goes dark (`otaInProgress` flag stops `loop()` from driving LEDs/filesystem).

## Code Architecture

### Module Organization

- **[src/ShelfClock.cpp](src/ShelfClock.cpp)**: `setup()`/`loop()`, LED addressing tables, character font, timezone handling (`applyTimeConfig()`), boot sequence
- **[src/display_modes.cpp](src/display_modes.cpp)**: all `display*Mode()` functions, `scroll()`, `displayNumber()`, `ShelfDownLights()` (spotlights), colon/dots rendering
- **[src/lightshow.cpp](src/lightshow.cpp)**: ~22 lightshow effects (Chase, Twinkles, Rainbow, Matrix, Rain, Fire, Snake, Cylon, Breathing, Sparkle, Meteor, ColorWipe, Plasma, Confetti, Juggle, Heartbeat, pixel-level effects...)
- **[src/party_games.cpp](src/party_games.cpp)**: party games (0=Dice, 1=Roulette, 2=Shot Timer, 3=Higher/Lower)
- **[src/settings_manager.cpp](src/settings_manager.cpp)**: settings JSON save/load incl. NVS backup, file helpers, schedule processing
- **[src/web_handlers.cpp](src/web_handlers.cpp)**: all web endpoints (`loadWebPageHandlers()`), incl. `/getleds` (live preview data), `/updateanything` (generic settings updates), OTA handlers
- **[src/ha_integration.cpp](src/ha_integration.cpp)**: Home Assistant MQTT discovery entities and state sync (`haSyncState()`, throttled to 1s/dirty-flag)
- **[src/mqtt_integration.cpp](src/mqtt_integration.cpp)**: plain MQTT client setup/reconnect
- **[src/prefs.cpp](src/prefs.cpp)**: type-safe Preferences (NVS) wrapper
- **[include/globals.h](include/globals.h)**: LED config constants and extern declarations for all shared globals (defined in ShelfClock.cpp)

### Display Modes (`clockMode`)

- **0**: Time, **1**: Countdown, **2**: Party games, **3**: Scoreboard, **4**: Stopwatch, **5**: Lightshow, **7**: Date, **8**: HA value display (number sent from Home Assistant), **10**: Display off, **11**: Scroll text

Modes get CPU time in the 500ms tick in `loop()`; `displayRealtimeMode()` runs every loop pass for fast lightshow effects.

### LED Addressing System

Segment `n` (0-36) occupies LEDs `n*LEDS_PER_SEGMENT ... +LEDS_PER_SEGMENT-1`. Wiring starts at digit 0 (rightmost): seg 0 = its lower-right vertical, then serpentine per digit. Segment roles within a digit (base = digit*5): +0 lower-right, +1 bottom, +2 lower-left, +3 upper-left, +4 top, +5 upper-right, +6 middle. Real digits are 0/2/4/6 (right to left), fake digits 1/3/5 share their verticals with the neighbouring real digits and only own top/middle/bottom. LED direction per role: top = left-to-right, middle/bottom = right-to-left, left verticals = bottom-to-top, right verticals = top-to-bottom. The 14 spotlights follow after the segment LEDs, zig-zag from the LEFT (col6 upper,lower / col5 lower,upper / alternating). The `seg(n)`/`nseg(n)` macros and `FAKE_LEDs_C_*` tables in ShelfClock.cpp map effect layouts onto this.

The web live preview ([data/js/preview.js](data/js/preview.js)) mirrors this exact topology and auto-detects LEDs-per-segment from the `/getleds` payload length.

### Settings and Persistence

Three layers:
1. **LittleFS JSON** (`/settings/clockSettings-generic.json`): main settings file, saved by `saveclockSettings("generic")`, auto-saved 3s after `updateSettingsRequired = 1`
2. **NVS backup** (`settingsBak` key): written on every save, restored on boot if it differs from the file - this is what makes settings survive filesystem updates. NVS also holds MQTT credentials (never in the repo!) and WiFi credentials (managed by AutoConnect)
3. **Named presets**: `/settings/clockSettings-<name>.json`

**Timezone**: `timeZone` (POSIX TZ string, default `CET-1CEST,M3.5.0,M10.5.0/3` = Berlin with automatic DST) applied via `applyTimeConfig()`. Empty string = legacy manual mode (`gmtOffset_sec` + `DSTime`). Setting a manual offset (web or HA) clears `timeZone`.

### Web Interface (data/ -> LittleFS)

- **[data/index.html](data/index.html)**: home page - live LED preview canvas, mode buttons, spotlight quick controls, timer/scoreboard/party/lightshow controls
- **[data/settings.html](data/settings.html)**: full settings incl. timezone dropdown and MQTT config (password is masked server-side)
- **[data/js/api.js](data/js/api.js)**: fetch wrapper, **[data/js/settings-store.js](data/js/settings-store.js)**: settings cache/update logic, **[data/js/main.js](data/js/main.js)**: page wiring, **[data/js/preview.js](data/js/preview.js)**: live preview renderer (applies FastLED TypicalLEDStrip correction + gamma so colors match the real strip)
- No frameworks - vanilla JS/CSS (Bootstrap/jQuery were removed)

Notable endpoints: `/getleds` (hex LED state for preview, polled every 500ms), `/updateanything` (POST JSON, generic setting updates), `/getsettings`, `/exportsettings`/`/importsettings` (full backup incl. MQTT config), `/formatfs` (POST with confirm=yes only), `/debuglog`, `/getdebug` (includes `resetReason` - "Brownout" means power problem, the hardware has had flaky-connector issues).

## Important Configuration Notes

- **Partition scheme**: `4M-default_nvs.csv` is REQUIRED (spiffs partition 0xd0000 must match littlefs.bin size). Do not change.
- **LED_PIN 15** - this build's hardware; the refactor once silently reverted it to 4 and the display went dark.
- **Serial baud 112500** (not 115200), matches platformio.ini.
- **LittleFS** (not SPIFFS); `data/` uploads via uploadfs or `/updatefs`.
- Extensive global state: most config vars are defined in ShelfClock.cpp and declared extern in globals.h.
- `git push` goes to the user's fork `Mastershort/ShelfClock_Mastershort`; `upstream` is the original MacGyverr/ShelfClock - never push there.

## Known Issues / Open Work

- Scrolling is a non-blocking state machine: `startScroll()`/`startScrollColored()` queue text, `scrollTick()` (every `loop()` pass) renders one column per 250ms while `scrollActive` suppresses normal mode rendering. The blocking `scroll()` wrapper exists only for `setup()`.
- `/updateanything` is a ~400-line if-chain, could be table-driven
- `checkSleepTimer()` is empty (suspend/sleep feature unfinished)
- Planned features: HA-Notify (MQTT messages scrolled on the clock), sunrise alarm, birthday confetti via scheduler
