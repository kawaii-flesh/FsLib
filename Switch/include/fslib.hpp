#pragma once
#include "directory.hpp"
#include "file.hpp"
#include <string>
#include <switch.h>

namespace FsLib
{
    // Initializes fslib and mounts sd card.
    bool Initialize(void);
    // Exits
    void Exit(void);
    // Returns internal error string.
    std::string GetErrorString(void);
    // Opens system save data and associates deviceName with it.
    bool OpenSystemSaveFileSystem(const std::string &DeviceName, uint64_t SystemSaveID);
    // Opens account save data and associates deviceName with it.
    bool OpenAccountSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID, AccountUid UserID);
    // Opens bcat to deviceName
    bool OpenBCATSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID);
    // Opens device save and assigns deviceName to it
    bool OpenDeviceSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID);
    // Opens temporary save data and assigns deviceName to it.
    bool OpenTemporarySaveFileSystem(const std::string &DeviceName);
    // Opens cache save and associates it with deviceName.
    bool OpenCacheSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID, uint16_t SaveIndex);
    // Opens system bcat and associates it with deviceName
    bool OpenSystemBCATSaveFileSystem(const std::string &DeviceName, uint64_t SystemSaveID);
    // Closes filesystem handle associated with deviceName.
    bool CloseFileSystem(const std::string &DeviceName);
    // Uses deviceName to search the map for the filesystem associated with it. Returns NULL on failure.
    FsFileSystem *GetFileSystemHandleByDeviceName(const std::string &DeviceName);
    // Attempts to create directory recursively. Still trying to figure out how to error check this a good way.
    bool CreateDirectoryRecursively(const std::string &DirectoryPath);
} // namespace FsLib
