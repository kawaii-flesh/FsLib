#include "Path.hpp"
#include <cstring>

extern std::string g_ErrorString;

FsLib::Path::Path(const char *P)
{
    *this = P;
}

FsLib::Path::Path(const std::string &P) : Path(P.c_str())
{
}

FsLib::Path::Path(std::string_view P) : Path(P.data())
{
}

FsLib::Path::Path(const std::filesystem::path &P) : Path(P.string().c_str())
{
}

bool FsLib::Path::IsValid(void) const
{
    return std::strlen(m_DeviceName) > 0 && std::strlen(m_PathData) > 0;
}

std::string_view FsLib::Path::GetDeviceName(void) const
{
    return std::string_view(m_DeviceName);
}

const char *FsLib::Path::GetPathData(void) const
{
    return m_PathData;
}

FsLib::Path FsLib::Path::SubPath(size_t Begin, size_t Length) const
{
    if (Begin + Length >= m_PathLength)
    {
        Length = m_PathLength - Begin;
    }

    FsLib::Path Sub;
    // Just memcpy the device name.
    std::memcpy(Sub.m_DeviceName, m_DeviceName, FS_MAX_PATH + 1);
    // Memcpy just the chunk of the path we want
    std::memcpy(Sub.m_PathData, &m_PathData[Begin], Length);
    // This should still be true.
    Sub.m_PathLength = Length;
    // Welp, let's hope it worked.
    return Sub;
}

size_t FsLib::Path::FindFirstOf(char Character) const
{
    for (size_t i = 0; i < m_PathLength; i++)
    {
        if (m_PathData[i] == Character)
        {
            return i;
        }
    }
    return Path::EndPos;
}

size_t FsLib::Path::FindFirstOf(size_t Begin, char Character) const
{
    if (Begin >= m_PathLength)
    {
        return Path::EndPos;
    }

    for (size_t i = Begin; i < m_PathLength; i++)
    {
        if (m_PathData[i] == Character)
        {
            return i;
        }
    }
    return Path::EndPos;
}

size_t FsLib::Path::FindLastOf(char Character) const
{
    for (size_t i = m_PathLength; i > 0; i--)
    {
        if (m_PathData[i] == Character)
        {
            return i;
        }
    }
    return Path::EndPos;
}

size_t FsLib::Path::FindLastOf(size_t Begin, char Character) const
{
    if (Begin > m_PathLength)
    {
        Begin = m_PathLength;
    }

    for (size_t i = Begin; i > 0; i--)
    {
        if (m_PathData[i] == Character)
        {
            return i;
        }
    }
    return Path::EndPos;
}

FsLib::Path &FsLib::Path::operator=(const Path &P)
{
    m_PathLength = P.m_PathLength;
    std::memcpy(m_DeviceName, P.m_DeviceName, FS_MAX_PATH + 1);
    std::memcpy(m_PathData, P.m_PathData, FS_MAX_PATH + 1);
    return *this;
}

FsLib::Path &FsLib::Path::operator=(const char *P)
{
    // Get where the device ends.
    size_t PathLength = std::strlen(P);
    char *DeviceEnd = std::strchr(P, ':');

    // If there is a device, copy it. If not, just treat this as an absolute path.
    if (DeviceEnd)
    {
        std::memset(m_DeviceName, 0x00, FS_MAX_PATH + 1);
        std::memcpy(m_DeviceName, P, DeviceEnd - P);
    }
    std::memset(m_PathData, 0x00, FS_MAX_PATH + 1);
    std::memcpy(m_PathData, DeviceEnd + 1, PathLength - ((DeviceEnd - P) + 1));

    m_PathLength = std::strlen(m_PathData);
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

FsLib::Path FsLib::Path::operator+(const FsLib::Path &P)
{
    FsLib::Path NewPath = *this;
    NewPath += P.GetPathData();
    return NewPath;
}

FsLib::Path FsLib::Path::operator+(const char *P)
{
    FsLib::Path NewPath = *this;
    NewPath += P;
    return NewPath;
}

FsLib::Path FsLib::Path::operator+(const std::string &P)
{
    FsLib::Path NewPath = *this;
    NewPath += P.c_str();
    return NewPath;
}

FsLib::Path FsLib::Path::operator+(std::string_view P)
{
    FsLib::Path NewPath = *this;
    NewPath += P.data();
    return NewPath;
}

FsLib::Path FsLib::Path::operator+(const std::filesystem::path &P)
{
    FsLib::Path NewPath = *this;
    NewPath += P.string().c_str();
    return NewPath;
}

FsLib::Path &FsLib::Path::operator+=(const FsLib::Path &P)
{
    *this += P.GetPathData();
    return *this;
}

FsLib::Path &FsLib::Path::operator+=(const char *P)
{
    size_t PLength = std::strlen(P);
    if (m_PathLength + PLength >= FS_MAX_PATH)
    {
        g_ErrorString = "Path too long to append.";
        return *this;
    }

    std::memcpy(&m_PathData[m_PathLength], P, PLength);
    m_PathLength += PLength;
    return *this;
}

FsLib::Path &FsLib::Path::operator+=(const std::string &P)
{
    *this += P.c_str();
    return *this;
}

FsLib::Path &FsLib::Path::operator+=(std::string_view P)
{
    *this += P.data();
    return *this;
}

FsLib::Path &FsLib::Path::operator+=(const std::filesystem::path &P)
{
    *this += P.string().c_str();
    return *this;
}
