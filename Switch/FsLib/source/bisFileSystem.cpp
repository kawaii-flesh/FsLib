#include "bisFileSystem.hpp"
#include "fslib.hpp"
#include "string.hpp"
#include <string>

extern std::string g_fslibErrorString;

bool fslib::openBisFileSystem(std::string_view deviceName, FsBisPartitionId partitionID)
{
    FsFileSystem fileSystem;
    Result fsError = fsOpenBisFileSystem(&fileSystem, partitionID, "");
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening BIS filesystem: 0x%08.", fsError);
        return false;
    }

    if (!fslib::mapFileSystem(deviceName, &fileSystem))
    {
        fsFsClose(&fileSystem);
        return false;
    }
    return true;
}
