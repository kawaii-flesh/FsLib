#include "fslib.hpp"
#include "string.hpp"
#include <unordered_map>

// This macro is so I can reject requests to mount with sdmc as device without having to repeat typing it over and over
#define SDMC_DEVICE_GUARD(DeviceName)                                                                                                                          \
    if (DeviceName == SD_CARD_DEVICE_NAME)                                                                                                                     \
    {                                                                                                                                                          \
        return false;                                                                                                                                          \
    }

namespace
{
    // Filesystems paired with their mount point.
    std::unordered_map<std::string, FsFileSystem> s_DeviceMap;
    // String for SD card device.
    const std::string SD_CARD_DEVICE_NAME = "sdmc";
} // namespace

// This error string is shared globally, but I didn't want it extern'd in the header.
std::string g_ErrorString = "No Errors to report, sir.";

// This is for opening functions to search and make sure there are no duplicate uses of the same device name.
static bool DeviceNameIsInUse(const std::string &DeviceName)
{
    return s_DeviceMap.find(DeviceName) != s_DeviceMap.end();
}

bool FsLib::Initialize(void)
{
    // Just in case. I think LibNX takes care of this by default.
    Result FsError = fsInitialize();
    if (R_FAILED(FsError))
    {
        // Not going to return on this one mainly because LibNX protects against this IIRC...
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%0X initializing FS.", FsError);
    }

    FsError = fsOpenSdCardFileSystem(&s_DeviceMap[SD_CARD_DEVICE_NAME]);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%0X mounting SD card.", FsError);
        return false;
    }

    return true;
}

void FsLib::Exit(void)
{
    for (auto &[DeviceName, FileSystemHandle] : s_DeviceMap)
    {
        // This is called directly instead of FsLib::CloseFileSystem since that guards against closing the SD.
        fsFsClose(&FileSystemHandle);
    }
    // JIC
    fsExit();
}

std::string FsLib::GetErrorString(void)
{
    return g_ErrorString;
}

bool FsLib::OpenSystemSaveFileSystem(const std::string &DeviceName, uint64_t SystemSaveID)
{
    // Reject altogether if sdmc is what is passed.
    SDMC_DEVICE_GUARD(DeviceName);

    if (DeviceNameIsInUse(DeviceName))
    {
        // Close the previously opened system using the same name before continuing.
        FsLib::CloseFileSystem(DeviceName);
    }

    FsSaveDataAttribute SaveDataAttributes = {.application_id = 0,
                                              .uid = {0},
                                              .system_save_data_id = SystemSaveID,
                                              .save_data_type = FsSaveDataType_System,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    Result FsError = fsOpenSaveDataFileSystem(&s_DeviceMap[DeviceName], FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening system save data 0x%016lX.", FsError, SystemSaveID);
        return false;
    }

    return true;
}

bool FsLib::OpenAccountSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID, AccountUid UserID)
{
    SDMC_DEVICE_GUARD(DeviceName);

    if (DeviceNameIsInUse(DeviceName))
    {
        FsLib::CloseFileSystem(DeviceName);
    }

    FsSaveDataAttribute SaveDataAttributes = {.application_id = ApplicationID,
                                              .uid = UserID,
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_System,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    Result FsError = fsOpenSaveDataFileSystem(&s_DeviceMap[DeviceName], FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0xX opening account save data for 0x%016lX.", FsError, ApplicationID);
        return false;
    }

    return true;
}

bool FsLib::OpenBCATSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID)
{
    SDMC_DEVICE_GUARD(DeviceName);

    if (DeviceNameIsInUse(DeviceName))
    {
        FsLib::CloseFileSystem(DeviceName);
    }

    FsSaveDataAttribute SaveDataAttributes = {.application_id = ApplicationID,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Bcat,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    Result FsError = fsOpenSaveDataFileSystem(&s_DeviceMap[DeviceName], FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening BCAT for 0x%016lX.", FsError, ApplicationID);
        return false;
    }

    return true;
}

bool FsLib::OpenDeviceSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID)
{
    SDMC_DEVICE_GUARD(DeviceName);

    if (DeviceNameIsInUse(DeviceName))
    {
        FsLib::CloseFileSystem(DeviceName);
    }

    FsSaveDataAttribute SaveDataAttributes = {.application_id = ApplicationID,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Device,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    Result FsError = fsOpenSaveDataFileSystem(&s_DeviceMap[DeviceName], FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening device save for 0x%016X", FsError, ApplicationID);
        return false;
    }

    return true;
}

bool FsLib::OpenTemporarySaveFileSystem(const std::string &DeviceName)
{
    SDMC_DEVICE_GUARD(DeviceName);

    if (DeviceNameIsInUse(DeviceName))
    {
        FsLib::CloseFileSystem(DeviceName);
    }

    FsSaveDataAttribute SaveDataAttributes = {.application_id = 0,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Temporary,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    Result FsError = fsOpenSaveDataFileSystem(&s_DeviceMap[DeviceName], FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening temporary save data.", FsError);
        return false;
    }

    return true;
}

bool FsLib::OpenCacheSaveFileSystem(const std::string &DeviceName, uint64_t ApplicationID, uint16_t SaveIndex)
{
    SDMC_DEVICE_GUARD(DeviceName);

    if (DeviceNameIsInUse(DeviceName))
    {
        FsLib::CloseFileSystem(DeviceName);
    }

    FsSaveDataAttribute SaveDataAttributes = {.application_id = ApplicationID,
                                              .uid = {0},
                                              .system_save_data_id = 0,
                                              .save_data_type = FsSaveDataType_Cache,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = SaveIndex};

    Result FsError = fsOpenSaveDataFileSystem(&s_DeviceMap[DeviceName], FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening cache save for 0x%016X with index %02d.", FsError, ApplicationID, SaveIndex);
        return false;
    }

    return true;
}

bool FsLib::OpenSystemBCATSaveFileSystem(const std::string &DeviceName, uint64_t SystemSaveID)
{
    SDMC_DEVICE_GUARD(DeviceName);

    if (DeviceNameIsInUse(DeviceName))
    {
        FsLib::CloseFileSystem(DeviceName);
    }

    FsSaveDataAttribute SaveDataAttributes = {.application_id = 0,
                                              .uid = {0},
                                              .system_save_data_id = SystemSaveID,
                                              .save_data_type = FsSaveDataType_SystemBcat,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    Result FsError = fsOpenSaveDataFileSystem(&s_DeviceMap[DeviceName], FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening system BCAT for 0x%016X.", FsError, SystemSaveID);
        return false;
    }

    return true;
}

bool FsLib::CloseFileSystem(const std::string &DeviceName)
{
    // Guard against closing sdmc. Only exiting FsLib will do that.
    SDMC_DEVICE_GUARD(DeviceName);

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

FsFileSystem *FsLib::GetFileSystemHandleByDeviceName(const std::string &DeviceName)
{
    if (DeviceNameIsInUse(DeviceName))
    {
        return &s_DeviceMap[DeviceName];
    }
    return NULL;
}

bool FsLib::CreateDirectoryRecursively(const std::string &DirectoryPath)
{
    std::string DeviceName, TruePath;
    bool PathProcessed = FsLib::String::ProcessPathString(DirectoryPath, DeviceName, TruePath);
    FsFileSystem *TargetFileSystem = FsLib::GetFileSystemHandleByDeviceName(DeviceName);
    if (PathProcessed == false || TargetFileSystem == NULL)
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error creating directories: Invalid path supplied.");
        return false;
    }

    // Skip the first slash in the path.
    size_t SlashPosition = 1;
    while ((SlashPosition = TruePath.find_first_of('/', SlashPosition)) != TruePath.npos)
    {
        Result FsError = fsFsCreateDirectory(TargetFileSystem, TruePath.substr(0, SlashPosition).c_str());
        if (R_FAILED(FsError))
        {
            // This will fail if the directory already exists. I want to error check this better but adding calls to check will slow things down.
            g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X creating directory \"%s\".", FsError, TruePath.substr(0, SlashPosition).c_str());
        }
        ++SlashPosition;
    }
    return true;
}
