#include "TunixMemoryManager.h"

#include <EEPROM.h>

#include "Constants.h"

TunixMemoryManager::TunixMemoryManager(int EEPROMSize)
{
  _EEPROMSize = EEPROMSize - 1;

  // --- Assign Default values here - OR - in the factory reset.
}

void TunixMemoryManager::getBasicMemory()
{
  EEPROM.get(VERSION_ADRESS, eepromVersion);
  EEPROM.get(SETTINGS_ADRESS, settings);
  // --- Add Additional Device Memory Recalls Here
  return;
}

void TunixMemoryManager::saveBasicMemory()
{
  EEPROM.put(SETTINGS_ADRESS, settings);
  // --- Add Additional Device Memory Saves Here
  return;
}

void TunixMemoryManager::factoryReset()
{
  // --- Assign Default Values Here

  settings = defaultSettings;
  EEPROM.put(SETTINGS_ADRESS, settings);

  eepromVersion = system_version::FIRMWARE;
  EEPROM.put(VERSION_ADRESS, eepromVersion);

  delay(500);
  while(true);
}

// --- Validate the firmware versions
ErrorCode TunixMemoryManager::firmwareValidate() 
{
  // If no version difference, return no error
  if (eepromVersion == system_version::FIRMWARE) return ErrorCode::NONE;

  // If the firmware version is greater than the EEPROM version, it means the firmware has been updated.
  if (eepromVersion < system_version::FIRMWARE)
  {
    // If the EEPROM version is less than the minimum supported version, return an error.
    if (eepromVersion < system_version::MIN_SUPPORTED) return ErrorCode::VERSION_NOT_SUPPORTED;

    delay(2000);

    if (eepromVersion == VersionInfo{1, 4, 2}) // --- Version Specific Update/Fix Example

    // Save the new firmware version to EEPROM
    eepromVersion = system_version::FIRMWARE;
    EEPROM.put(VERSION_ADRESS, eepromVersion);

    EEPROM.put(SETTINGS_ADRESS, settings);
    
    while (true); // Lock the Device.
  }

  // If device is downgraded return the error/warning
  return ErrorCode::DOWNGRADED_FIRMWARE;
}
