#include "FsLib.hpp"
#include "String.hpp"
#include <cstring>
#include <unordered_map>

namespace
{
    // Filesystems paired with their mount point.
    std::unordered_map<std::string_view, FsFileSystem> s_DeviceMap;
    // String for SD card device.
    constexpr std::string_view SD_CARD_DEVICE_NAME = "sdmc";
} // namespace

// This error string is shared globally, but I didn't want it extern'd in the header.
std::string g_ErrorString = "No errors encountered.";

// This is for opening functions to search and make sure there are no duplicate uses of the same device name.
static bool DeviceNameIsInUse(std::string_view DeviceName)
{
    return s_DeviceMap.find(DeviceName) != s_DeviceMap.end();
}

void FsLib::Initialize(void)
{
    // If you don't want me to call the function directly... I'll just steal the handle.
    std::memcpy(&s_DeviceMap[SD_CARD_DEVICE_NAME], fsdevGetDeviceFileSystem(SD_CARD_DEVICE_NAME.data()), sizeof(FsFileSystem));
}

void FsLib::Exit(void)
{
    for (auto &[DeviceName, FileSystemHandle] : s_DeviceMap)
    {
        // This is called directly instead of FsLib::CloseFileSystem since that guards against closing the SD.
        fsFsClose(&FileSystemHandle);
    }
}

const char *FsLib::GetErrorString(void)
{
    return g_ErrorString.c_str();
}

bool FsLib::RegisterFileSystem(std::string_view DeviceName, FsFileSystem *FileSystem)
{
    if (DeviceName == SD_CARD_DEVICE_NAME)
    {
        g_ErrorString = "Error: Cannot use sdmc for registering device.";
        return false;
    }

    if (DeviceNameIsInUse(DeviceName))
    {
        FsLib::CloseFileSystem(DeviceName);
    }

    std::memcpy(&s_DeviceMap[DeviceName], FileSystem, sizeof(FsFileSystem));

    return true;
}

bool FsLib::GetFileSystemByDeviceName(std::string_view DeviceName, FsFileSystem **FileSystemOut)
{
    if (!DeviceNameIsInUse(DeviceName))
    {
        return false;
    }
    *FileSystemOut = &s_DeviceMap[DeviceName];
    return true;
}

bool FsLib::CommitDataToFileSystem(std::string_view DeviceName)
{
    if (!DeviceNameIsInUse(DeviceName))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error committing data to \"%s\": Device does not exist.", DeviceName.data());
        return false;
    }

    Result FsError = fsFsCommit(&s_DeviceMap[DeviceName]);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X committing data to \"%s\".", FsError, DeviceName.data());
        return false;
    }
    return true;
}

bool FsLib::CloseFileSystem(std::string_view DeviceName)
{
    // Guard against closing sdmc. Only exiting FsLib will do that.
    if (DeviceName == SD_CARD_DEVICE_NAME)
    {
        return false;
    }

    if (DeviceNameIsInUse(DeviceName))
    {
        // Close
        fsFsClose(&s_DeviceMap[DeviceName]);
        // Erase from map.
        s_DeviceMap.erase(DeviceName);
        return true;
    }
    return false;
}
