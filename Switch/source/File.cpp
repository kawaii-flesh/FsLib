#include "File.hpp"
#include "FsLib.hpp"
#include "String.hpp"
#include <cstring>

// fslib's error string.
extern std::string g_ErrorString;

FsLib::File::File(const std::string &FilePath, FsOpenMode OpenMode)
{
    File::Open(FilePath, OpenMode);
}

FsLib::File::~File()
{
    File::Close();
}

void FsLib::File::Open(const std::string &FilePath, FsOpenMode OpenMode)
{
    // Make sure this is false incase file is reused.
    m_IsOpen = false;
    // Dissect path.
    FsFileSystem *FileSystem = NULL;
    char Path[FS_MAX_PATH];
    if (!FsLib::ProcessPath(FilePath, FileSystem, Path, FS_MAX_PATH))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error processing \"%s\": Invalid path supplied.", FilePath.c_str());
        return;
    }

    switch (OpenMode)
    {
        case FsOpenMode_Read:
        {
            m_IsOpen = File::OpenForReading(FileSystem, Path);
        }
        break;

        case FsOpenMode_Write:
        {
            m_IsOpen = File::OpenForWriting(FileSystem, Path);
        }
        break;

        case FsOpenMode_Append:
        {
            m_IsOpen = File::OpenForAppending(FileSystem, Path);
        }
        break;

        default:
        {
            m_IsOpen = File::OpenForReading(FileSystem, Path);
        }
        break;
    }
}

void FsLib::File::Close(void)
{
    fsFileClose(&m_FileHandle);
}

size_t FsLib::File::Read(void *Buffer, size_t ReadSize)
{
    // Block read operation from a write mode.
    if (m_OpenMode != FsOpenMode_Read)
    {
        return 0;
    }

    uint64_t BytesRead = 0;
    Result FsError = fsFileRead(&m_FileHandle, m_Offset, Buffer, ReadSize, FsReadOption_None, &BytesRead);
    if (R_FAILED(FsError) || BytesRead != ReadSize)
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X reading from file: %016d/%016d bytes read.", FsError, BytesRead, ReadSize);
        return 0;
    }
    // Update offset
    m_Offset += BytesRead;
    // Return
    return BytesRead;
}

size_t FsLib::File::Write(const void *Buffer, size_t WriteSize)
{
    if (m_OpenMode == FsOpenMode_Read)
    {
        return 0;
    }
    // Resize file if necessary to fit incoming buffer.
    if (File::ResizeIfNeeded(WriteSize) == false)
    {
        return 0;
    }

    Result FsError = fsFileWrite(&m_FileHandle, m_Offset, Buffer, WriteSize, FsWriteOption_None);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X writing to file.", FsError);
        return 0;
    }
    m_Offset += WriteSize;
    // There is no way to really check if this was 100% successful.
    return WriteSize;
}

char FsLib::File::GetCharacter(void)
{
    if (m_OpenMode != FsOpenMode_Read || m_Offset >= m_StreamSize)
    {
        return 0x00;
    }

    uint64_t BytesRead = 0;
    char ReadCharacter = 0x00;
    Result FsError = fsFileRead(&m_FileHandle, m_Offset++, &ReadCharacter, 1, FsReadOption_None, &BytesRead);
    if (R_FAILED(FsError) || BytesRead != 1)
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X reading byte from file.", FsError);
        return 0x00;
    }
    Stream::EnsureOffsetIsValid();
    return ReadCharacter;
}

bool FsLib::File::PutCharacter(char C)
{
    if (m_OpenMode == FsOpenMode_Read)
    {
        return false;
    }

    // Just in case.
    if (File::ResizeIfNeeded(1) == false)
    {
        return false;
    }

    Result FsError = fsFileWrite(&m_FileHandle, m_Offset++, &C, 1, FsWriteOption_None);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X writing byte to file.", FsError);
        return false;
    }
    return true;
}

bool FsLib::File::ReadLine(std::string &LineOut)
{
    if (m_OpenMode != FsOpenMode_Read)
    {
        return false;
    }

    // Clear LineOut before beginning.
    LineOut.clear();

    while (m_Offset < m_StreamSize)
    {
        char CurrentCharacter = File::GetCharacter();
        if (CurrentCharacter == '\n' || CurrentCharacter == '\r')
        {
            // Just assume the end of a line has been hit. Return true.
            return true;
        }
        LineOut += CurrentCharacter;
    }
    // This means the end of the file was hit and no lines, I guess
    return false;
}

void FsLib::File::Flush(void)
{
    fsFileFlush(&m_FileHandle);
}

bool FsLib::File::OpenForReading(FsFileSystem *FileSystem, const char *FilePath)
{
    Result FsError = fsFsOpenFile(FileSystem, FilePath, FsOpenMode_Read, &m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening file for reading.", FsError);
        return false;
    }

    FsError = fsFileGetSize(&m_FileHandle, &m_StreamSize);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X retrieving file size.", FsError);
        return false;
    }
    // Starting offset in this case is 0.
    m_Offset = 0;

    return true;
}

bool FsLib::File::OpenForWriting(FsFileSystem *FileSystem, const char *FilePath)
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

bool FsLib::File::OpenForAppending(FsFileSystem *FileSystem, const char *FilePath)
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

bool FsLib::File::ResizeIfNeeded(size_t BufferSize)
{
    // Get the space remaining between offset and file's end.
    int64_t WriteSizeRemaining = m_StreamSize - m_Offset;

    if (WriteSizeRemaining < static_cast<int64_t>(BufferSize))
    {
        int64_t NewFileSize = WriteSizeRemaining + BufferSize;

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
