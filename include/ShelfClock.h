#pragma once
#include "Arduino.h"

// ShelfClock.h - Only declarations for functions that remain in ShelfClock.cpp
// Most function declarations have moved to their respective module headers:
//   display_modes.h, lightshow.h, settings_manager.h, web_handlers.h

extern byte mac[];

// Applies the configured timezone: POSIX TZ string with automatic DST when
// timeZone is set, legacy manual GMT offset + DST flag otherwise
void applyTimeConfig();
