#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// --- PINOUT ---
namespace pins 
{
  
}

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
enum class ErrorCode : byte 
{
  NONE                   =  0,
  FIRMWARE_VERSION_ERROR = 11,
  VERSION_NOT_SUPPORTED  = 12,
  DOWNGRADED_FIRMWARE    = 13,
  PROGRAM_LOOP_FAILURE   = 14,
  RAM_FAILURE            = 15,

  // --- Additional Error Codes
};

#endif