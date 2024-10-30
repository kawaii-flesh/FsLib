#include "FsLib.hpp"
#include "String.hpp"
#include <array>
#include <cstring>
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
    // String for SD card device. This only works as constexpr because the string is so short.
    constexpr std::string SD_CARD_DEVICE_NAME = "sdmc";
} // namespace

// This error string is shared globally, but I didn't want it extern'd in the header.
std::string g_ErrorString = "No Errors to report, sir.";

// This is for opening functions to search and make sure there are no duplicate uses of the same device name.
static bool DeviceNameIsInUse(const std::string &DeviceName)
{
    return s_DeviceMap.find(DeviceName) != s_DeviceMap.end();
}

void FsLib::Initialize(void)
{
    // If you don't want me to call the function directly... I'll just steal the handle.
    std::memcpy(&s_DeviceMap[SD_CARD_DEVICE_NAME], fsdevGetDeviceFileSystem(SD_CARD_DEVICE_NAME.c_str()), sizeof(FsFileSystem));
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

bool FsLib::ProcessPath(const std::string &PathIn, FsFileSystem **FileSystemOut, char *PathOut, size_t PathOutMax)
{
    size_t ColonPosition = PathIn.find_first_of(':');
    if (ColonPosition == PathIn.npos)
    {
        return false;
    }
    // Split string at :
    std::string DeviceName = PathIn.substr(0, ColonPosition);
    std::string Path = PathIn.substr(ColonPosition + 1, PathIn.length());
    if (Path.length() >= PathOutMax || !DeviceNameIsInUse(DeviceName))
    {
        return false;
    }
    /*
        This feels stupid but it's more reliable than trying to use c_str() with switch FS.
        For some reason passing a short path with std::string.c_str() makes the switch throw 0xD401,
        but it doesn't if you pass the same thing as a C string...
    */
    std::memset(PathOut, 0x00, PathOutMax);
    std::memcpy(PathOut, Path.c_str(), Path.length());
    // Pointer to File system
    *FileSystemOut = &s_DeviceMap[DeviceName];
    // Should be good to go.
    return true;
}

bool FsLib::CreateDirectory(const std::string &DirectoryPath)
{
    FsFileSystem *FileSystem = NULL;
    std::array<char, FS_MAX_PATH> Path;
    if (!FsLib::ProcessPath(DirectoryPath, &FileSystem, Path.data(), FS_MAX_PATH))
    {
        return false;
    }

    Result FsError = fsFsCreateDirectory(FileSystem, Path.data());
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X creating directory \"%s\".", FsError, DirectoryPath.c_str());
        return false;
    }
    return true;
}

bool FsLib::CreateDirectoryRecursively(const std::string &DirectoryPath)
{
    // Get the position of the first slash and skip it.
    size_t FolderPosition = DirectoryPath.find_first_of('/', 0) + 1;
    while ((FolderPosition = DirectoryPath.find_first_of('/', FolderPosition)) != DirectoryPath.npos)
    {
        if (FsLib::DirectoryExists(DirectoryPath.substr(0, FolderPosition)))
        {
            ++FolderPosition;
            continue;
        }
        // Just call create directory with a substring.
        if (!FsLib::CreateDirectory(DirectoryPath.substr(0, FolderPosition)))
        {
            return false;
        }
        ++FolderPosition;
    }
    return true;
}

bool FsLib::DeleteDirectory(const std::string &DirectoryPath)
{
    FsFileSystem *FileSystem = NULL;
    std::array<char, FS_MAX_PATH> Path;
    if (!FsLib::ProcessPath(DirectoryPath, &FileSystem, Path.data(), FS_MAX_PATH))
    {
        return false;
    }

    Result FsError = fsFsDeleteDirectory(FileSystem, Path.data());
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X deleting directory \"%s\".", FsError, DirectoryPath.c_str());
        return false;
    }
    return true;
}

bool FsLib::DeleteDirectoryRecursively(const std::string &DirectoryPath)
{
    FsFileSystem *FileSystem;
    std::array<char, FS_MAX_PATH> Path;
    if (!FsLib::ProcessPath(DirectoryPath, &FileSystem, Path.data(), FS_MAX_PATH))
    {
        return false;
    }

    Result FsError = fsFsDeleteDirectoryRecursively(FileSystem, Path.data());
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X deleting directory \"%s\".", FsError, DirectoryPath.c_str());
        return false;
    }
    return true;
}

bool FsLib::DirectoryExists(const std::string &DirectoryPath)
{
    FsFileSystem *FileSystem = NULL;
    std::array<char, FS_MAX_PATH> Path;
    if (!FsLib::ProcessPath(DirectoryPath, &FileSystem, Path.data(), FS_MAX_PATH))
    {
        return false;
    }

    FsDir DirectoryHandle;
    Result FsError = fsFsOpenDirectory(FileSystem, Path.data(), FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &DirectoryHandle);
    if (R_FAILED(FsError))
    {
        // Directory probably doesn't exist. That was the point of the function, so no error string setting.
        return false;
    }
    // Close handle and return
    fsDirClose(&DirectoryHandle);
    return true;
}

bool FsLib::FileExists(const std::string &FilePath)
{
    FsFileSystem *FileSystem = NULL;
    std::array<char, FS_MAX_PATH> Path;
    if (!FsLib::ProcessPath(FilePath, &FileSystem, Path.data(), FS_MAX_PATH))
    {
        return false;
    }

    FsFile FileHandle;
    Result FsError = fsFsOpenFile(FileSystem, Path.data(), FsOpenMode_Read, &FileHandle);
    if (R_FAILED(FsError))
    {
        return false;
    }
    fsFileClose(&FileHandle);
    return true;
}

bool FsLib::DeleteFile(const std::string &FilePath)
{
    FsFileSystem *FileSystem;
    std::array<char, FS_MAX_PATH> Path;
    if (!FsLib::ProcessPath(FilePath, &FileSystem, Path.data(), FS_MAX_PATH))
    {
        return false;
    }

    Result FsError = fsFsDeleteFile(FileSystem, Path.data());
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X deleting file \"%s\".", FsError, FilePath.c_str());
        return false;
    }
    return true;
}

int64_t FsLib::GetFileSize(const std::string &FilePath)
{
    FsFileSystem *FileSystem = NULL;
    std::array<char, FS_MAX_PATH> Path;
    if (!FsLib::ProcessPath(FilePath, &FileSystem, Path.data(), FS_MAX_PATH))
    {
        return -1;
    }

    FsFile FileHandle;
    Result FsError = fsFsOpenFile(FileSystem, Path.data(), FsOpenMode_Read, &FileHandle);
    if (R_FAILED(FsError))
    {
        return -1;
    }

    int64_t FileSize = 0;
    FsError = fsFileGetSize(&FileHandle, &FileSize);
    if (R_FAILED(FsError))
    {
        return -1;
    }

    fsFileClose(&FileHandle);
    return FileSize;
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
                                              .save_data_type = FsSaveDataType_Account,
                                              .save_data_rank = FsSaveDataRank_Primary,
                                              .save_data_index = 0};

    Result FsError = fsOpenSaveDataFileSystem(&s_DeviceMap[DeviceName], FsSaveDataSpaceId_User, &SaveDataAttributes);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening account save data for 0x%016lX.", FsError, ApplicationID);
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
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening cache save for %016X with index %02d.", FsError, ApplicationID, SaveIndex);
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
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening system BCAT for %016X.", FsError, SystemSaveID);
        return false;
    }

    return true;
}

bool FsLib::CommitDataToFileSystem(const std::string &DeviceName)
{
    if (!DeviceNameIsInUse(DeviceName))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error committing data to \"%s\": Device does not exist.", DeviceName.c_str());
        return false;
    }

    Result FsError = fsFsCommit(&s_DeviceMap[DeviceName]);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X committing data to \"%s\".", FsError, DeviceName.c_str());
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
