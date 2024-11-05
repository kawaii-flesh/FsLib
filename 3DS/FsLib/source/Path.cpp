#include "Path.hpp"
#include <cstring>

// I don't feel like writing overloads for uint16_t
template <typename Type>
static size_t StrLenUTF16(const Type *Str)
{
    size_t Length = 0;
    while (*Str++)
    {
        Length++;
    }
    return Length;
}

static const char16_t *StrChrUTF16(const char16_t *Str, char16_t Character)
{
    while (*Str++ != 0x0000)
    {
        if (*Str == Character)
        {
            return Str;
        }
    }
    return NULL;
}

FsLib::Path::Path(const char16_t *P)
{
    *this = P;
}

FsLib::Path::Path(const uint16_t *P)
{
    *this = reinterpret_cast<const char16_t *>(P);
}

FsLib::Path::Path(const std::u16string &P)
{
    *this = P.c_str();
}

FsLib::Path::Path(std::u16string_view P)
{
    *this = P.data();
}

bool FsLib::Path::IsValid(void) const
{
    return StrLenUTF16(m_DeviceName) > 0 && StrLenUTF16(m_PathData) > 0;
}

std::u16string_view FsLib::Path::GetDeviceName(void) const
{
    return std::u16string_view(m_DeviceName);
}

const char16_t *FsLib::Path::GetPathData(void) const
{
    return m_PathData;
}

/*
    INSERT MISSING FUNCTIONS HERE WHEN I CAN.
*/

FsLib::Path &FsLib::Path::operator=(const FsLib::Path &P)
{
    std::memcpy(m_DeviceName, P.m_DeviceName, FSLIB_MAX_PATH * sizeof(char16_t));
    std::memcpy(m_PathData, P.m_PathData, FSLIB_MAX_PATH * sizeof(char16_t));
    m_PathLength = P.m_PathLength;
    return *this;
}

FsLib::Path &FsLib::Path::operator=(const char16_t *P)
{
    size_t PathLength = StrLenUTF16(P);
    const char16_t *DeviceEnd = StrChrUTF16(P, u':');

    if (DeviceEnd)
    {
        std::memset(m_DeviceName, 0x00, FSLIB_MAX_PATH * sizeof(char16_t));
        std::memcpy(m_DeviceName, P, (DeviceEnd - P) * sizeof(char16_t));
    }
    std::memset(m_PathData, 0x00, FSLIB_MAX_PATH * sizeof(char16_t));
    std::memcpy(m_PathData, DeviceEnd + 1, (PathLength - ((DeviceEnd - P) + 1)) * sizeof(char16_t));

    m_PathLength = StrLenUTF16(m_PathData);
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

FsLib::Path FsLib::Path::operator+(const FsLib::Path &P)
{
    FsLib::Path NewPath = *this;
    NewPath += P.m_PathData;
    return NewPath;
}

FsLib::Path FsLib::Path::operator+(const char16_t *P)
{
    FsLib::Path NewPath = *this;
    NewPath += P;
    return NewPath;
}

FsLib::Path FsLib::Path::operator+(const uint16_t *P)
{
    FsLib::Path NewPath = *this;
    NewPath += reinterpret_cast<const char16_t *>(P);
    return NewPath;
}

FsLib::Path FsLib::Path::operator+(const std::u16string &P)
{
    FsLib::Path NewPath = *this;
    NewPath += P.c_str();
    return NewPath;
}

FsLib::Path FsLib::Path::operator+(std::u16string_view P)
{
    FsLib::Path NewPath = *this;
    NewPath += P.data();
    return NewPath;
}

FsLib::Path &FsLib::Path::operator+=(const FsLib::Path &P)
{
    return *this += P.m_PathData;
}

FsLib::Path &FsLib::Path::operator+=(const char16_t *P)
{
    size_t PLength = StrLenUTF16(P);
    if (m_PathLength + PLength >= FSLIB_MAX_PATH)
    {
        return *this;
    }
    std::memcpy(&m_PathData[m_PathLength], P, PLength * sizeof(char16_t));
    m_PathLength += PLength;
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
