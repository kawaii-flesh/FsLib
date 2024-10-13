#include "file.hpp"
#include "fslib.hpp"
#include "string.hpp"
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
    FsFileSystem TargetFileSystem;
    std::string DeviceName = FsLib::String::GetDeviceNameFromPath(FilePath);
    std::string TruePath = FsLib::String::GetTruePathFromPath(FilePath);
    bool FileSystemFound = FsLib::GetFileSystemHandleByDeviceName(DeviceName, TargetFileSystem);
    if (DeviceName.empty() || TruePath.empty() || FileSystemFound == false)
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error opening file: Invalid Path Supplied.");
        return;
    }

    switch (OpenMode)
    {
        case FsOpenMode_Read:
        {
            m_IsOpen = File::OpenForReading(TargetFileSystem, TruePath);
        }
        break;

        case FsOpenMode_Write:
        {
            m_IsOpen = File::OpenForWriting(TargetFileSystem, TruePath);
        }
        break;

        case FsOpenMode_Append:
        {
            m_IsOpen = File::OpenForAppending(TargetFileSystem, TruePath);
        }
        break;

        default:
        {
            m_IsOpen = File::OpenForReading(TargetFileSystem, TruePath);
        }
        break;
    }
}

void FsLib::File::Close(void)
{
    fsFileClose(&m_FileHandle);
}

bool FsLib::File::IsOpen(void) const
{
    return m_IsOpen;
}

bool FsLib::File::EndOfFile(void) const
{
    // The > is JIC. Shouldn't be able to happen.
    return m_Offset >= m_FileSize;
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
    if (m_OpenMode != FsOpenMode_Read || m_Offset >= m_FileSize)
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

    while (m_Offset < m_FileSize)
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

bool FsLib::File::OpenForReading(FsFileSystem &FileSystem, const std::string &FilePath)
{
    Result FsError = fsFsOpenFile(&FileSystem, FilePath.c_str(), FsOpenMode_Read, &m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening file for reading.", FsError);
        return false;
    }

    FsError = fsFileGetSize(&m_FileHandle, &m_FileSize);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X retrieving file size.", FsError);
        return false;
    }
    // Starting offset in this case is 0.
    m_Offset = 0;

    return true;
}

bool FsLib::File::OpenForWriting(FsFileSystem &FileSystem, const std::string &FilePath)
{
    // Start with deleting file if it already exists.
    fsFsDeleteFile(&FileSystem, FilePath.c_str());

    // Try to recreate it.
    Result FsError = fsFsCreateFile(&FileSystem, FilePath.c_str(), 0, 0);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X creating file.", FsError);
        return false;
    }

    FsError = fsFsOpenFile(&FileSystem, FilePath.c_str(), FsOpenMode_Write, &m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening file for writing.", FsError);
        return false;
    }
    // Offset and Size are both 0 in this case.
    m_Offset = 0;
    m_FileSize = 0;

    return true;
}

bool FsLib::File::OpenForAppending(FsFileSystem &FileSystem, const std::string &FilePath)
{
    Result FsError = fsFsOpenFile(&FileSystem, FilePath.c_str(), FsOpenMode_Append, &m_FileHandle);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X opening file for appending.", FsError);
        return false;
    }

    FsError = fsFileGetSize(&m_FileHandle, &m_FileSize);
    if (R_FAILED(FsError))
    {
        g_ErrorString = FsLib::String::GetFormattedString("Error 0x%X retrieving file size.", FsError);
        return false;
    }
    // This is the same in this case.
    m_Offset = m_FileSize;

    return true;
}

bool FsLib::File::ResizeIfNeeded(size_t BufferSize)
{
    // Get the space remaining between offset and file's end.
    int64_t WriteSizeRemaining = m_FileSize - m_Offset;

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
        m_FileSize = NewFileSize;
    }
    return true;
}
