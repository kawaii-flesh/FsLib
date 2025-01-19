#include "fslib.hpp"
#include "string.hpp"

extern std::string g_fslibErrorString;

bool fslib::createFile(const fslib::Path &filePath, uint64_t fileSize)
{
    FS_Archive archive;
    if (!fslib::processDeviceAndPath(filePath, &archive))
    {
        return false;
    }

    Result fsError = FSUSER_CreateFile(archive, filePath.getPath(), 0, fileSize);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error creating file: 0x%08X.", fsError);
        return false;
    }
    return true;
}

bool fslib::fileExists(const fslib::Path &filePath)
{
    FS_Archive archive;
    if (!fslib::processDeviceAndPath(filePath, &archive))
    {
        return false;
    }

    Handle fileHandle;
    Result fsError = FSUSER_OpenFile(&fileHandle, archive, filePath.getPath(), FS_OPEN_READ, 0);
    if (R_FAILED(fsError))
    {
        return false;
    }
    FSFILE_Close(fileHandle);
    return true;
}

bool fslib::getFileSize(const fslib::Path &filePath, uint64_t &fileSizeOut)
{
    FS_Archive archive;
    if (!fslib::processDeviceAndPath(filePath, &archive))
    {
        return false;
    }

    Handle fileHandle;
    Result fsError = FSUSER_OpenFile(&fileHandle, archive, filePath.getPath(), FS_OPEN_READ, 0);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error opening file: 0x%08X.", fsError);
        return false;
    }

    fsError = FSFILE_GetSize(fileHandle, &fileSizeOut);
    if (R_FAILED(fsError))
    {
        FSFILE_Close(fileHandle);
        g_fslibErrorString = string::getFormattedString("Error getting file's size: 0x%08X.", fsError);
        return false;
    }
    FSFILE_Close(fileHandle);
    return true;
}

bool fslib::renameFile(const fslib::Path &oldPath, const fslib::Path &newPath)
{
    FS_Archive archiveA, archiveB;
    if (!fslib::processDeviceAndPath(oldPath, &archiveA) || !fslib::processDeviceAndPath(newPath, &archiveB) || archiveA != archiveB)
    {
        return false;
    }

    // Why does this call take two archives? How can just renaming a file move it to a different physical location?
    Result fsError = FSUSER_RenameFile(archiveA, oldPath.getPath(), archiveB, newPath.getPath());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error renaming file: 0x%08X.", fsError);
        return false;
    }
    return true;
}

bool fslib::deleteFile(const fslib::Path &filePath)
{
    FS_Archive archive;
    if (!fslib::processDeviceAndPath(filePath, &archive))
    {
        return false;
    }

    Result fsError = FSUSER_DeleteFile(archive, filePath.getPath());
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error deleting file: 0x%08X.", fsError);
        return false;
    }
    return true;
}
