#include "fslib.hpp"
#include "string.hpp"
#include <unordered_map>

namespace
{
    // Filesystems paired with their mount point.
    std::unordered_map<std::string, FsFileSystem> s_DeviceMap;
    // String for SD card device.
    const std::string SD_CARD_DEVICE_NAME = "sdmc";
} // namespace

// This error string is shared globally, but I didn't want it extern'd in the header.
std::string g_ErrorString;

bool fslib::initialize(void)
{
    // Just in case.
    Result fsError = fsInitialize();
    if (R_FAILED(fsError))
    {
        // We're gonna set the string, but this might fail anyway because libnx takes care of this and has guards against doubling it IIRC.
        // Not going to return false. This might fail no matter what.
        g_ErrorString = fslib::getFormattedString("Error 0x%X initializing FS.", fsError);
    }
    // Mount the sd card to sdmc. This only works internally with fslib. Libnx stdio is out of my control.
    fsError = fsOpenSdCardFileSystem(&s_DeviceMap[SD_CARD_DEVICE_NAME]);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X mounting SD Card.", fsError);
        return false;
    }
    return true;
}

void fslib::exit(void)
{
    // Loop through map and close all open handles.
    for (auto &[deviceName, filesystemHandle] : s_DeviceMap)
    {
        fsFsClose(&filesystemHandle);
    }
    // Just in case
    fsExit();
}

std::string fslib::getErrorString(void)
{
    return g_ErrorString;
}

bool fslib::getFilesystemHandleByName(const std::string &deviceName, FsFileSystem &handleOut)
{
    if (s_DeviceMap.find(deviceName) != s_DeviceMap.end())
    {
        handleOut = s_DeviceMap.at(deviceName);
        return true;
    }
    // Set error string and return false.
    g_ErrorString = fslib::getFormattedString("\"%s\" was not found in device map!", deviceName.c_str());
    return false;
}
