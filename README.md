# T Core Modules

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Build-orange?style=flat-square&logo=platformio)](https://platformio.org/)

T Core Modules are a helper for the production-ready devices' version validations, EEPROM management, error interrupting and handling also POST (Power-On-Self-Test) capabilities. Used in my own Projects. 

---

## Key Features

- **TPT (T Partition Table) System**
  - Supports up to **10 unique Partition IDs** with **multi-instance support**.
  - **Indexed Access:** Partition IDs can instantiate multiple isolated sub-blocks allowing dynamic array-like indexing without consuming additional ID slots.
  - **Advanced EEPROM Memory Management Engine:** Dynamic data resizing and shifting based on the table updates.

* **Robust Error Handler:** Seamlessly catches and manages hardware, runtime, and user-input errors with minimal overhead.

* **Self-Test** Automatically verifies RAM integrity and external peripheral readiness at boot time.

---
