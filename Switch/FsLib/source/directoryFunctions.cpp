#include "directoryFunctions.hpp"
#include "errorCommon.h"
#include "fslib.hpp"
#include "string.hpp"
#include <switch.h>

extern std::string g_fslibErrorString;

bool fslib::createDirectory(const fslib::Path &directoryPath)
{
    if (!directoryPath.isValid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::getFileSystemByDeviceName(directoryPath.getDeviceName(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsCreateDirectory(fileSystem, directoryPath.getPath());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error creating directory: 0x%X.", fsError);
        return false;
    }

    return true;
}

bool fslib::createDirectoriesRecursively(const fslib::Path &directoryPath)
{
    size_t slashPosition = directoryPath.findFirstOf('/') + 1;
    do
    {
        // Get next slash position.
        slashPosition = directoryPath.findFirstOf('/', slashPosition);
        // Get subpath up to that slash.
        fslib::Path currentDirectory = directoryPath.subPath(slashPosition);
        // Try to create it, but check to see if it exists first to prevent false failures.
        if (!fslib::directoryExists(currentDirectory) && !fslib::createDirectory(currentDirectory))
        {
            return false;
        }
        ++slashPosition;
    } while (slashPosition < directoryPath.getLength());
    return true;
}

bool fslib::deleteDirectory(const fslib::Path &directoryPath)
{
    if (!directoryPath.isValid())
    {
        g_fslibErrorString = ERROR_INVALID_PATH;
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::getFileSystemByDeviceName(directoryPath.getDeviceName(), &fileSystem))
    {
        g_fslibErrorString = ERROR_DEVICE_NOT_FOUND;
        return false;
    }

    Result fsError = fsFsDeleteDirectory(fileSystem, directoryPath.getPath());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error deleting directory: 0x%X.", fsError);
        return false;
    }
    return true;
}

bool fslib::deleteDirectoryRecursively(const fslib::Path &directoryPath)
{
    fslib::Directory targetDirectory(directoryPath);
    if (!targetDirectory.isOpen())
    {
        g_fslibErrorString = string::getFormattedString("Error deleting directory recursively: %s", g_fslibErrorString.c_str());
        return false;
    }

    for (int64_t i = 0; i < targetDirectory.getEntryCount(); i++)
    {
        fslib::Path targetPath = directoryPath / targetDirectory[i];
        if (targetDirectory.entryAtIsDirectory(i) && !fslib::deleteDirectoryRecursively(targetPath))
        {
            return false;
        }
        else if (!targetDirectory.entryAtIsDirectory(i) && !fslib::deleteFile(targetPath))
        {
            return false;
        }
    }

    // This will prevent this function from trying to delete the root (device:/) and reporting failure. Nintendo's doesn't.
    const char *pathBegin = std::char_traits<char>::find(directoryPath.cString(), directoryPath.getLength(), '/');
    if (std::char_traits<char>::length(pathBegin) > 1 && !fslib::deleteDirectory(directoryPath))
    {
        return false;
    }
    return true;
}

bool fslib::directoryExists(const fslib::Path &directoryPath)
{
    if (!directoryPath.isValid())
    {
        return false;
    }

    FsFileSystem *fileSystem;
    if (!fslib::getFileSystemByDeviceName(directoryPath.getDeviceName(), &fileSystem))
    {
        return false;
    }

    FsDir directoryHandle;
    Result fsError = fsFsOpenDirectory(fileSystem, directoryPath.getPath(), FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &directoryHandle);
    if (R_FAILED(fsError))
    {
        return false;
    }
    fsDirClose(&directoryHandle);
    return true;
}

bool fslib::renameDirectory(const fslib::Path &oldPath, const fslib::Path &newPath)
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

    Result fsError = fsFsRenameDirectory(fileSystem, oldPath.getPath(), newPath.getPath());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error renaming directory: 0x%X.", fsError);
        return false;
    }
    return true;
}
