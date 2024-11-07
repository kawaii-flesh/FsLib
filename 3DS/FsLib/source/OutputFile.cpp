#include "OutputFile.hpp"
#include "ErrorCommon.h"
#include "FsLib.hpp"
#include "String.hpp"
#include <array>
#include <cstdarg>
#include <cstring>

namespace
{
    constexpr int VA_BUFFER_SIZE = 0x1000;
}

extern std::string g_ErrorString;

FsLib::OutputFile::OutputFile(const FsLib::Path &FilePath, bool Append)
{
    OutputFile::Open(FilePath, 0, Append);
}

FsLib::OutputFile::OutputFile(const FsLib::Path &FilePath, uint64_t FileSize)
{
    // This can't be for appending so not going to bother with taking that argument.
    OutputFile::Open(FilePath, FileSize, false);
}

void FsLib::OutputFile::Open(const FsLib::Path &FilePath, uint64_t FileSize, bool Append)
{
    if (m_IsOpen)
    {
        FileBase::Close();
        m_IsOpen = false;
    }

    if (!FilePath.IsValid())
    {
        g_ErrorString = ERROR_INVALID_PATH;
        return;
    }

    FS_Archive Archive;
    if (!FsLib::GetArchiveByDeviceName(FilePath.GetDeviceName(), &Archive))
    {
        g_ErrorString = ERROR_DEVICE_NOT_FOUND;
        return;
    }

    if (Append)
    {
        m_IsOpen = OutputFile::OpenForAppending(Archive, FilePath.GetPathData());
    }
    else
    {
        m_IsOpen = OutputFile::OpenForWriting(Archive, FilePath.GetPathData(), FileSize);
    }
}

size_t FsLib::OutputFile::Write(const void *Buffer, size_t WriteSize)
{
    // This is so we don't need to know file size for sure for extdata etc.
    if (!OutputFile::ResizeIfNeeded(WriteSize))
    {
        return 0;
    }

    uint32_t BytesWritten = 0;
    Result FsError = FSFILE_Write(m_FileHandle, &BytesWritten, m_Offset, Buffer, WriteSize, 0);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error writing to file: 0x%08X.", FsError);
        return 0;
    }
    m_Offset += BytesWritten;
    return BytesWritten;
}

bool FsLib::OutputFile::Writef(const char *Format, ...)
{
    std::array<char, VA_BUFFER_SIZE> VaBuffer;

    std::va_list VaList;
    va_start(VaList, Format);
    vsnprintf(VaBuffer.data(), VA_BUFFER_SIZE, Format, VaList);
    va_end(VaList);

    size_t StringLength = std::strlen(VaBuffer.data());
    return OutputFile::Write(VaBuffer.data(), StringLength) == StringLength;
}

FsLib::OutputFile &FsLib::OutputFile::operator<<(const char *String)
{
    size_t StringLength = std::strlen(String);
    OutputFile::Write(String, StringLength);
    return *this;
}

FsLib::OutputFile &FsLib::OutputFile::operator<<(const std::string &String)
{
    OutputFile::Write(String.c_str(), String.length());
    return *this;
}

bool FsLib::OutputFile::PutCharacter(char C)
{
    return OutputFile::Write(&C, 1) == 1;
}

bool FsLib::OutputFile::Flush(void)
{
    Result FsError = FSFILE_Flush(m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error flushing file: 0x%08X.", FsError);
        return false;
    }
    return true;
}

bool FsLib::OutputFile::OpenForWriting(FS_Archive Archive, const char16_t *FilePath, uint64_t FileSize)
{
    // FS_Path we're working with.
    FS_Path FsPath = fsMakePath(PATH_UTF16, FilePath);

    // This will fail if the file doesn't exist. Hence, no error checking unless it becomes a problem.
    FSUSER_DeleteFile(Archive, FsPath);

    Result FsError = FSUSER_CreateFile(Archive, FsPath, 0, FileSize);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error creating file: 0x%08X.", FsError);
        return false;
    }

    FsError = FSUSER_OpenFile(&m_FileHandle, Archive, FsPath, FS_OPEN_WRITE, 0);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening file for writing: 0x%08X.", FsError);
        return false;
    }

    m_Offset = 0;
    m_FileSize = 0;
    return true;
}

bool FsLib::OutputFile::OpenForAppending(FS_Archive Archive, const char16_t *FilePath)
{
    Result FsError = FSUSER_OpenFile(&m_FileHandle, Archive, fsMakePath(PATH_UTF16, FilePath), FS_OPEN_WRITE, 0);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening file for appending: 0x%08X.", FsError);
        return false;
    }

    FsError = FSFILE_GetSize(m_FileHandle, reinterpret_cast<uint64_t *>(&m_FileSize));
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error obtaining file size: 0x%08X.", FsError);
        return false;
    }

    m_Offset = m_FileSize;
    return true;
}

bool FsLib::OutputFile::ResizeIfNeeded(size_t BufferSize)
{
    size_t WriteSizeRemaining = m_FileSize - m_Offset;

    if (BufferSize > WriteSizeRemaining)
    {
        int64_t NewFileSize = (m_FileSize - WriteSizeRemaining) + BufferSize;
        Result FsError = FSFILE_SetSize(m_FileHandle, static_cast<uint64_t>(NewFileSize));
        if (R_FAILED(FsError))
        {
            g_ErrorString = FsLib::String::GetFormattedString("Error resizing file to fit buffer: 0x%08X.", FsError);
            return false;
        }
    }
    return true;
}
