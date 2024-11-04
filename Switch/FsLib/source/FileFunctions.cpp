#include "FileFunctions.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"
#include <switch.h>

extern std::string g_ErrorString;

bool FsLib::FileExists(const FsLib::Path &FilePath)
{
    if (!FilePath.IsValid())
    {
        g_ErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(FilePath.GetDeviceName(), &FileSystem))
    {
        g_ErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    FsFile FileHandle;
    Result FsError = fsFsOpenFile(FileSystem, FilePath.GetPathData(), FsOpenMode_Read, &FileHandle);
    if (R_FAILED(FsError))
    {
        return false;
    }
    fsFileClose(&FileHandle);
    return true;
}

bool FsLib::DeleteFile(const FsLib::Path &FilePath)
{
    if (!FilePath.IsValid())
    {
        g_ErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(FilePath.GetDeviceName(), &FileSystem))
    {
        g_ErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = fsFsDeleteFile(FileSystem, FilePath.GetPathData());
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error deleting file: 0x%X.", FsError);
        return false;
    }
    return true;
}

int64_t FsLib::GetFileSize(const FsLib::Path &FilePath)
{
    if (!FilePath.IsValid())
    {
        g_ErrorString = ERROR_INVALID_PATH;
        return -1;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(FilePath.GetDeviceName(), &FileSystem))
    {
        g_ErrorString = ERROR_DEVICE_NOT_FOUND;
        return -1;
    }

    FsFile FileHandle;
    Result FsError = fsFsOpenFile(FileSystem, FilePath.GetPathData(), FsOpenMode_Read, &FileHandle);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening file to get size: 0x%X.", FsError);
        return -1;
    }

    int64_t FileSize = 0;
    FsError = fsFileGetSize(&FileHandle, &FileSize);
    if (R_FAILED(FsError))
    {
        fsFileClose(&FileHandle);
        g_ErrorString = FsLib::String::GetFormattedString("Error getting file size: 0x%X.", FsError);
        return -1;
    }

    fsFileClose(&FileHandle);
    return FileSize;
}

bool FsLib::RenameFile(const FsLib::Path &OldPath, const FsLib::Path &NewPath)
{
    if (!OldPath.IsValid() || !NewPath.IsValid() || OldPath.GetDeviceName() != NewPath.GetDeviceName())
    {
        g_ErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *FileSystem;
    if (!FsLib::GetFileSystemByDeviceName(OldPath.GetDeviceName(), &FileSystem))
    {
        g_ErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = fsFsRenameFile(FileSystem, OldPath.GetPathData(), NewPath.GetPathData());
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error renaming file: 0x%X.", FsError);
        return false;
    }
    return true;
}
