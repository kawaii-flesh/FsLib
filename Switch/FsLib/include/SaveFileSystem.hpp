#pragma once
#include <cstdint>
#include <string>
#include <switch.h>

namespace FsLib
{
    // These are shortcut functions to open and mount save filesystems.
    bool OpenSystemSaveFileSystem(std::string_view DeviceName, uint64_t SystemSaveID);
    bool OpenAccountSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID, AccountUid UserID);
    bool OpenBCATSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID);
    bool OpenDeviceSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID);
    bool OpenTemporarySaveFileSystem(std::string_view DeviceName);
    bool OpenCacheSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID, uint16_t SaveIndex);
    bool OpenSystemBCATSaveFileSystem(std::string_view DeviceName, uint64_t SystemSaveID);
} // namespace FsLib
