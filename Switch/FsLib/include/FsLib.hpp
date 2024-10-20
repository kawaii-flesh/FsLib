#pragma once
#include "Directory.hpp"
#include "File.hpp"
#include "Storage.hpp"
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
    bool ProcessPath(const std::string &PathIn, FsFileSystem **FileSystemOut, char *PathOut, size_t PathOutMax);

    // Opens save data type and associates it with DeviceName
    bool OpenSystemSaveFileSystem(const std::string &DeviceName, uint64_t SystemSaveID);
    bool OpenAccountSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID, AccountUid UserID);
    bool OpenBCATSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID);
    bool OpenDeviceSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID);
    bool OpenTemporarySaveFileSystem(const std::string &DeviceName);
    bool OpenCacheSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID, uint16_t SaveIndex);
    bool OpenSystemBCATSaveFileSystem(const std::string &DeviceName, uint64_t SystemSaveID);

    // Opens the filesystem
    // Closes filesystem handle associated with deviceName.
    bool CloseFileSystem(const std::string &DeviceName);
    // Commits data to filesystem accosiated with DeviceName
    bool CommitDataToFileSystem(const std::string &DeviceName);
} // namespace FsLib
