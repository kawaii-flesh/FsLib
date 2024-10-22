#include "FsLib.hpp"
#include "String.hpp"
#include <unordered_map>

// Macro to reject requests to mount with SDMC as device name
#define SDMC_DEVICE_GUARD(DeviceName)                                                                                                                          \
    if (DeviceName == SDMC_DEVICE_NAME)                                                                                                                        \
    {                                                                                                                                                          \
        return false;                                                                                                                                          \
    }

namespace
{
    // 3DS can use UTF-16 paths so that's what we're using.
    std::unordered_map<std::u16string, FS_Archive> s_DeviceMap;
    // This only works because the string is so short.
    constexpr std::u16string SDMC_DEVICE_NAME = u"sdmc";
} // namespace

// Globally shared error string.
std::string g_ErrorString = "No errors encountered.";

// Checks if device is already in map.
static bool DeviceNameIsInUse(const std::u16string &DeviceName)
{
    return s_DeviceMap.find(DeviceName) != s_DeviceMap.end();
}

bool FsLib::Initialize(void)
{
    Result FsError = fsInit();
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error initializing FS: 0x%08X.", FsError);
        return false;
    }

    FsError = FSUSER_OpenArchive(&s_DeviceMap[SDMC_DEVICE_NAME], ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening SDMC Archive: 0x%08X.", FsError);
        return false;
    }
    return true;
}

void FsLib::Exit(void)
{
    for (auto &[DeviceName, Archive] : s_DeviceMap)
    {
        FSUSER_CloseArchive(Archive);
    }
    fsExit();
}

bool FsLib::ProcessPath(const std::u16string &PathIn, FS_Archive **ArchiveOut, std::u16string &PathOut)
{
    size_t ColonPosition = PathIn.find_first_of(L':');
    if (ColonPosition == PathIn.npos)
    {
        return false;
    }

    std::u16string DeviceName = PathIn.substr(0, ColonPosition);
    if (!DeviceNameIsInUse(DeviceName))
    {
        return false;
    }
    // Should be safe now.
    PathOut = PathIn.substr(ColonPosition + 1, PathIn.length());
    *ArchiveOut = &s_DeviceMap.at(DeviceName);
    return true;
}

bool FsLib::OpenSaveData(const std::u16string &DeviceName)
{
    SDMC_DEVICE_GUARD(DeviceName);

    Result FsError = FSUSER_OpenArchive(&s_DeviceMap[DeviceName], ARCHIVE_SAVEDATA, fsMakePath(PATH_EMPTY, NULL));
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening save data archive: 0x%08X.", FsError);
        return false;
    }
    return true;
}

bool FsLib::OpenExtData(const std::u16string &DeviceName, uint32_t ExtDataID)
{
    SDMC_DEVICE_GUARD(DeviceName);

    uint32_t BinaryData[] = {MEDIATYPE_SD, ExtDataID, 0x00000000};
    FS_Path PathData = {.type = PATH_BINARY, .size = 0x0C, .data = BinaryData};

    Result FsError = FSUSER_OpenArchive(&s_DeviceMap[DeviceName], ARCHIVE_EXTDATA, PathData);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening ExtData archive %08X: 0x%08X.", ExtDataID, FsError);
        return false;
    }
    return true;
}

bool FsLib::OpenSharedExtData(const std::u16string &DeviceName, uint32_t SharedExtDataID)
{
    SDMC_DEVICE_GUARD(DeviceName);

    uint32_t BinaryData[] = {MEDIATYPE_NAND, SharedExtDataID, 0x00048000};
    FS_Path PathData = {.type = PATH_BINARY, .size = 0x0C, .data = BinaryData};
    Result FsError = FSUSER_OpenArchive(&s_DeviceMap[DeviceName], ARCHIVE_SHARED_EXTDATA, PathData);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening shared extdata %08X: 0x%08X.", SharedExtDataID, FsError);
        return false;
    }
    return true;
}

bool FsLib::OpenSystemSaveData(const std::u16string &DeviceName, uint32_t UniqueID)
{
    SDMC_DEVICE_GUARD(DeviceName);

    uint32_t BinaryData[] = {MEDIATYPE_NAND, 0x00020000 | UniqueID};
    FS_Path PathData = {.type = PATH_BINARY, .size = 0x08, .data = BinaryData};
    Result FsError = FSUSER_OpenArchive(&s_DeviceMap[DeviceName], ARCHIVE_SYSTEM_SAVEDATA, PathData);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening system save data %08X: 0x%08.", UniqueID, FsError);
        return false;
    }
    return true;
}

bool FsLib::OpenGamecardSaveData(const std::u16string &DeviceName)
{
    SDMC_DEVICE_GUARD(DeviceName);

    Result FsError = FSUSER_OpenArchive(&s_DeviceMap[DeviceName], ARCHIVE_GAMECARD_SAVEDATA, fsMakePath(PATH_EMPTY, NULL));
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening game card save data: 0x%08X.", FsError);
        return false;
    }
    return true;
}

bool FsLib::OpenUserSaveData(const std::u16string &DeviceName, FS_MediaType MediaType, uint32_t LowerID, uint32_t UpperID)
{
    SDMC_DEVICE_GUARD(DeviceName);

    uint32_t BinaryData[] = {MediaType, LowerID, UpperID};
    FS_Path PathData = {.type = PATH_BINARY, .size = 0x0C, .data = BinaryData};
    Result FsError = FSUSER_OpenArchive(&s_DeviceMap[DeviceName], ARCHIVE_USER_SAVEDATA, PathData);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening user save data %08X%08X: 0x%08X.", UpperID, LowerID, FsError);
        return false;
    }
    return true;
}

bool FsLib::CloseDevice(const std::u16string &DeviceName)
{
    if (!DeviceNameIsInUse(DeviceName))
    {
        return false;
    }

    Result FsError = FSUSER_CloseArchive(s_DeviceMap[DeviceName]);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error closing archive: 0x%08X.", FsError);
        return false;
    }
    return true;
}
