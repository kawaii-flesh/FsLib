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

// This is for opening functions to search and make sure there are no duplicate uses of the same device name.
static inline bool deviceNameIsInUse(const std::string &deviceName)
{
    return s_DeviceMap.find(deviceName) != s_DeviceMap.end();
}

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

bool fslib::openSystemSaveFileSystem(const std::string &deviceName, uint64_t systemSaveID)
{
    if (deviceNameIsInUse(deviceName))
    {
        g_ErrorString = fslib::getFormattedString("Error mounting system save data with device name \"%s\".", deviceName.c_str());
        return false;
    }

    FsSaveDataAttribute saveAttributes = {.application_id = 0,
                                          .uid = 0,
                                          .system_save_data_id = systemSaveID,
                                          .save_data_type = FsSaveDataType_System,
                                          .save_data_rank = FsSaveDataRank_Primary,
                                          .save_data_index = 0};

    Result fsError = fsOpenSaveDataFileSystem(&s_DeviceMap[deviceName], FsSaveDataSpaceId_User, &saveAttributes);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X opening system save data.", fsError);
        return false;
    }
    return true;
}

bool fslib::openAccountSaveFileSystem(const std::string &deviceName, uint64_t applicationID, AccountUid userID)
{
    if (deviceNameIsInUse(deviceName))
    {
        g_ErrorString = fslib::getFormattedString("Error mounting account save with device name \"%s\".", deviceName.c_str());
        return false;
    }

    FsSaveDataAttribute saveAttributes = {.application_id = applicationID,
                                          .uid = userID,
                                          .system_save_data_id = 0,
                                          .save_data_type = FsSaveDataType_Account,
                                          .save_data_rank = FsSaveDataRank_Primary,
                                          .save_data_index = 0};

    Result fsError = fsOpenSaveDataFileSystem(&s_DeviceMap[deviceName], FsSaveDataSpaceId_User, &saveAttributes);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X opening account save data.", fsError);
        return false;
    }
    return true;
}

bool fslib::openBCATSaveFileSystem(const std::string &deviceName, uint64_t applicationID)
{
    if (deviceNameIsInUse(deviceName))
    {
        g_ErrorString = fslib::getFormattedString("Error mounting BCAT with device name \"%s\".", deviceName.c_str());
        return false;
    }

    FsSaveDataAttribute saveAttributes = {.application_id = applicationID,
                                          .uid = {0},
                                          .system_save_data_id = 0,
                                          .save_data_type = FsSaveDataType_Bcat,
                                          .save_data_rank = FsSaveDataRank_Primary,
                                          .save_data_index = 0};

    Result fsError = fsOpenSaveDataFileSystem(&s_DeviceMap[deviceName], FsSaveDataSpaceId_User, &saveAttributes);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X opening BCAT save data.", fsError);
        return false;
    }
    return true;
}

bool fslib::closeFileSystem(const std::string &deviceName)
{
    if (deviceNameIsInUse(deviceName))
    {
        fsFsClose(&s_DeviceMap[deviceName]);
        return true;
    }
    return false;
}

bool fslib::getFileSystemHandleByName(const std::string &deviceName, FsFileSystem &handleOut)
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

bool fslib::createDirectoryRecursively(const std::string &directoryPath)
{
    // Get stuff we need to start.
    FsFileSystem targetFileSystem;
    std::string deviceName = fslib::getDeviceFromPath(directoryPath);
    std::string truePath = fslib::removeDeviceFromPath(directoryPath);
    bool targetSystemFound = fslib::getFileSystemHandleByName(deviceName, targetFileSystem);
    if (deviceName.empty() || truePath.empty() || targetSystemFound == false)
    {
        g_ErrorString = fslib::getFormattedString("Error creating directories: Invalid path supplied.");
        return false;
    }

    // Skip the first slash. This is going to need work when I get my switch back. I feel like I'm missing something here.
    size_t slashPosition = 1;
    while ((slashPosition = truePath.find_first_of('/', slashPosition)) != truePath.npos)
    {
        std::string currentDirectoryPath = truePath.substr(0, slashPosition);
        Result fsError = fsFsCreateDirectory(&targetFileSystem, currentDirectoryPath.c_str());
        if (R_FAILED(fsError))
        {
            // Set the error string, but don't return. This can happen because of the directory already existing.
            g_ErrorString = fslib::getFormattedString("Error 0x%X creating directory \"%s\".", fsError, currentDirectoryPath.c_str());
        }
    }
    // I might rework this when I have time. Error checking this might be tricky.
    return true;
}
