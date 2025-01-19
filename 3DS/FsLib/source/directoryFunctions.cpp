#include "fslib.hpp"
#include "string.hpp"
#include <3ds.h>

extern std::string g_fslibErrorString;

bool fslib::directoryExists(const fslib::Path &directoryPath)
{
    FS_Archive archive;
    if (!fslib::processDeviceAndPath(directoryPath, &archive))
    {
        return false;
    }

    Handle directoryHandle;
    Result fsError = FSUSER_OpenDirectory(&directoryHandle, archive, directoryPath.getPath());
    if (R_FAILED(fsError))
    {
        return false;
    }
    FSDIR_Close(directoryHandle);
    return true;
}

bool fslib::createDirectory(const fslib::Path &directoryPath)
{
    FS_Archive archive;
    if (!fslib::processDeviceAndPath(directoryPath, &archive))
    {
        return false;
    }

    Result fsError = FSUSER_CreateDirectory(archive, directoryPath.getPath(), 0);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error creating directory: 0x%08X.", fsError);
        return false;
    }
    return true;
}

bool fslib::createDirectoriesRecursively(const fslib::Path &directoryPath)
{
    size_t slashPosition = directoryPath.findFirstOf(u'/') + 1;
    do
    {
        // Find the next slash
        slashPosition = directoryPath.findFirstOf(u'/', slashPosition);
        // Get the sub path to create
        fslib::Path currentDirectory = directoryPath.subPath(slashPosition);

        // If it doesn't exist, try to create it.
        if (!fslib::directoryExists(currentDirectory) && !fslib::createDirectory(currentDirectory))
        {
            // createDirectory should set the error string.
            return false;
        }
        ++slashPosition;
    } while (slashPosition < directoryPath.getLength());
    return true;
}

bool fslib::renameDirectory(const fslib::Path &oldPath, const fslib::Path &newPath)
{
    // This is going to be a bit different because we're working with two paths. Archives can be compared since they're just uint64_t's underneath.
    FS_Archive archiveA, archiveB;
    if (!fslib::processDeviceAndPath(oldPath, &archiveA) || !fslib::processDeviceAndPath(newPath, &archiveB) || archiveA != archiveB)
    {
        return false;
    }

    // I don't understand why this call takes two different archives. It's not like you can rename a file from a different physical location and make it move that way.
    Result fsError = FSUSER_RenameDirectory(archiveA, oldPath.getPath(), archiveB, newPath.getPath());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error renaming directory: 0x%08X.", fsError);
        return false;
    }
    return true;
}

bool fslib::deleteDirectory(const fslib::Path &directoryPath)
{
    FS_Archive archive;
    if (!fslib::processDeviceAndPath(directoryPath, &archive))
    {
        return false;
    }

    Result fsError = FSUSER_DeleteDirectory(archive, directoryPath.getPath());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error deleting directory: 0x%08X.", fsError);
        return false;
    }
    return true;
}

bool fslib::deleteDirectoryRecursively(const fslib::Path &directoryPath)
{
    // Get directory listing
    fslib::Directory targetDirectory(directoryPath);
    if (!targetDirectory.isOpen())
    {
        // Error should be set.
        return false;
    }

    for (uint32_t i = 0; i < targetDirectory.getCount(); i++)
    {
        fslib::Path newTarget = directoryPath / targetDirectory[i];

        // Debug shit: remove
        char asciiBuffer[fslib::MAX_PATH] = {0};
        utf16_to_utf8(reinterpret_cast<uint8_t *>(asciiBuffer), reinterpret_cast<const uint16_t *>(newTarget.cString()), fslib::MAX_PATH);
        printf("newTarget: %s\n", asciiBuffer);

        if (targetDirectory.isDirectory(i) && !fslib::deleteDirectoryRecursively(newTarget))
        {
            return false;
        }
        else if (!targetDirectory.isDirectory(i) && !fslib::deleteFile(newTarget))
        {
            return false;
        }
    }
    /*
        Make sure we're not trying to delete a device root before returning failure. I think this is what's wrong with Nintendo's implementation
        and why it fails when called on the root.
    */
    char asciiBuffer[fslib::MAX_PATH] = {0};
    utf16_to_utf8(reinterpret_cast<uint8_t *>(asciiBuffer), reinterpret_cast<const uint16_t *>(directoryPath.cString()), fslib::MAX_PATH);
    printf("directoryPath: %s\n", asciiBuffer);

    const char16_t *pathBegin = std::char_traits<char16_t>::find(directoryPath.cString(), directoryPath.getLength(), u'/');
    if (std::char_traits<char16_t>::length(pathBegin) > 1 && !fslib::deleteDirectory(directoryPath))
    {
        return false;
    }
    return true;
}
