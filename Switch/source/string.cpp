#include "string.hpp"
#include <array>
#include <cstdarg>

namespace
{
    constexpr int VA_BUFFER_SIZE = 0x1000;
}

std::string FsLib::String::GetDeviceNameFromPath(const std::string &Path)
{
    size_t ColonPosition = Path.find_first_of(':');
    if (ColonPosition == Path.npos)
    {
        // Just return an empty string.
        return std::string("");
    }
    return Path.substr(0, ColonPosition);
}

std::string FsLib::String::GetTruePathFromPath(const std::string &Path)
{
    size_t ColonPosition = Path.find_first_of(':');
    if (ColonPosition == Path.npos)
    {
        return std::string("");
    }
    return Path.substr(ColonPosition + 1);
}

std::string FsLib::String::GetFormattedString(const char *Format, ...)
{
    std::array<char, VA_BUFFER_SIZE> VaBuffer;

    std::va_list VaList;
    va_start(VaList, Format);
    vsnprintf(VaBuffer.data(), VA_BUFFER_SIZE, Format, VaList);
    va_end(VaList);

    return std::string(VaBuffer.data());
}
