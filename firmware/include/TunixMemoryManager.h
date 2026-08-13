#ifndef TUNIXMEMORYMANAGER_H
#define TUNIXMEMORYMANAGER_H

#include <Arduino.h>
#include "Constants.h"

class TunixMemoryManager
{
  private:
    int _EEPROMSize;

  public:
    struct settingsStruct
    {
      // --- Add Settings That Has to be Saved Here
    };

    // --- Add additional memory structs here if needed

    settingsStruct settings;
    settingsStruct defaultSettings;
    VersionInfo eepromVersion;

    // --- Create objects here

    TunixMemoryManager(int EEPROMSize);

    void getBasicMemory();
    void saveBasicMemory();
    void factoryReset();
    ErrorCode firmwareValidate();
};

// --- EEPROM MEMORY ADRESSES ---
constexpr int VERSION_ADRESS     = 0;
constexpr int SETTINGS_ADRESS    = VERSION_ADRESS + sizeof(VersionInfo);

extern TunixMemoryManager memory;

#endif