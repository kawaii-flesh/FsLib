#pragma once
#include "Directory.hpp"
#include "DirectoryFunctions.hpp"
#include "FileFunctions.hpp"
#include "InputFile.hpp"
#include "OutputFile.hpp"
#include "Path.hpp"
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
    // Maps DeviceName to FileSystem. If device name is already in use, previous one is closed before continuing. Returns false if something failed.
    bool MapFileSystem(std::string_view DeviceName, FsFileSystem *FileSystem);
    // Retrieves handle to FileSystem mapped to DeviceName. Returns false if DeviceName isn't found.
    bool GetFileSystemByDeviceName(std::string_view DeviceName, FsFileSystem **FileSystemOut);
    // Commits data to filesystem associated with DeviceName
    bool CommitDataToFileSystem(std::string_view DeviceName);
    // Closes filesystem handle associated with deviceName.
    bool CloseFileSystem(std::string_view DeviceName);
} // namespace FsLib
