#include "fileFunctions.hpp"
#include "errorCommon.h"
#include "fslib.hpp"
#include "string.hpp"
#include <switch.h>

extern std::string g_fslibErrorString;

bool fslib::createFile(const fslib::Path &filePath, int64_t fileSize)
{
    if (!filePath.isValid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::getFileSystemByDeviceName(filePath.getDeviceName(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsCreateFile(fileSystem, filePath.getPath(), fileSize, 0);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error creating file: 0x%X.", fsError);
        return false;
    }

    return true;
}

bool fslib::fileExists(const fslib::Path &filePath)
{
    if (!filePath.isValid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::getFileSystemByDeviceName(filePath.getDeviceName(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    FsFile fileHandle;
    Result fsError = fsFsOpenFile(fileSystem, filePath.getPath(), FsOpenMode_Read, &fileHandle);
    if (R_FAILED(fsError))
    {
        return false;
    }
    fsFileClose(&fileHandle);
    return true;
}

bool fslib::deleteFile(const fslib::Path &filePath)
{
    if (!filePath.isValid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::getFileSystemByDeviceName(filePath.getDeviceName(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsDeleteFile(fileSystem, filePath.getPath());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error deleting file: 0x%X.", fsError);
        return false;
    }
    return true;
}

int64_t fslib::getFileSize(const fslib::Path &filePath)
{
    if (!filePath.isValid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return -1;
    }

    FsFileSystem *fileSystem;
    if (!fslib::getFileSystemByDeviceName(filePath.getDeviceName(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return -1;
    }

    FsFile fileHandle;
    Result fsError = fsFsOpenFile(fileSystem, filePath.getPath(), FsOpenMode_Read, &fileHandle);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening file to get size: 0x%X.", fsError);
        return -1;
    }

    int64_t fileSize = 0;
    fsError = fsFileGetSize(&fileHandle, &fileSize);
    if (R_FAILED(fsError))
    {
        fsFileClose(&fileHandle);
        g_fslibErrorString = string::getFormattedString("Error getting file size: 0x%X.", fsError);
        return -1;
    }
    fsFileClose(&fileHandle);
    return fileSize;
}

bool fslib::renameFile(const fslib::Path &oldPath, const fslib::Path &newPath)
{
    if (!oldPath.isValid() || !newPath.isValid() || oldPath.getDeviceName() != newPath.getDeviceName())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::getFileSystemByDeviceName(oldPath.getDeviceName(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsRenameFile(fileSystem, oldPath.getPath(), newPath.getPath());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error renaming file: 0x%X.", fsError);
        return false;
    }
    return true;
}
