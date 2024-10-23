#include "InputFile.hpp"
#include "FsLib.hpp"
#include "String.hpp"
#include <array>
#include <string>

extern std::string g_ErrorString;

FsLib::InputFile::InputFile(const std::string &FilePath)
{
    InputFile::Open(FilePath);
}

void FsLib::InputFile::Open(const std::string &FilePath)
{
    // Just in case this is reused.
    if (m_IsOpen)
    {
        FileBase::Close();
        m_IsOpen = false;
    }

    FsFileSystem *FileSystem;
    std::array<char, FS_MAX_PATH> Path;
    if (!FsLib::ProcessPath(FilePath, &FileSystem, Path.data(), FS_MAX_PATH))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening \"%s\" for reading: Invalid path supplied.", FilePath.c_str());
        return;
    }

    Result FsError = fsFsOpenFile(FileSystem, Path.data(), FsOpenMode_Read, &m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening \"%s\" for reading: 0x%X.", FilePath.c_str(), FsError);
        return;
    }

    FsError = fsFileGetSize(&m_FileHandle, &m_StreamSize);
    if (R_FAILED(FsError))
    {
        FileBase::Close();
        g_ErrorString = FsLib::String::GetFormattedString("Error obtaining file size: 0x%X.", FsError);
        return;
    }
    // Set offset and we're good.
    m_Offset = 0;
    m_IsOpen = true;
}

size_t FsLib::InputFile::Read(void *Buffer, size_t ReadSize)
{
    uint64_t BytesRead = 0;
    Result FsError = fsFileRead(&m_FileHandle, m_Offset, Buffer, ReadSize, FsReadOption_None, &BytesRead);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error reading from file: 0x%X.", FsError);
        return 0;
    }
    m_Offset += BytesRead;
    return BytesRead;
}

bool FsLib::InputFile::ReadLine(std::string &LineOut)
{
    // Clear line first.
    LineOut.clear();

    char CurrentCharacter = 0x00;
    while ((CurrentCharacter = InputFile::GetCharacter()) != -1)
    {
        if (CurrentCharacter == '\n' || CurrentCharacter == '\r')
        {
            return true;
        }
        LineOut += CurrentCharacter;
    }
    return false;
}

char FsLib::InputFile::GetCharacter(void)
{
    // Return -1 if the end of the file was/is hit.
    if (m_Offset >= m_StreamSize)
    {
        return -1;
    }

    uint64_t BytesRead = 0;
    char CharacterRead = 0x00;
    Result FsError = fsFileRead(&m_FileHandle, m_Offset++, &CharacterRead, 1, FsReadOption_None, &BytesRead);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error reading byte from file: 0x%X.", FsError);
        return -1;
    }
    return CharacterRead;
}
