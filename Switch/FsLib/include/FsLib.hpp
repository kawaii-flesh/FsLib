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
    const char *GetErrorString(void);
    /*
        Processes a path. Returns true on success. PathOut is a C string because Switch gives me grief with std::string.c_str().
        This is mostly needed for Directory and File to work, but I'm not going to stop someone from using it for something else.
    */
    bool ProcessPath(const std::string &PathIn, FsFileSystem **FileSystemOut, char *PathOut, size_t PathOutMax);

    // These funcions are sort of shortcuts to be used instead of allocating a new instance of Directory or File to check things.
    // Directory functions.
    bool CreateDirectory(const std::string &DirectoryPath);
    bool DeleteDirectory(const std::string &DirectoryPath);
    bool DeleteDirectoryRecursively(const std::string &DirectoryPath);
    bool DirectoryExists(const std::string &DirectoryPath);
    // File functions.
    bool FileExists(const std::string &FilePath);
    bool DeleteFile(const std::string &FilePath);
    int64_t GetFileSize(const std::string &FilePath);

    // Opens save data type and associates it with DeviceName
    bool OpenSystemSaveFileSystem(const std::string &DeviceName, uint64_t SystemSaveID);
    bool OpenAccountSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID, AccountUid UserID);
    bool OpenBCATSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID);
    bool OpenDeviceSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID);
    bool OpenTemporarySaveFileSystem(const std::string &DeviceName);
    bool OpenCacheSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID, uint16_t SaveIndex);
    bool OpenSystemBCATSaveFileSystem(const std::string &DeviceName, uint64_t SystemSaveID);

    // Commits data to filesystem associated with DeviceName
    bool CommitDataToFileSystem(const std::string &DeviceName);
    // Closes filesystem handle associated with deviceName.
    bool CloseFileSystem(const std::string &DeviceName);
} // namespace FsLib
