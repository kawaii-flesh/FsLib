#include "InputFile.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"
#include <cstring>

extern std::string g_FsLibErrorString;

FsLib::InputFile::InputFile(const FsLib::Path &FilePath)
{
    InputFile::Open(FilePath);
}

void FsLib::InputFile::Open(const FsLib::Path &FilePath)
{
    if (m_IsOpen)
    {
        FileBase::Close();
        m_IsOpen = false;
    }

    if (!FilePath.IsValid())
    {
        g_FsLibErrorString = ERROR_INVALID_PATH;
        return;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(FilePath.GetDevice(), &Archive))
    {
        g_FsLibErrorString = ERROR_DEVICE_NOT_FOUND;
        return;
    }

    Result FsError = FSUSER_OpenFile(&m_FileHandle, Archive, fsMakePath(PATH_UTF16, FilePath.GetPath()), FS_OPEN_READ, 0);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error opening file for reading: 0x%08X", FsError);
        return;
    }

    FsError = FSFILE_GetSize(m_FileHandle, reinterpret_cast<uint64_t *>(&m_FileSize));
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error getting file size: 0x%08X.", FsError);
        FileBase::Close();
        return;
    }

    m_Offset = 0;
    m_IsOpen = true;
}

size_t FsLib::InputFile::Read(void *Buffer, size_t ReadSize)
{
    uint32_t BytesRead = 0;
    Result FsError = FSFILE_Read(m_FileHandle, &BytesRead, static_cast<uint64_t>(m_Offset), Buffer, static_cast<uint32_t>(ReadSize));
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error reading from file: 0x%08X.", FsError);
        BytesRead = m_Offset + ReadSize > m_FileSize ? m_FileSize - m_Offset : ReadSize;
    }
    m_Offset += BytesRead;
    return BytesRead;
}

bool FsLib::InputFile::ReadLine(std::string &LineOut)
{
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

signed char FsLib::InputFile::GetCharacter(void)
{
    if (m_Offset >= m_FileSize)
    {
        return -1;
    }

    uint32_t BytesRead = 0;
    char CharacterRead = 0x00;
    Result FsError = FSFILE_Read(m_FileHandle, &BytesRead, m_Offset++, &CharacterRead, 1);
    if (R_FAILED(FsError))
    {
        g_FsLibErrorString = FsLib::String::GetFormattedString("Error reading byte from file: 0x%08X.", FsError);
        return 0x00;
    }
    return CharacterRead;
}
