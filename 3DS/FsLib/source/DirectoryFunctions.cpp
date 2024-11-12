#include "DirectoryFunctions.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"
#include <3ds.h>

extern std::string g_ErrorString;

bool FsLib::DirectoryExists(const FsLib::Path &DirectoryPath)
{
    if (!DirectoryPath.IsValid())
    {
        g_ErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(DirectoryPath.GetDevice(), &Archive))
    {
        g_ErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Handle DirectoryHandle;
    Result FsError = FSUSER_OpenDirectory(&DirectoryHandle, Archive, fsMakePath(PATH_UTF16, DirectoryPath.GetPath()));
    if (R_FAILED(FsError))
    {
        return false;
    }
    FSDIR_Close(DirectoryHandle);
    return true;
}

bool FsLib::CreateDirectory(const FsLib::Path &DirectoryPath)
{
    if (!DirectoryPath.IsValid())
    {
        g_ErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(DirectoryPath.GetDevice(), &Archive))
    {
        g_ErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = FSUSER_CreateDirectory(Archive, fsMakePath(PATH_UTF16, DirectoryPath.GetPath()), 0);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error creating directory: 0x%08X.", FsError);
        return false;
    }
    return true;
}

bool FsLib::CreateDirectoriesRecursively(const FsLib::Path &DirectoryPath)
{
    if (!DirectoryPath.IsValid())
    {
        g_ErrorString = ERROR_INVALID_PATH;
        return false;
    }

    size_t Slash = DirectoryPath.FindFirstOf(u'/') + 1;
    while ((Slash = DirectoryPath.FindFirstOf(u'/', Slash)) != DirectoryPath.npos)
    {
        FsLib::Path CurrentDir = DirectoryPath.SubPath(Slash);
        if (!FsLib::DirectoryExists(CurrentDir) && !FsLib::CreateDirectory(CurrentDir))
        {
            return false;
        }
        ++Slash;
    }
    return true;
}

bool FsLib::DeleteDirectory(const FsLib::Path &DirectoryPath)
{
    if (!DirectoryPath.IsValid())
    {
        g_ErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(DirectoryPath.GetDevice(), &Archive))
    {
        g_ErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = FSUSER_DeleteDirectory(Archive, fsMakePath(PATH_UTF16, DirectoryPath.GetPath()));
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error deleting directory: 0x%08X.", FsError);
        return false;
    }

    return true;
}

bool FsLib::DeleteDirectoryRecursively(const FsLib::Path &DirectoryPath)
{
    if (!DirectoryPath.IsValid())
    {
        g_ErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(DirectoryPath.GetDevice(), &Archive))
    {
        g_ErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result FsError = FSUSER_DeleteDirectoryRecursively(Archive, fsMakePath(PATH_UTF16, DirectoryPath.GetPath()));
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error deleting directory recursively: 0x%08X.", FsError);
        return false;
    }
    return true;
}
