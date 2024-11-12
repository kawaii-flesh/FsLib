#include "Path.hpp"
#include <cstring>
#include <switch.h>

FsLib::Path::Path(const FsLib::Path &P)
{
    *this = P;
}

FsLib::Path::Path(const char *P)
{
    *this = P;
}

FsLib::Path::Path(const std::string &P)
{
    *this = P;
}

FsLib::Path::Path(std::string_view P)
{
    *this = P;
}

FsLib::Path::Path(const std::filesystem::path &P)
{
    *this = P;
}

FsLib::Path::~Path()
{
    Path::FreePath();
}

bool FsLib::Path::IsValid(void) const
{
    return m_Path && m_DeviceEnd && std::strlen(m_DeviceEnd + 1) > 0;
}

FsLib::Path FsLib::Path::SubPath(size_t PathLength) const
{
    if (PathLength >= m_PathLength)
    {
        PathLength = m_PathLength;
    }

    // Only doing this cause it should get us going quicker without as much work.
    FsLib::Path NewPath = *this;
    std::memset(NewPath.m_Path, 0x00, NewPath.m_PathSize);
    std::memcpy(NewPath.m_Path, m_Path, PathLength);
    NewPath.m_DeviceEnd = std::strchr(NewPath.m_Path, ':');
    NewPath.m_PathLength = std::strlen(NewPath.m_Path);
    return NewPath;
}

size_t FsLib::Path::FindFirstOf(char Character) const
{
    for (size_t i = 0; i < m_PathLength; i++)
    {
        if (m_Path[i] == Character)
        {
            return i;
        }
    }
    return Path::npos;
}

size_t FsLib::Path::FindFirstOf(char Character, size_t Begin) const
{
    if (Begin >= m_PathLength)
    {
        return Path::npos;
    }

    for (size_t i = Begin; i < m_PathLength; i++)
    {
        if (m_Path[i] == Character)
        {
            return i;
        }
    }
    return Path::npos;
}

size_t FsLib::Path::FindLastOf(char Character) const
{
    for (size_t i = m_PathLength; i > 0; i--)
    {
        if (m_Path[i] == Character)
        {
            return i;
        }
    }
    return Path::npos;
}

size_t FsLib::Path::FindLastOf(char Character, size_t Begin) const
{
    if (Begin >= m_PathLength)
    {
        Begin = m_PathLength;
    }

    for (size_t i = Begin; i > 0; i--)
    {
        if (m_Path[i] == Character)
        {
            return i;
        }
    }
    return Path::npos;
}

std::string_view FsLib::Path::GetDevice(void) const
{
    return std::string_view(m_Path, m_DeviceEnd - m_Path);
}

const char *FsLib::Path::GetPath(void) const
{
    return m_DeviceEnd + 1;
}

const char *FsLib::Path::GetFullPath(void) const
{
    return m_Path;
}

FsLib::Path &FsLib::Path::operator=(const FsLib::Path &P)
{
    if (!Path::AllocatePath(P.m_PathSize))
    {
        return *this;
    }

    m_PathSize = P.m_PathSize;
    m_PathLength = P.m_PathLength;
    std::memcpy(m_Path, P.m_Path, m_PathSize);

    m_DeviceEnd = std::strchr(m_Path, ':');

    return *this;
}

FsLib::Path &FsLib::Path::operator=(const char *P)
{
    /*
        Need to calculate the path's full size since the Switch expects a string FS_MAX_PATH in length starting from the '/'.
    */
    m_DeviceEnd = std::strchr(P, ':');
    if (!m_DeviceEnd)
    {
        // Should do something here...
        return *this;
    }
    m_PathSize = FS_MAX_PATH + ((m_DeviceEnd - P) + 1);

    if (!Path::AllocatePath(m_PathSize))
    {
        return *this;
    }

    std::memcpy(m_Path, P, std::strlen(P));

    m_DeviceEnd = std::strchr(m_Path, ':');
    m_PathLength = std::strlen(m_Path);

    return *this;
}

FsLib::Path &FsLib::Path::operator=(const std::string &P)
{
    return *this = P.c_str();
}

FsLib::Path &FsLib::Path::operator=(std::string_view P)
{
    return *this = P.data();
}

FsLib::Path &FsLib::Path::operator=(const std::filesystem::path &P)
{
    return *this = P.string().c_str();
}

FsLib::Path &FsLib::Path::operator+=(const char *P)
{
    size_t StringLength = std::strlen(P);
    if (m_PathLength + StringLength >= m_PathSize)
    {
        return *this;
    }

    std::memcpy(&m_Path[m_PathLength], P, StringLength);

    m_PathLength += StringLength;

    return *this;
}

FsLib::Path &FsLib::Path::operator+=(const std::string &P)
{
    return *this += P.c_str();
}

FsLib::Path &FsLib::Path::operator+=(std::string_view P)
{
    return *this += P.data();
}

FsLib::Path &FsLib::Path::operator+=(const std::filesystem::path &P)
{
    return *this += P.string().c_str();
}

bool FsLib::Path::AllocatePath(uint16_t PathSize)
{
    Path::FreePath();
    m_Path = new (std::nothrow) char[PathSize];
    if (!m_Path)
    {
        return false;
    }
    std::memset(m_Path, 0x00, m_PathSize);
    return true;
}

void FsLib::Path::FreePath(void)
{
    if (m_Path)
    {
        delete[] m_Path;
    }
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, const char *Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, const std::string &Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, std::string_view Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, const std::filesystem::path &Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}
