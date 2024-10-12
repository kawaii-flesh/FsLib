#include "file.hpp"
#include "fslib.hpp"
#include "string.hpp"
#include <cstring>
#include <vector>

// fslib's error string.
extern std::string g_ErrorString;

fslib::file::file(const std::string &filePath, FsOpenMode openMode)
{
    fslib::file::open(filePath, openMode);
}

fslib::file::~file()
{
    fslib::file::close();
}

void fslib::file::operator<<(const char *string)
{
    size_t stringLength = std::strlen(string);
    fslib::file::write(string, stringLength);
}

void fslib::file::operator<<(const std::string &string)
{
    *this << string.c_str();
}

void fslib::file::open(const std::string &filePath, FsOpenMode openMode)
{

    FsFileSystem targetFileSystem;
    std::string deviceName = fslib::getDeviceFromPath(filePath);
    std::string truePath = fslib::removeDeviceFromPath(filePath);
    bool fileSystemFound = fslib::getFileSystemHandleByName(deviceName, targetFileSystem);
    if (deviceName.empty() || truePath.empty() || fileSystemFound == false)
    {
        g_ErrorString = fslib::getFormattedString("Error opening \"%s\": Invalid path supplied or filesystem not opened.", filePath.c_str());
        return;
    }

    switch (openMode)
    {
        case FsOpenMode_Read:
        {
            m_IsOpen = fslib::file::openForReading(targetFileSystem, truePath);
        }
        break;

        case FsOpenMode_Write:
        {
            m_IsOpen = fslib::file::openForWriting(targetFileSystem, truePath);
        }
        break;

        case FsOpenMode_Append:
        {
            m_IsOpen = fslib::file::openForAppending(targetFileSystem, truePath);
        }
        break;

        default:
        {
            m_IsOpen = false;
        }
        break;
    }
}

void fslib::file::close(void)
{
    Result fsError = fsFileFlush(&m_FileHandle);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X flushing file.", fsError);
    }
    fsFileClose(&m_FileHandle);
}

bool fslib::file::isOpen(void) const
{
    return m_IsOpen;
}

bool fslib::file::endOfFile(void) const
{
    // Like this just in case.
    return m_Offset >= m_FileSize;
}

size_t fslib::file::read(void *buffer, size_t readSize)
{
    uint64_t bytesRead = 0;
    Result fsError = fsFileRead(&m_FileHandle, m_Offset, buffer, readSize, 0, &bytesRead);
    if (R_FAILED(fsError) || bytesRead != readSize)
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X reading from file, or read size mismatch.", fsError);
        return bytesRead;
    }

    m_Offset += bytesRead;

    return bytesRead;
}

size_t fslib::file::write(const void *buffer, size_t writeSize)
{
    resizeIfNeeded(writeSize);
    Result fsError = fsFileWrite(&m_FileHandle, m_Offset, buffer, writeSize, 0);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X writing to file.", fsError);
        return 0;
    }
    // There's no way to really check what was written on Switch.
    m_Offset += writeSize;
    return writeSize;
}

char fslib::file::getChar(void)
{
    if (m_Offset >= m_FileSize)
    {
        return -1;
    }

    char returnChar = 0x00;
    uint64_t bytesRead = 0;
    Result fsError = fsFileRead(&m_FileHandle, m_Offset++, &returnChar, 1, 0, &bytesRead);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X reading byte from file.", fsError);
        return 0;
    }
    return returnChar;
}

bool fslib::file::putChar(char c)
{
    // Just incase.
    fslib::file::resizeIfNeeded(1);

    Result fsError = fsFileWrite(&m_FileHandle, m_Offset++, &c, 1, 0);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X writing byte to file.", fsError);
        return false;
    }
    return true;
}

bool fslib::file::getLine(std::string &lineOut)
{
    // Clear line
    lineOut.clear();
    while (m_Offset < m_FileSize)
    {
        // I want to rethink this some time...
        char currentChar = fslib::file::getChar();
        if (currentChar == '\n' || currentChar == '\r')
        {
            // Just assume we've hit the end of the line and return true.
            return true;
        }
        lineOut += currentChar;
    }
    // Assumed end of file is hit.
    return false;
}

bool fslib::file::flush(void)
{
    Result fsError = fsFileFlush(&m_FileHandle);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X flushing file.", fsError);
        return false;
    }
    return true;
}

bool fslib::file::openForReading(FsFileSystem &fileSystem, const std::string &filePath)
{
    Result fsError = fsFsOpenFile(&fileSystem, filePath.c_str(), FsOpenMode_Read, &m_FileHandle);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X opening file for reading.", fsError);
        return false;
    }

    fsError = fsFileGetSize(&m_FileHandle, &m_FileSize);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X obtaining file size.", fsError);
        return false;
    }
    // Set offset to 0.
    m_Offset = 0;
    return true;
}

bool fslib::file::openForWriting(FsFileSystem &fileSystem, const std::string &filePath)
{
    // Start by deleting file if it exists. If this fails, it's not really important. It probably just means the file doesn't exist yet.
    fsFsDeleteFile(&fileSystem, filePath.c_str());

    Result fsError = fsFsCreateFile(&fileSystem, filePath.c_str(), 0, 0);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X opening file for writing.", fsError);
        return false;
    }

    fsError = fsFsOpenFile(&fileSystem, filePath.c_str(), FsOpenMode_Write, &m_FileHandle);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X opening file for writing.", fsError);
        return false;
    }
    // File size and offset are both 0 for this one.
    m_Offset = 0;
    m_FileSize = 0;
    return true;
}

bool fslib::file::openForAppending(FsFileSystem &fileSystem, const std::string &filePath)
{
    Result fsError = fsFsOpenFile(&fileSystem, filePath.c_str(), 0, &m_FileHandle);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X opening file for appending.", fsError);
        return false;
    }

    fsError = fsFileGetSize(&m_FileHandle, &m_FileSize);
    if (R_FAILED(fsError))
    {
        g_ErrorString = fslib::getFormattedString("Error 0x%X obtaining file size.", fsError);
        return false;
    }
    // Both are file's size in this case
    m_Offset = m_FileSize;
    return true;
}

bool fslib::file::resizeIfNeeded(size_t dataSize)
{
    // Basically, if writing dataSize to file starting at m_Offset would be larger than the file is, resize it to fit.
    if (m_FileSize - m_Offset < static_cast<int64_t>(dataSize))
    {
        int64_t newFileSize = (m_FileSize - m_Offset) + dataSize;
        Result fsError = fsFileSetSize(&m_FileHandle, newFileSize);
        if (R_FAILED(fsError))
        {
            g_ErrorString = fslib::getFormattedString("Error 0x%X resizing file.", fsError);
            return false;
        }
        // Update file size.
        m_FileSize = newFileSize;
    }
    return true;
}
