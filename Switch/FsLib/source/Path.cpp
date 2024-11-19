#include "Path.hpp"
#include <cstring>
#include <switch.h>

namespace
{
    const char *ForbiddenPathCharacters = "#%&{}\\<>*?$!'\":@+`|=";
}

// This will return where the beginning of a string to append really is.
const char *GetItemBegin(const char *P)
{
    while (*P == '/')
    {
        P++;
    }
    return P;
}

// This is to trim trailing slashes from the end of the path.
size_t GetPathLength(const char *P)
{
    // This is to trim trailing slashes from the path.
    char *PathBegin = std::strchr(P, '/');
    if (!PathBegin)
    {
        // This might be bad...
        return std::strlen(P);
    }

    // Skip over the first slash so it can't be deleted by the loop.
    ++PathBegin;
    size_t PathLength = std::strlen(PathBegin);
    while (PathLength > 0 && PathBegin[PathLength - 1] == '/')
    {
        --PathLength;
    }

    return (PathBegin - P) + PathLength;
}

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
    return m_Path && m_DeviceEnd && std::strlen(m_DeviceEnd + 1) > 0 && std::strpbrk(m_DeviceEnd + 1, ForbiddenPathCharacters) == NULL;
}

FsLib::Path FsLib::Path::SubPath(size_t PathLength) const
{
    if (PathLength >= m_PathLength)
    {
        PathLength = m_PathLength;
    }

    FsLib::Path NewPath;
    if (NewPath.AllocatePath(m_PathSize))
    {
        std::memcpy(NewPath.m_Path, m_Path, PathLength);
        NewPath.m_DeviceEnd = std::strchr(NewPath.m_Path, ':');
        NewPath.m_PathLength = std::strlen(NewPath.m_Path);
    }
    // I don't think returning a blank path is the best idea, but it might be the only option. IsValid should catch this anyway.
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
    return Path::NotFound;
}

size_t FsLib::Path::FindFirstOf(char Character, size_t Begin) const
{
    if (Begin >= m_PathLength)
    {
        return Path::NotFound;
    }

    for (size_t i = Begin; i < m_PathLength; i++)
    {
        if (m_Path[i] == Character)
        {
            return i;
        }
    }
    return Path::NotFound;
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
    return Path::NotFound;
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
    return Path::NotFound;
}

const char *FsLib::Path::CString(void) const
{
    return m_Path;
}

std::string_view FsLib::Path::GetDeviceName(void) const
{
    return std::string_view(m_Path, m_DeviceEnd - m_Path);
}

const char *FsLib::Path::GetPath(void) const
{
    return m_DeviceEnd + 1;
}

size_t FsLib::Path::GetLength(void) const
{
    return m_PathLength;
}

FsLib::Path &FsLib::Path::operator=(const FsLib::Path &P)
{
    if (!Path::AllocatePath(P.m_PathSize))
    {
        return *this;
    }

    // Copy P's data and make m_DeviceEnd point to this instance's m_Path.
    m_PathSize = P.m_PathSize;
    m_PathLength = P.m_PathLength;
    std::memcpy(m_Path, P.m_Path, P.m_PathSize);
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

    // This function will return the length of the path being assigned without trailing slashes being counted.
    size_t PathLength = GetPathLength(P);

    // Copy and set everything up.
    std::memcpy(m_Path, P, PathLength);
    m_DeviceEnd = std::strchr(m_Path, ':');
    m_PathLength = std::strlen(m_Path);

    // Should be good to go.
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

FsLib::Path &FsLib::Path::operator/=(const char *P)
{
    // This will trim beginning and trailing slashes.
    const char *AppendBegin = GetItemBegin(P);
    size_t AppendLength = GetPathLength(AppendBegin);

    // Length check.
    if ((m_PathLength + AppendLength) + 1 >= m_PathSize)
    {
        // I should do something here, but what I don't know what yet.
        return *this;
    }

    // This is to avoid doubling up device slashes.
    if (m_Path[m_PathLength - 1] != '/')
    {
        m_Path[m_PathLength++] = '/';
    }

    std::memcpy(&m_Path[m_PathLength], AppendBegin, AppendLength);

    m_PathLength += AppendLength;

    return *this;
}

FsLib::Path &FsLib::Path::operator/=(const std::string &P)
{
    return *this /= P.c_str();
}

FsLib::Path &FsLib::Path::operator/=(std::string_view P)
{
    return *this /= P.data();
}

FsLib::Path &FsLib::Path::operator/=(const std::filesystem::path &P)
{
    return *this /= P.string().c_str();
}

bool FsLib::Path::AllocatePath(uint16_t PathSize)
{
    Path::FreePath();
    m_Path = new (std::nothrow) char[PathSize];
    if (!m_Path)
    {
        return false;
    }
    std::memset(m_Path, 0x00, PathSize);
    return true;
}

void FsLib::Path::FreePath(void)
{
    if (m_Path)
    {
        delete[] m_Path;
    }
}

FsLib::Path FsLib::operator/(const FsLib::Path &Path1, const char *Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath /= Path2;
    return NewPath;
}

FsLib::Path FsLib::operator/(const FsLib::Path &Path1, const std::string &Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath /= Path2;
    return NewPath;
}

FsLib::Path FsLib::operator/(const FsLib::Path &Path1, std::string_view Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath /= Path2;
    return NewPath;
}

FsLib::Path FsLib::operator/(const FsLib::Path &Path1, const std::filesystem::path &Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath /= Path2;
    return NewPath;
}
