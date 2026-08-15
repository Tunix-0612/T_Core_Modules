#include "TMemoryManager.h"

#include "TErrorManager.h"

TMemoryManager::TMemoryManager(uint16_t EEPROMSize)
{
  _EEPROMSize = EEPROMSize;

  // --- Assign Default values here - OR - in the factory reset with local defaultSettings Struct.
}

void TMemoryManager::begin()
{
  // Load / Init partition table.
  loadPartitionTable();

  readData(PartitionID::VERSION, eepromVersion);
  readData(PartitionID::SETTINGS, settings);
}

ErrorCode TMemoryManager::resizePartition(PartitionID id, uint16_t newSize, uint16_t newCount)
{
  // Get the ID.
  byte index = static_cast<byte>(id);

  // If ID exceedes the maximimum return ACCESS_VIOLATION.
  if (index >= MAX_PARTITIONS) return ErrorCode::ACCESS_VIOLATION;

  // In case of a system shutdown while Partitioning.
  EEPROM.put(TRANSACTION_FLAG_ADRESS, TRANSACTION_PROGRESS);

  uint16_t oldTotalSize = partitionTable[index].size * partitionTable[index].count;
  uint16_t newTotalSize = newSize * newCount;

  if (oldTotalSize == newTotalSize) // If no change in size return.
  {
    partitionTable[index].size = newSize;
    partitionTable[index].count = newCount;
    EEPROM.put(HEADER_START_ADDRESS, partitionTable);
    EEPROM.put(TRANSACTION_FLAG_ADRESS, TRANSACTION_COMMITTED); // Mark as Transaction finished succseffuly.
    return ErrorCode::NONE;
  }

  // If partition is a new partition.
  if (partitionTable[index].startAddress == 0 && newTotalSize > 0)
  {
    if (index == 0) partitionTable[index].startAddress = DATA_START_ADDRESS;
    else
    {
      // Find the last active Partition's End Address
      for (int i = index - 1; i >= 0; i--)
      {
        if (partitionTable[i].size > 0 && partitionTable[i].count > 0)
        {
          partitionTable[index].startAddress = partitionTable[i].startAddress + (partitionTable[i].size * partitionTable[i].count);
          break;
        }
      }

      // If no active Partitions before, start from DATA_START_ADDRESS
      if (partitionTable[index].startAddress == 0) partitionTable[index].startAddress = DATA_START_ADDRESS;
    }
  }

  int shiftAmount = static_cast<int>(newTotalSize) - static_cast<int>(oldTotalSize);
  byte nextIndex = index + 1;

  // Calculate the addresses of data that has to be migrated
  uint16_t moveStartAddress = 0;
  uint16_t moveEndAddress = 0;
  bool hasSubsequentData = false;

  for (byte i = nextIndex; i < MAX_PARTITIONS; i++)
  {
    if (partitionTable[i].size > 0 && partitionTable[i].count > 0)
    {
      if (!hasSubsequentData)
      {
        moveStartAddress = partitionTable[i].startAddress;
        hasSubsequentData = true;
      }
      moveEndAddress = partitionTable[i].startAddress + (partitionTable[i].size * partitionTable[i].count);
    }
  }

  // If no data has to migrated update Partition table and return
  if (!hasSubsequentData)
  {
    // EEPROM Limit check
    if (partitionTable[index].startAddress + newTotalSize > _EEPROMSize) return ErrorCode::EEPROM_FULL;

    // If Partition is deleted by 0 as size, delete the start address as well
    if (newTotalSize == 0) partitionTable[index].startAddress = 0;
    partitionTable[index].size = newSize;
    partitionTable[index].count = newCount;
    EEPROM.put(HEADER_START_ADDRESS, partitionTable);
    EEPROM.put(TRANSACTION_FLAG_ADRESS, TRANSACTION_COMMITTED); // Mark as Transaction finished succseffuly.
    return ErrorCode::NONE;
  }

  // EEPROM Limit check
  if (shiftAmount > 0 && (moveEndAddress + shiftAmount) > _EEPROMSize) return ErrorCode::EEPROM_FULL;

  // Shift data on the EEPROM
  if (shiftAmount > 0) // Shift forward (From End to Start)
  {
    for (int32_t i = moveEndAddress - 1; i >= moveStartAddress; i--)
    {
      byte data = EEPROM.read(i);
      EEPROM.update(i + shiftAmount, data);
    }
  }
  else // Shift backwards (From Start to End)
  {
    for (uint32_t i = moveStartAddress; i < moveEndAddress; i++)
    {
      byte data = EEPROM.read(i);
      EEPROM.update(i + shiftAmount, data);
    }
  }

  // Update Partitions' start addresses
  for (byte i = nextIndex; i < MAX_PARTITIONS; i++) 
    if (partitionTable[i].size > 0 && partitionTable[i].count > 0) partitionTable[i].startAddress += shiftAmount;

  // Update Target Partition's entry
  partitionTable[index].size = newSize;
  partitionTable[index].count = newCount;

  // If Partition is deleted by 0 as size, delete the start address as well
  if (newTotalSize == 0) partitionTable[index].startAddress = 0;

  EEPROM.put(HEADER_START_ADDRESS, partitionTable);

  EEPROM.put(TRANSACTION_FLAG_ADRESS, TRANSACTION_COMMITTED); // Mark as Transaction finished succseffuly.
  return ErrorCode::NONE;
}

void TMemoryManager::initPartitionTable()
{
  EEPROM.put(TRANSACTION_FLAG_ADRESS, TRANSACTION_PROGRESS);
  memset(partitionTable, 0, sizeof(partitionTable));
  
  // YOU CAN allocate more bytes to any partition including version and settings.
  // TPT (Tunix Partition Table) allows user to write/read data smaller than the partition size.
  // If you try to write +20 bytes to a 20 byte partition it will CALL "BUFFER_OVERFLOW" Error.

  // --- You can Allocate more memory than needed here.
  partitionTable[static_cast<byte>(PartitionID::VERSION)].startAddress = DATA_START_ADDRESS;
  partitionTable[static_cast<byte>(PartitionID::VERSION)].size = sizeof(VersionInfo);
  partitionTable[static_cast<byte>(PartitionID::VERSION)].count = 1;

  uint16_t settingsStart = DATA_START_ADDRESS + sizeof(VersionInfo);

  partitionTable[static_cast<byte>(PartitionID::SETTINGS)].startAddress = settingsStart;
  partitionTable[static_cast<byte>(PartitionID::SETTINGS)].size = sizeof(settingsStruct);
  partitionTable[static_cast<byte>(PartitionID::SETTINGS)].count = 1;

  // --- Custom structs' partitioning should be here
  EEPROM.put(HEADER_START_ADDRESS, partitionTable);

  EEPROM.put(HEADER_START_ADDRESS, partitionTable);
  EEPROM.put(DEVICE_SIGNATURE_ADRESS, DEVICE_SIGNATURE);
  EEPROM.put(TRANSACTION_FLAG_ADRESS, TRANSACTION_COMMITTED);
}

uint16_t TMemoryManager::getAddress(PartitionID id, uint16_t itemIndex) 
{
    byte idIndex = static_cast<byte>(id);
    PartitionEntry& entry = partitionTable[idIndex];

    if (itemIndex >= entry.count) errorManager.errorHandler(ErrorCode::ACCESS_VIOLATION);

    return entry.startAddress + (itemIndex * entry.size);
}

void TMemoryManager::loadPartitionTable()
{
  uint16_t signature;
  EEPROM.get(DEVICE_SIGNATURE_ADRESS, signature);

  uint16_t transactionFlag;
  EEPROM.get(TRANSACTION_FLAG_ADRESS, transactionFlag);

  // If there is a signature error or failed update attempt re-initialize Partition Table.
  if (signature != DEVICE_SIGNATURE || transactionFlag == TRANSACTION_PROGRESS)
  {
    errorManager.errorHandler(ErrorCode::INTEGRITY_ERROR);
    initPartitionTable();
  }
  else EEPROM.get(HEADER_START_ADDRESS, partitionTable);
}

void TMemoryManager::factoryReset()
{
  initPartitionTable();

  // --- Assign Default Values Here if no global defaultVariables is present.

  settings = defaultSettings;
  writeData(PartitionID::SETTINGS, settings);

  eepromVersion = system_version::FIRMWARE;
  writeData(PartitionID::VERSION, eepromVersion);

  delay(500);
  while(true);
}

// Validate the firmware versions
ErrorCode TMemoryManager::firmwareValidate()
{
  // If no version difference, return no error
  if (eepromVersion == system_version::FIRMWARE) return ErrorCode::NONE;

  // If the firmware version is greater than the EEPROM version, it means the firmware has been updated.
  if (eepromVersion < system_version::FIRMWARE)
  {
    // If the EEPROM version is less than the minimum supported version, return an error.
    if (eepromVersion < system_version::MIN_SUPPORTED) return ErrorCode::VERSION_NOT_SUPPORTED;

    delay(2000);

    if (eepromVersion == VersionInfo{1, 4, 2}) // --- Example version declare.
    {
      // --- Version Specific Update/Fix Example
    }

    // --- Generic update path

    // !!! Please check the use check for the T Core Modules V4.0 to see about resize rules.
    resizePartition(PartitionID::SETTINGS, sizeof(settingsStruct));
    resizePartition(PartitionID::VERSION, sizeof(VersionInfo));
    
    // Save the new firmware version to EEPROM
    eepromVersion = system_version::FIRMWARE;
    writeData(PartitionID::SETTINGS, settings);
    writeData(PartitionID::VERSION, eepromVersion);
    
    while (true); // Lock the Device.
  }

  // If device is downgraded return the error/warning
  return ErrorCode::DOWNGRADED_FIRMWARE;
}
