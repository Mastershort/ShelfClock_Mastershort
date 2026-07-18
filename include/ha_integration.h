// ha_integration.h
#ifndef HA_INTEGRATION_H
#define HA_INTEGRATION_H

#include "globals.h"
#include "ShelfClock.h"

// --- HA-specific functions ---
void setupHA();
void setDisplayMode(String mode);
void haSyncState();
void haMarkDirty();

#endif // HA_INTEGRATION_H
