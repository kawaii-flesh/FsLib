#pragma once
#include <string_view>
#include <switch.h>

namespace FsLib
{
    // Opens and maps the Bis filesystem to DeviceName.
    bool OpenBisFileSystem(std::string_view DeviceName, FsBisPartitionId PartitionID);
} // namespace FsLib
