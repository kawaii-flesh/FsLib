#include "directory.hpp"
#include "fslib.hpp"
#include "string.hpp"
#include <algorithm>
#include <cstring>

// Sorts by entry type, then alphabetically.
static bool compareEntries(const FsDirectoryEntry &entryA, const FsDirectoryEntry &entryB)
{
    if (entryA.type != entryB.type)
    {
        return entryA.type == FsDirEntryType_Dir;
    }

    size_t entryALength = std::strlen(entryA.name);
    size_t entryBLength = std::strlen(entryB.name);
    // So we don't try to read out of bounds.
    size_t shortestString = entryALength < entryBLength ? entryALength : entryBLength;
    for (size_t i = 0; i < shortestString; i++)
    {
        // Might need to add utf-8 support here since filesystems themselves can handle it. Only the SD card gets weird with it.
        int charA = std::tolower(entryA.name[i]);
        int charB = std::tolower(entryB.name[i]);
        if (charA != charB)
        {
            return charA < charB;
        }
    }
    return false;
}

// fslib global error string.
extern std::string g_ErrorString;

FsLib::Directory::Directory(const std::string &DirectoryPath)
{
    Directory::Open(DirectoryPath);
}

void FsLib::Directory::Open(const std::string &DirectoryPath)
{
    // Make sure this is set to false incase the directory is being reused.
    m_WasRead = false;
    // Dissect the path passed.
    FsFileSystem TargetFileSystem;
    std::string DeviceName = FsLib::String::GetDeviceNameFromPath(DirectoryPath);
    std::string TruePath = FsLib::String::GetTruePathFromPath(DirectoryPath);
    bool FileSystemFound = FsLib::GetFileSystemHandleByDeviceName(DeviceName, TargetFileSystem);
    if (DeviceName.empty() || TruePath.empty() || FileSystemFound == false)
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening directory: Invalid path supplied.");
        return;
    }

    Result FsError = fsFsOpenDirectory(&TargetFileSystem, TruePath.c_str(), FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &m_DirectoryHandle);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening directory.", FsError);
        return;
    }

    FsError = fsDirGetEntryCount(&m_DirectoryHandle, &m_EntryCount);
    if (R_FAILED(FsError))
    {
        Directory::Close();
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X obtaining entry count.", FsError);
        return;
    }

    // Read entries.
    int64_t TotalEntriesRead = 0;
    m_DirectoryList = std::make_unique<FsDirectoryEntry[]>(m_EntryCount); // This should free the previous array on allocation.
    FsError = fsDirRead(&m_DirectoryHandle, &TotalEntriesRead, m_EntryCount, m_DirectoryList.get());
    if (R_FAILED(FsError) || TotalEntriesRead != m_EntryCount)
    {
        Directory::Close();
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X reading entries: %02d/%02d read.", FsError, TotalEntriesRead, m_EntryCount);
        return;
    }
    // Sort the array.
    std::sort(m_DirectoryList.get(), m_DirectoryList.get() + m_EntryCount, compareEntries);
    // Close handle for sure
    Directory::Close();
    // We're good
    m_WasRead = true;
}

bool FsLib::Directory::IsOpen(void) const
{
    return m_WasRead;
}

int64_t FsLib::Directory::GetEntryCount(void) const
{
    return m_EntryCount;
}

std::string FsLib::Directory::GetEntryNameAt(int index) const
{
    if (index >= m_EntryCount)
    {
        return std::string("");
    }
    return std::string(m_DirectoryList[index].name);
}

bool FsLib::Directory::EntryAtIsDirectory(int index) const
{
    if (index >= m_EntryCount)
    {
        return false;
    }
    return m_DirectoryList[index].type == FsDirEntryType_Dir;
}

void FsLib::Directory::Close(void)
{
    fsDirClose(&m_DirectoryHandle);
}
