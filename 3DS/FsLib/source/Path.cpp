#include "Path.hpp"
#include "String.hpp"
#include <cstring>

FsLib::Path::Path(const FsLib::Path &P)
{
    *this = P;
}

FsLib::Path::Path(const char16_t *P)
{
    *this = P;
}

FsLib::Path::Path(const uint16_t *P)
{
    *this = P;
}

FsLib::Path::Path(const std::u16string &P)
{
    *this = P;
}

FsLib::Path::Path(std::u16string_view P)
{
    *this = P;
}

FsLib::Path::~Path()
{
    Path::FreePath();
}

bool FsLib::Path::IsValid(void) const
{
    return m_Path != nullptr && m_DeviceEnd != nullptr && std::char_traits<char16_t>::length(m_Path) > 0;
}

std::u16string_view FsLib::Path::GetDevice(void) const
{
    return std::u16string_view(m_Path, m_DeviceEnd - m_Path);
}

const char16_t *FsLib::Path::GetPath(void) const
{
    return m_DeviceEnd + 1;
}

FsLib::Path FsLib::Path::SubPath(size_t PathLength) const
{
    FsLib::Path Sub = *this;
    if (PathLength >= m_PathLength)
    {
        // Reassign this to correct path.
        Sub.m_DeviceEnd = std::char_traits<char16_t>::find(Sub.m_Path, m_PathLength, u':');
        return Sub;
    }
    // Only copy the section of the path we want.
    std::memset(Sub.m_Path, 0x00, m_PathSize);
    std::memcpy(Sub.m_Path, m_Path, PathLength);
    // Reassign these to point to the correct path.
    Sub.m_PathLength = std::char_traits<char16_t>::length(Sub.m_Path);
    Sub.m_DeviceEnd = std::char_traits<char16_t>::find(Sub.m_Path, Sub.m_PathLength, u':');
    return Sub;
}

size_t FsLib::Path::FindFirstOf(char16_t Character) const
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

size_t FsLib::Path::FindFirstOf(char16_t Character, size_t Begin) const
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

size_t FsLib::Path::FindLastOf(char16_t Character) const
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

size_t FsLib::Path::FindLastOf(char16_t Character, size_t Begin) const
{
    if (Begin > m_PathLength)
    {
        Begin = m_PathLength;
    }

    for (size_t i = Begin; i > m_PathLength; i--)
    {
        if (m_Path[i] == Character)
        {
            return i;
        }
    }
    return Path::npos;
}

FsLib::Path &FsLib::Path::operator=(const FsLib::Path &P)
{
    if (!Path::AllocatePath(P.m_PathSize))
    {
        // To do: Something with errors.
        return *this;
    }

    // Just copy the path.
    std::memcpy(m_Path, P.m_Path, P.m_PathSize);
    // These need to point to this path instead of the other.
    m_PathLength = std::char_traits<char16_t>::length(m_Path);
    m_DeviceEnd = std::char_traits<char16_t>::find(m_Path, P.m_PathSize, u':');
    m_PathSize = P.m_PathSize;

    return *this;
}

FsLib::Path &FsLib::Path::operator=(const char16_t *P)
{
    // Gotta calculate how much memory to allocate for path.
    size_t PathLength = std::char_traits<char16_t>::length(P);
    m_DeviceEnd = std::char_traits<char16_t>::find(P, PathLength, u':');
    if (!m_DeviceEnd)
    {
        // Should do something here.
        return *this;
    }

    m_PathSize = FsLib::MAX_PATH + ((m_DeviceEnd - P) + 1);
    if (!Path::AllocatePath(m_PathSize))
    {
        return *this;
    }
    // Copy path.
    std::memcpy(m_Path, P, std::char_traits<char16_t>::length(P) * sizeof(char16_t));
    // Need to make sure this all points to this class's stuff now.
    m_DeviceEnd = std::char_traits<char16_t>::find(m_Path, PathLength, u':');
    m_PathLength = std::char_traits<char16_t>::length(m_Path);

    return *this;
}

FsLib::Path &FsLib::Path::operator=(const uint16_t *P)
{
    return *this = reinterpret_cast<const char16_t *>(P);
}

FsLib::Path &FsLib::Path::operator=(const std::u16string &P)
{
    return *this = P.c_str();
}

FsLib::Path &FsLib::Path::operator=(std::u16string_view P)
{
    return *this = P.data();
}

FsLib::Path &FsLib::Path::operator+=(const char16_t *P)
{
    size_t PathLength = std::char_traits<char16_t>::length(P);
    if (m_PathLength + PathLength >= m_PathSize)
    {
        return *this;
    }

    std::memcpy(&m_Path[m_PathLength], P, PathLength * sizeof(char16_t));

    m_PathLength += PathLength;

    return *this;
}

FsLib::Path &FsLib::Path::operator+=(const uint16_t *P)
{
    return *this += reinterpret_cast<const char16_t *>(P);
}

FsLib::Path &FsLib::Path::operator+=(const std::u16string &P)
{
    return *this += P.c_str();
}

FsLib::Path &FsLib::Path::operator+=(std::u16string_view P)
{
    return *this += P.data();
}

bool FsLib::Path::AllocatePath(size_t PathSize)
{
    Path::FreePath();
    m_Path = new (std::nothrow) char16_t[PathSize];
    if (!m_Path)
    {
        return false;
    }
    std::memset(m_Path, 0x00, PathSize * sizeof(char16_t));
    return true;
}

void FsLib::Path::FreePath(void)
{
    if (m_Path)
    {
        delete[] m_Path;
    }
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, const char16_t *Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, const uint16_t *Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, const std::u16string &Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &Path1, std::u16string_view Path2)
{
    FsLib::Path NewPath = Path1;
    NewPath += Path2;
    return NewPath;
}
