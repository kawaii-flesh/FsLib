#include "directory.hpp"
#include "fslib.hpp"
#include "string.hpp"

// fslib global error string.
extern std::string g_ErrorString;

fslib::directory::directory(const std::string &directoryPath)
{
    fslib::directory::open(directoryPath);
}

fslib::directory::~directory()
{
    fslib::directory::close();
}

void fslib::directory::open(const std::string &directoryPath)
{
    // Error being used and clear entry vector
    Result fsError = 0;
    m_DirectoryList.clear();
    // Get filesystem and path without device in the beginning.
    FsFileSystem targetFileSystem;
    std::string deviceName = fslib::getDeviceFromPath(directoryPath);
    std::string truePath = fslib::removeDeviceFromPath(directoryPath);
    bool filesystemMounted = fslib::getFileSystemHandleByName(deviceName, targetFileSystem);
    if (deviceName.empty() || truePath.empty() || filesystemMounted == false)
    {
        // Set error string and bail.
        g_ErrorString = fslib::getFormattedString("Error opening \"%s\". Invalid path or device provided.", directoryPath.c_str());
        return;
    }

    fsError = fsFsOpenDirectory(&targetFileSystem, truePath.c_str(), FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &m_DirectoryHandle);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X opening \"%s\"", fsError, directoryPath.c_str());
        return;
    }

    // Get entry count.
    fsError = fsDirGetEntryCount(&m_DirectoryHandle, &m_EntryCount);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X getting entry count for \"%s\".", fsError, directoryPath.c_str());
        return;
    }
    // Resize vector to fit all entries and try to load them.
    int64_t totalEntries = 0;
    m_DirectoryList.resize(m_EntryCount);
    fsError = fsDirRead(&m_DirectoryHandle, &totalEntries, static_cast<size_t>(m_EntryCount), m_DirectoryList.data());
    if (R_FAILED(fsError) || totalEntries != m_EntryCount)
    {
        g_ErrorString = fslib::getFormattedString("Error reading entries for \"%s\".", directoryPath.c_str());
        return;
    }
    m_IsOpen = true;
}

void fslib::directory::close(void)
{
    if (m_IsOpen)
    {
        fsDirClose(&m_DirectoryHandle);
    }
}

bool fslib::directory::isOpen(void) const
{
    return m_IsOpen;
}

int64_t fslib::directory::getEntryCount(void) const
{
    return m_EntryCount;
}

std::string fslib::directory::getEntryNameAt(int index) const
{
    if (index >= static_cast<int>(m_DirectoryList.size()))
    {
        return std::string("");
    }
    return std::string(m_DirectoryList.at(index).name);
}

bool fslib::directory::entryAtIsDirectory(int index) const
{
    if (index >= static_cast<int>(m_DirectoryList.size()))
    {
        return false;
    }
    return m_DirectoryList.at(index).type == FsDirEntryType_Dir;
}
