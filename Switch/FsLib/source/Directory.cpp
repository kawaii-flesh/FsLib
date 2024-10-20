#include "Directory.hpp"
#include "FsLib.hpp"
#include "String.hpp"
#include <algorithm>
#include <cstring>

// fslib global error string.
extern std::string g_ErrorString;

// Sorts by entry type, then alphabetically.
static bool CompareEntries(const FsDirectoryEntry &EntryA, const FsDirectoryEntry &EntryB)
{
    if (EntryA.type != EntryB.type)
    {
        return EntryA.type == FsDirEntryType_Dir;
    }

    size_t EntryALength = std::strlen(EntryA.name);
    size_t EntryBLength = std::strlen(EntryB.name);
    size_t ShortestString = EntryALength < EntryBLength ? EntryALength : EntryBLength;
    for (size_t i = 0; i < ShortestString; i++)
    {
        int CharA = std::tolower(EntryA.name[i]);
        int CharB = std::tolower(EntryB.name[i]);
        if (CharA != CharB)
        {
            return CharA < CharB;
        }
    }
    return false;
}
cd

FsLib::Directory::Directory(const std::string &DirectoryPath)
{
    Directory::Open(DirectoryPath);
}

void FsLib::Directory::Open(const std::string &DirectoryPath)
{
    // Make sure this is set to false incase the directory is being reused.
    m_WasRead = false;
    // Dissect the path passed.
    char Path[FS_MAX_PATH];
    FsFileSystem *FileSystem = NULL;
    if (!FsLib::ProcessPath(DirectoryPath, &FileSystem, Path, FS_MAX_PATH))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error processing directory \"%s\": Invalid path supplied.", DirectoryPath.c_str());
        return;
    }

    Result FsError = fsFsOpenDirectory(FileSystem, Path, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, &m_DirectoryHandle);
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
    std::sort(m_DirectoryList.get(), m_DirectoryList.get() + m_EntryCount, CompareEntries);
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

int64_t FsLib::Directory::GetEntrySizeAt(int Index) const
{
    if (Index >= m_EntryCount)
    {
        return 0;
    }
    return m_DirectoryList[Index].file_size;
}

std::string FsLib::Directory::GetEntryNameAt(int Index) const
{
    if (Index >= m_EntryCount)
    {
        return std::string("");
    }
    return std::string(m_DirectoryList[Index].name);
}

bool FsLib::Directory::EntryAtIsDirectory(int Index) const
{
    if (Index >= m_EntryCount)
    {
        return false;
    }
    return m_DirectoryList[Index].type == FsDirEntryType_Dir;
}

void FsLib::Directory::Close(void)
{
    fsDirClose(&m_DirectoryHandle);
}
