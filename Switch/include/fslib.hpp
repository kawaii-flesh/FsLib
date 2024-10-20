#pragma once
#include "Directory.hpp"
#include "File.hpp"
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
    /*
        Processes a path. Returns true on success. PathOut is a C string because Switch gives me grief with std::string.c_str().
        This is mostly needed for Directory and File to work, but I'm not going to stop someone from using it for something else.
    */
    bool ProcessPath(const std::string &PathIn, FsFileSystem *FileSystemOut, char *PathOut, size_t PathOutMax);
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
} // namespace FsLib
