#include "OutputFile.hpp"
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

FsLib::OutputFile::OutputFile(const std::string &FilePath, bool Append)
{
    OutputFile::Open(FilePath, Append);
}

void FsLib::OutputFile::Open(const std::string &FilePath, bool Append)
{
    if (m_IsOpen)
    {
        FileBase::Close();
        m_IsOpen = false;
    }

    FsFileSystem *FileSystem = NULL;
    std::array<char, FS_MAX_PATH> Path;
    if (!FsLib::ProcessPath(FilePath, &FileSystem, Path.data(), FS_MAX_PATH))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening \"%s\" for writing: Invalid path supplied.", FilePath.c_str());
        return;
    }

    if (Append)
    {
        m_IsOpen = OutputFile::OpenForAppending(FileSystem, Path.data());
    }
    else
    {
        m_IsOpen = OutputFile::OpenForWriting(FileSystem, Path.data());
    }
}

size_t FsLib::OutputFile::Write(const void *Buffer, size_t WriteSize)
{
    if (!OutputFile::ResizeIfNeeded(WriteSize))
    {
        return 0;
    }

    Result FsError = fsFileWrite(&m_FileHandle, m_Offset, Buffer, WriteSize, FsWriteOption_None);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error writing to file: 0x%X.", FsError);
        return 0;
    }
    m_Offset += WriteSize;
    // There is no way to really check if this was 100% successful.
    return WriteSize;
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

void FsLib::OutputFile::operator<<(const char *String)
{
    size_t StringLength = std::strlen(String);
    OutputFile::Write(String, StringLength);
}

bool FsLib::OutputFile::PutCharacter(char C)
{
    return OutputFile::Write(&C, 1) == 1;
}

bool FsLib::OutputFile::Flush(void)
{
    Result FsError = fsFileFlush(&m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error flushing file: 0x%X.", FsError);
        return false;
    }
    return true;
}

bool FsLib::OutputFile::OpenForWriting(FsFileSystem *FileSystem, const char *FilePath)
{
    // Start with deleting file if it already exists. This will fail if it doesn't exist.
    fsFsDeleteFile(FileSystem, FilePath);

    // Try to recreate it.
    Result FsError = fsFsCreateFile(FileSystem, FilePath, 0, 0);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X creating file.", FsError);
        return false;
    }

    FsError = fsFsOpenFile(FileSystem, FilePath, FsOpenMode_Write, &m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening file for writing.", FsError);
        return false;
    }
    // Offset and Size are both 0 in this case.
    m_Offset = 0;
    m_StreamSize = 0;
    return true;
}

bool FsLib::OutputFile::OpenForAppending(FsFileSystem *FileSystem, const char *FilePath)
{
    Result FsError = fsFsOpenFile(FileSystem, FilePath, FsOpenMode_Append, &m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening file for appending.", FsError);
        return false;
    }

    FsError = fsFileGetSize(&m_FileHandle, &m_StreamSize);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X retrieving file size.", FsError);
        return false;
    }
    // This is the same in this case.
    m_Offset = m_StreamSize;
    return true;
}

bool FsLib::OutputFile::ResizeIfNeeded(size_t BufferSize)
{
    // Get the space remaining between offset and file's end.
    int64_t WriteSizeRemaining = m_StreamSize - m_Offset;

    if (WriteSizeRemaining < static_cast<int64_t>(BufferSize))
    {
        int64_t NewFileSize = (m_StreamSize - WriteSizeRemaining) + BufferSize;

        Result FsError = fsFileSetSize(&m_FileHandle, NewFileSize);
        if (R_FAILED(FsError))
        {
            g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X resizing file.", FsError);
            return false;
        }
        // Update file size.
        m_StreamSize = NewFileSize;
    }
    return true;
}
