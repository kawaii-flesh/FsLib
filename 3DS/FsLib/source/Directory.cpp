#include "Directory.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"
#include <algorithm>
#include <cstring>

// This is only for testing the length of entry filenames.
static size_t UTF16StringLength(const uint16_t *String)
{
    size_t Length = 0;
    while (*String++ != 0)
    {
        Length++;
    }
    return Length + 1; // Forgot null terminator... derp.
}

static bool CompareEntries(const FS_DirectoryEntry &EntryA, const FS_DirectoryEntry &EntryB)
{
    if (EntryA.attributes != EntryB.attributes)
    {
        return EntryA.attributes & FS_ATTRIBUTE_DIRECTORY;
    }

    size_t EntryALength = UTF16StringLength(EntryA.name);
    size_t EntryBLength = UTF16StringLength(EntryB.name);
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

extern std::string g_ErrorString;

FsLib::Directory::Directory(const FsLib::Path &DirectoryPath)
{
    Directory::Open(DirectoryPath);
}

void FsLib::Directory::Open(const FsLib::Path &DirectoryPath)
{
    // Just in case directory is reused.
    m_WasOpened = false;
    m_DirectoryList.clear();

    if (!DirectoryPath.IsValid())
    {
        g_ErrorString = ERROR_INVALID_PATH;
        return;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(DirectoryPath.GetDevice(), &Archive))
    {
        g_ErrorString = ERROR_DEVICE_NOT_FOUND;
        return;
    }

    Result FsError = FSUSER_OpenDirectory(&m_DirectoryHande, Archive, fsMakePath(PATH_UTF16, DirectoryPath.GetPath()));
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening directory: 0x%08X.", FsError);
        return;
    }

    // Switch has a function to fetch entry count. 3DS doesn't, so we have to loop and load one at a time.
    uint32_t EntriesRead = 0;
    FS_DirectoryEntry CurrentEntry;
    while (R_SUCCEEDED(FSDIR_Read(m_DirectoryHande, &EntriesRead, 1, &CurrentEntry)) && EntriesRead == 1)
    {
        m_DirectoryList.push_back(CurrentEntry);
    }
    Directory::Close();
    // This will sort Folder->Alphabetical. This spits tons of warnings from GCC about versions, but it works fine so I'm not going to worry about that.
    std::sort(m_DirectoryList.begin(), m_DirectoryList.end(), CompareEntries);
    m_WasOpened = true;
}

bool FsLib::Directory::IsOpen(void) const
{
    return m_WasOpened;
}

uint32_t FsLib::Directory::GetEntryCount(void) const
{
    return m_DirectoryList.size();
}

bool FsLib::Directory::EntryAtIsDirectory(int Index) const
{
    if (Index < 0 || Index >= static_cast<int>(m_DirectoryList.size()))
    {
        return false;
    }
    return m_DirectoryList[Index].attributes == FS_ATTRIBUTE_DIRECTORY;
}

std::u16string_view FsLib::Directory::GetEntryAt(int Index) const
{
    if (Index < 0 || Index >= static_cast<int>(m_DirectoryList.size()))
    {
        // I'm hoping this works the way I want and string_view.empty is true.
        return std::u16string_view(u"");
    }
    return std::u16string_view(reinterpret_cast<const char16_t *>(m_DirectoryList[Index].name));
}

bool FsLib::Directory::Close(void)
{
    Result FsError = FSDIR_Close(m_DirectoryHande);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error closing directory handle: 0x%08X.", FsError);
        return false;
    }
    return true;
}
