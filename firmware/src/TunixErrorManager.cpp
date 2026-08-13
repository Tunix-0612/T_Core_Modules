#include "TunixErrorManager.h"

#include "TunixMemoryManager.h"

TunixErrorManager::TunixErrorManager() { }

// --- Add Additional Error Handler Functions Here

void TunixErrorManager::errorHandler(ErrorCode code) // --- Standart Error Handlers
{
  byte numericCode = static_cast<byte>(code);
  char errorCodeStr[4]; // Can be used to display in screens etc.
  itoa(numericCode, errorCodeStr, 10);

  switch(code) 
  {
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

      while(true); // Force-Reset Recommended
      break;

    case ErrorCode::RAM_FAILURE:

      while(true); // Faulty Micro Controller
      break;

    default:

      while(true); // Unknown Error
      break;
  }

  return;
}
