# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ShelfClock is an ESP32-based smart LED clock with 7-segment display using WS2812B LEDs. It features multiple display modes (clock, temperature, date, humidity, countdown, scoreboard, music visualizer), web interface configuration, MQTT integration, and Home Assistant integration.

**Hardware**: ESP-WROOM-32, DS3231 RTC, DHT11 sensor, photoresistor, sound detector, WS2812B LEDs (configurable 6-9 per segment)

**Key Libraries**: FastLED, AutoConnect, ArduinoJson, PubSubClient (MQTT), ArduinoHA, NonBlockingRTTTL

## Build and Development Commands

### Build
```bash
pio run
```

### Upload to device
```bash
pio run --target upload
```

### Monitor serial output
```bash
pio run --target monitor
```
Monitor is configured at 112500 baud (see platformio.ini).

### Build and upload
```bash
pio run --target upload && pio run --target monitor
```

### Clean build
```bash
pio run --target clean
```

### Upload filesystem (LittleFS)
```bash
pio run --target uploadfs
```
The `data/` directory contains HTML files for the web interface (index.html, settings.html) that get uploaded to the ESP32's LittleFS filesystem.

## Code Architecture

### Main Application Flow

**[src/ShelfClock.cpp](src/ShelfClock.cpp)** is the main application file containing:
- `setup()`: Initializes hardware, filesystem, WiFi (via AutoConnect), web server, MQTT, Home Assistant, RTC, sensors
- `loop()`: Main state machine that handles different display modes and updates

### Module Organization

The codebase is organized into specialized modules:

- **[src/ShelfClock.cpp](src/ShelfClock.cpp)**: Main application, display routines, all lightshow effects, web server handlers
- **[src/ha_integration.cpp](src/ha_integration.cpp)**: Home Assistant MQTT discovery and state synchronization
- **[src/mqtt_integration.cpp](src/mqtt_integration.cpp)**: MQTT client setup and message handling
- **[src/prefs.cpp](src/prefs.cpp)**: Preferences wrapper providing type-safe NVS storage access
- **[include/ShelfClock.h](include/ShelfClock.h)**: Function declarations for display modes and effects
- **[include/ha_integration.h](include/ha_integration.h)**: HA integration interface with extern declarations for shared state
- **[include/mqtt_integration.h](include/mqtt_integration.h)**: MQTT interface
- **[include/prefs.h](include/prefs.h)**: Preferences API

### LED Addressing System

The project uses a complex LED addressing system to map 7-segment digits to physical LED strips:

- **LEDS_PER_SEGMENT**: Configurable 1-10 LEDs per segment (default 9)
- **Segments per digit**: 7 (standard 7-segment display)
- **Total digits**: 7 (4 real digits for time + 3 "fake" digits for separators/effects)
- **Segment mapping**: Uses preprocessor macros (seg(n), nseg(n)) to map logical segments to physical LED positions
- **Digit definitions**: `digit0`, `fdigit1`, `digit2`, `fdigit3`, `digit4`, `fdigit5`, `digit6` define which segments belong to each digit position

The mapping handles LED direction reversals in the physical wiring. See [src/ShelfClock.cpp:74-130](src/ShelfClock.cpp#L74-L130) for the segment addressing macros.

### Display Modes

The clock operates in different modes controlled by `currentMode` variable:
- **0**: Time display
- **1**: Countdown timer
- **2**: Scoreboard
- **3**: Stopwatch
- **4**: Lightshow (various effects like Chase, Rainbow, Fire, Snake, etc.)
- **5**: Date display
- **6**: Scroll text
- **7**: Realtime mode (music visualizer using sound sensor and FFT)

Each mode has a corresponding `display*Mode()` function in ShelfClock.cpp.

### Settings and Preferences

**NVS (Non-Volatile Storage)** is used extensively for persistent settings:
- Color settings for different display elements (hour, minute, colon, countdown, scoreboard, etc.)
- Display preferences (brightness, color change frequency, display types)
- MQTT configuration
- Schedule data (alarms, recurring events)

**Settings Files**: Settings can be saved/loaded as JSON files in LittleFS under `/settings_*.json` naming pattern, allowing preset configurations. The functions `saveclockSettings(String fileType)` and `getclockSettings(String fileType)` handle this.

**Important**: The project uses a custom partition scheme ([4M-default_nvs.csv](4M-default_nvs.csv)) because standard NVS is too small for the extensive Preferences and AutoConnect data. The custom scheme allocates more NVS space.

### Web Interface

The web interface is served from LittleFS:
- **[data/index.html](data/index.html)**: Main control page
- **[data/settings.html](data/settings.html)**: Settings configuration page

Web handlers are registered in `loadWebPageHandlers()` and process GET/POST requests to read/update clock settings.

### Home Assistant Integration

**[src/ha_integration.cpp](src/ha_integration.cpp)** implements MQTT discovery for Home Assistant:
- Publishes device configuration and available entities
- Provides select entities for display modes, lightshow modes, color settings
- Provides number entities for brightness, scoreboard values
- Provides switch entities for various options
- Syncs state changes bidirectionally (HA ↔ ESP32)

The `haSyncState()` function publishes current state to HA. Device discovery uses the `home-assistant-integration` library.

### MQTT Integration

**[src/mqtt_integration.cpp](src/mqtt_integration.cpp)** handles:
- MQTT connection/reconnection logic
- Command topics for controlling the clock
- State publishing for monitoring

MQTT configuration (broker, port, username, password) is stored in NVS preferences.

### Schedule System

The clock supports complex scheduling using JSON arrays stored in NVS:
- **Schedule types**: Daily, weekly (specific days), hourly, specific date/time
- **Actions**: Play RTTTL songs, change modes, trigger alarms
- **Recurring events**: Support for yearly recurring events (like birthdays)

Schedules are processed in `processSchedules()` which runs periodically to check if any schedule matches current time.

## Important Configuration Notes

### Partition Scheme
The project REQUIRES the custom partition file `4M-default_nvs.csv` specified in platformio.ini:
```ini
board_build.partitions = 4M-default_nvs.csv
```
This allocates sufficient NVS space. Do not change this without understanding the memory layout implications.

### LED Configuration
To change LEDs per segment, modify `LEDS_PER_SEGMENT` in ShelfClock.cpp and ensure the corresponding `#elif` case exists for the seg(n)/nseg(n) macros.

### Serial Monitor Baud Rate
Always use 112500 baud (not the standard 115200) as configured in platformio.ini.

### Filesystem
The project uses LittleFS (not SPIFFS). See the `#if USE_LITTLEFS` block in ShelfClock.cpp. Files in `data/` directory need to be uploaded via `pio run --target uploadfs`.

### Global State
The codebase uses extensive global variables for state management. Most configuration variables are declared as `extern` in header files and defined in ShelfClock.cpp. The ha_integration.h file contains a comprehensive list of shared state variables.

## Key Dependencies

All dependencies are managed via platformio.ini lib_deps. Critical dependencies:
- **FastLED**: LED control and color management
- **AutoConnect**: WiFi configuration portal and web server
- **ArduinoJson**: Settings and schedule management
- **PubSubClient**: MQTT client
- **home-assistant-integration**: HA MQTT discovery
- **RTClib**: DS3231 real-time clock interface
- **DHT sensor library**: Temperature/humidity sensor
- **arduinoFFT**: Music visualizer frequency analysis
- **NonBlockingRTTTL**: Ringtone playback
