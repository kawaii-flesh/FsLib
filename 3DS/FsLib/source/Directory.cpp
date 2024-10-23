#include "Directory.hpp"
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
    return Length;
}

static bool CompareEntries(const FS_DirectoryEntry &EntryA, const FS_DirectoryEntry &EntryB)
{
    if ((EntryA.attributes & FS_ATTRIBUTE_DIRECTORY) != (EntryB.attributes & FS_ATTRIBUTE_DIRECTORY))
    {
        return EntryA.attributes == FS_ATTRIBUTE_DIRECTORY;
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

FsLib::Directory::Directory(const std::u16string &DirectoryPath)
{
    Directory::Open(DirectoryPath);
}

void FsLib::Directory::Open(const std::u16string &DirectoryPath)
{
    // Just in case directory is reused.
    m_WasOpened = false;
    // Same as above
    m_DirectoryList.clear();

    FS_Archive Archive;
    std::u16string PathOut;
    if (!FsLib::ProcessPath(DirectoryPath, &Archive, PathOut))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening directory \"%s\": Invalid path supplied.");
        return;
    }

    Result FsError = FSUSER_OpenDirectory(&m_DirectoryHande, Archive, fsMakePath(PATH_UTF16, PathOut.c_str()));
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening directory \"%s\": 0x%08X.", DirectoryPath.c_str(), FsError);
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
    // This will sort Folder->Alphabetical
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

bool FsLib::Directory::EntryAtIsDirectory(int index) const
{
    return m_DirectoryList[index].attributes == FS_ATTRIBUTE_DIRECTORY;
}

std::string FsLib::Directory::GetEntryNameAtAsUTF8(int index) const
{
    // This one needs to be converted. CTRULib has code for this already, thankfully.
    // To do: This results in messed up filenames. Need to figure that out some time soon.
    size_t NameLength = UTF16StringLength(m_DirectoryList[index].name);
    char UTF8Name[NameLength];
    std::memset(UTF8Name, 0x00, NameLength);
    utf16_to_utf8(reinterpret_cast<uint8_t *>(UTF8Name), m_DirectoryList[index].name, NameLength);

    return std::string(UTF8Name);
}

std::u16string FsLib::Directory::GetEntryNameAtAsUTF16(int index) const
{
    return std::u16string(reinterpret_cast<const char16_t *>(m_DirectoryList[index].name));
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
