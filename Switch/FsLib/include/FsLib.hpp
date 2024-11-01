#pragma once
#include "Directory.hpp"
#include "InputFile.hpp"
#include "OutputFile.hpp"
#include "Storage.hpp"
#include <string>
#include <switch.h>

namespace FsLib
{
    // Initializes fslib. Basically steals LibNX's SDMC handle and does nothing else.
    void Initialize(void);
    // Exits
    void Exit(void);
    // Returns internal error string.
    const char *GetErrorString(void);
    /*
        Processes a path. Returns true on success. PathOut is a C string because Switch gives me grief with std::string.c_str().
        This is mostly needed for Directory and File classes to work, but I'm not going to stop someone from using it for something else.
    */
    bool ProcessPath(std::string_view PathIn, FsFileSystem **FileSystemOut, char *PathOut, size_t PathOutMax);

    // These funcions are sort of shortcuts to be used instead of allocating a new instance of Directory or File to check things.
    // Directory functions.
    bool CreateDirectory(std::string_view DirectoryPath);
    // The path for this should have a trailing slash to work properly. If not, the last directory in the path will get skipped. Should probably do something about that.
    bool CreateDirectoryRecursively(std::string_view DirectoryPath);
    bool DeleteDirectory(std::string_view DirectoryPath);
    bool DeleteDirectoryRecursively(std::string_view DirectoryPath);
    bool DirectoryExists(std::string_view DirectoryPath);
    bool RenameDirectory(std::string_view Old, std::string_view New);

    // File functions.
    bool FileExists(std::string_view sFilePath);
    bool DeleteFile(std::string_view FilePath);
    int64_t GetFileSize(std::string_view FilePath);
    bool RenameFile(std::string_view Old, std::string_view New);

    // Opens save data type and associates it with DeviceName
    bool OpenSystemSaveFileSystem(std::string_view DeviceName, uint64_t SystemSaveID);
    bool OpenAccountSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID, AccountUid UserID);
    bool OpenBCATSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID);
    bool OpenDeviceSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID);
    bool OpenTemporarySaveFileSystem(std::string_view DeviceName);
    bool OpenCacheSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID, uint16_t SaveIndex);
    bool OpenSystemBCATSaveFileSystem(std::string_view DeviceName, uint64_t SystemSaveID);

    // Commits data to filesystem associated with DeviceName
    bool CommitDataToFileSystem(std::string_view DeviceName);
    // Closes filesystem handle associated with deviceName.
    bool CloseFileSystem(std::string_view DeviceName);
} // namespace FsLib
