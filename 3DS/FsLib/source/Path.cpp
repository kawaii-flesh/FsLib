#include "Path.hpp"
#include "String.hpp"
#include <cstring>

extern std::string g_ErrorString;

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
    return m_DeviceName.length() > 0 && m_PathData.length() > 0;
}

std::u16string_view FsLib::Path::GetDeviceName(void) const
{
    return std::u16string_view(m_DeviceName.c_str());
}

const char16_t *FsLib::Path::GetPathData(void) const
{
    return m_PathData.c_str();
}

/*
    INSERT MISSING FUNCTIONS HERE WHEN I CAN.
*/

FsLib::Path &FsLib::Path::operator=(const FsLib::Path &P)
{
    // Get the lengths we need to continue.
    m_DeviceName = P.GetDeviceName();
    m_PathData = P.GetPathData();
    return *this;
}

FsLib::Path &FsLib::Path::operator=(const char16_t *P)
{
    size_t FullPathLength = StrLenUTF16(P);
    const char16_t *DeviceEnd = StrChrUTF16(P, u':');
    if (!DeviceEnd)
    {
        g_ErrorString = "Path either has no device.";
        return *this;
    }

    // Calc length of device name, allocate space for strings.
    size_t DeviceLength = DeviceEnd - P;
    m_DeviceName.assign(P, DeviceLength);
    m_PathData.assign(DeviceEnd + 1, FullPathLength - (DeviceLength + 1));

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

FsLib::Path &FsLib::Path::operator+=(const FsLib::Path &P)
{
    return *this += P.m_PathData;
}

FsLib::Path &FsLib::Path::operator+=(const char16_t *P)
{
    m_PathData += P;
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

FsLib::Path FsLib::operator+(const FsLib::Path &P, const char16_t *P2)
{
    FsLib::Path NewPath = P;
    NewPath += P2;
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &P, const uint16_t *P2)
{
    FsLib::Path NewPath = P + reinterpret_cast<const char16_t *>(P2);
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &P, const std::u16string &P2)
{
    FsLib::Path NewPath = P + P2.c_str();
    return NewPath;
}

FsLib::Path FsLib::operator+(const FsLib::Path &P, std::u16string_view P2)
{
    FsLib::Path NewPath = P + P2.data();
    return NewPath;
}
