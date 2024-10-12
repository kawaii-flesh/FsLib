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
    // Retrieves the fileHandle associated with [deviceName]. Returns false on failure.
    bool getFilesystemHandleByName(const std::string &deviceName, FsFileSystem &handleOut);
} // namespace fslib
