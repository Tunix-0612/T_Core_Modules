#include "TErrorManager.h"

#include "TMemoryManager.h"

TErrorManager::TErrorManager() { }

// --- Add Additional Error Handler Functions Here

void TErrorManager::errorHandler(ErrorCode code) // --- Standart Error Handlers
{
  byte numericCode = static_cast<byte>(code);
  char errorCodeStr[4]; // Can be used to display in screens etc.
  itoa(numericCode, errorCodeStr, 10);

  switch(code) 
  {
    case ErrorCode::NONE: break; // Continue normally

    case ErrorCode::FIRMWARE_VERSION_ERROR:

      memory.factoryReset(); // Recommended
      break;

    case ErrorCode::VERSION_NOT_SUPPORTED:
      
      memory.factoryReset(); // Recommended
      break;

    case ErrorCode::DOWNGRADED_FIRMWARE:

      // Recommend Factory Reset
      break;

    case ErrorCode::PROGRAM_LOOP_FAILURE:

      while(true); // Force-Reset Recommended. You can use WDT watchdog if your chip supports it
      break;

    case ErrorCode::RAM_FAILURE:

      while(true); // Faulty Micro Controller
      break;

    case ErrorCode::RAM_LOW:

      while(true); // Low RAM Amount
      break;

    case ErrorCode::EEPROM_FULL:

      while(true);
      break;

    case ErrorCode::ACCESS_VIOLATION:

      while(true); // Software related issue, halt device and patch the update
      break;

    case ErrorCode::BUFFER_OVERFLOW:

      while(true); // Software related issue, halt device and patch the update
      break;

    case ErrorCode::INTEGRITY_ERROR:

      while(true); // Broken Partition Table, force factory reset
      break;

    default:

      while(true); // Unknown Error
      break;
  }

  return;
}
