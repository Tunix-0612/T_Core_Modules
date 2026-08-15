#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// --- PINOUT ---
namespace pins 
{
  
}

// --- DEVICE RELATED DATA ---
namespace characteristics 
{
  
}

// --- SOFTWARE VERSION ---
struct VersionInfo 
{
  byte major;
  byte minor;
  byte patch;

  bool operator<(const VersionInfo& other) const 
  {
    if (major != other.major) return major < other.major;
    if (minor != other.minor) return minor < other.minor;
    return patch < other.patch;
  }

  bool operator==(const VersionInfo& other) const 
  {
    return major == other.major && minor == other.minor && patch == other.patch;
  }
};

namespace system_version 
{
  constexpr VersionInfo FIRMWARE = {1, 0, 0};
  constexpr VersionInfo MIN_SUPPORTED = {0, 9, 0};
}

// --- ERROR CODES ---
enum class ErrorCode : uint16_t 
{
  NONE                    =  0,
  FIRMWARE_VERSION_ERROR  = 101,
  VERSION_NOT_SUPPORTED   = 102,
  DOWNGRADED_FIRMWARE     = 103,

  PROGRAM_LOOP_FAILURE    = 111,

  RAM_FAILURE             = 121,
  RAM_LOW                 = 122,

  EEPROM_FULL             = 131,
  ACCESS_VIOLATION        = 132,
  BUFFER_OVERFLOW         = 133,
  INTEGRITY_ERROR         = 134

  // --- Additional Error Codes
};

#endif