#ifndef TMEMORYMANAGER_H
#define TMEMORYMANAGER_H

#include <Arduino.h>
#include <EEPROM.h>

#include "Constants.h"
#include "TErrorManager.h"

class TMemoryManager
{
  public:
    enum class PartitionID : byte
    {
      VERSION     = 0,
      SETTINGS    = 1,
      USER_DATA_1 = 2,
      USER_DATA_2 = 3,
      USER_DATA_3 = 4,
      USER_DATA_4 = 5,
      USER_DATA_5 = 6,
      USER_DATA_6 = 7,
      USER_DATA_7 = 8,
      USER_DATA_8 = 9,
    };

    struct settingsStruct
    {
      // --- Add Settings That Has to be Saved Here
    };

    // --- Add additional memory structs here if needed

    settingsStruct settings;
    settingsStruct defaultSettings; // Can be moved to factory reset as a local variable
    VersionInfo eepromVersion;

    // --- Create objects here

    // ---------------------------------------------

    TMemoryManager(uint16_t EEPROMSize);

    void begin();

    void factoryReset();
    ErrorCode firmwareValidate();

    // Write/Read Data to/from Partitions
    template <typename T>
    void writeData(PartitionID id, const T& data, uint16_t itemIndex = 0) 
    { 
      byte idIndex = static_cast<byte>(id);
      if (idIndex >= MAX_PARTITIONS) errorManager.errorHandler(ErrorCode::ACCESS_VIOLATION);
      if (sizeof(T) > partitionTable[idIndex].size) errorManager.errorHandler(ErrorCode::BUFFER_OVERFLOW);
      EEPROM.put(getAddress(id, itemIndex), data);
    }

    template <typename T>
    void readData(PartitionID id, T& data, uint16_t itemIndex = 0) 
    { 
      byte idIndex = static_cast<byte>(id);
      if (idIndex >= MAX_PARTITIONS) errorManager.errorHandler(ErrorCode::ACCESS_VIOLATION);
      if (sizeof(T) > partitionTable[idIndex].size) errorManager.errorHandler(ErrorCode::BUFFER_OVERFLOW);
      EEPROM.get(getAddress(id, itemIndex), data);
    }

  private:
    // --- These constants holds the required information about partition table data ---
    static constexpr int MAX_PARTITIONS           = 10;
    static constexpr int HEADER_START_ADDRESS     = 0;
    static constexpr int HEADER_RESERVED_SIZE     = 64;
    static constexpr int DEVICE_SIGNATURE_ADRESS  = 60;
    static constexpr int TRANSACTION_FLAG_ADRESS  = 62;
    static constexpr int DATA_START_ADDRESS       = HEADER_START_ADDRESS + HEADER_RESERVED_SIZE;

    static constexpr uint16_t DEVICE_SIGNATURE      = 0x55AA;
    static constexpr uint16_t TRANSACTION_PROGRESS  = 0xBBAA;
    static constexpr uint16_t TRANSACTION_COMMITTED = 0xAABB;

    // --- This Struct holds both standard and custom struct's data ---
    struct PartitionEntry
    {
      uint16_t startAddress;
      uint16_t size;
      uint16_t count;
    };

    uint16_t _EEPROMSize;
    PartitionEntry partitionTable[MAX_PARTITIONS];

    ErrorCode resizePartition(PartitionID id, uint16_t newSize, uint16_t newCount = 1);
    void initPartitionTable();
    uint16_t getAddress(PartitionID id, uint16_t itemIndex = 0);
    void loadPartitionTable();
};

extern TMemoryManager memory;

#endif