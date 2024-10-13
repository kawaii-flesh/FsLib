#pragma once
#include <string>
#include <switch.h>

namespace fslib
{
    // Initializes fslib and mounts sd card.
    bool initialize(void);
    // Exits
    void exit(void);
    // Returns internal error string.
    std::string getErrorString(void);
    // Opens system save data and associates deviceName with it.
    bool openSystemSaveFileSystem(const std::string &deviceName, uint64_t systemSaveID);
    // Opens account save data and associates deviceName with it.
    bool openAccountSaveFileSystem(const std::string &deviceName, uint64_t applicationID, AccountUid userID);
    // Opens bcat to deviceName
    bool openBCATSaveFileSystem(const std::string &deviceName, uint64_t applicationID);
    // Uses deviceName to search the map for the filesystem associated with it.s
    bool getFileSystemHandleByName(const std::string &deviceName, FsFileSystem &handleOut);
    // Attempts to create directory recursively. Still trying to figure out how to error check this a good way.
    bool createDirectoryRecursively(const std::string &directoryPath);
} // namespace fslib
