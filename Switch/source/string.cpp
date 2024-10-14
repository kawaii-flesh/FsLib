#include "string.hpp"
#include <array>
#include <cstdarg>

namespace
{
    constexpr int VA_BUFFER_SIZE = 0x1000;
}

bool FsLib::String::ProcessPathString(const std::string &SourcePath, std::string &DeviceOut, std::string &PathOut)
{
    size_t ColonPosition = SourcePath.find_first_of(':');
    if (ColonPosition == SourcePath.npos)
    {
        return false;
    }
    DeviceOut = SourcePath.substr(0, ColonPosition);
    PathOut = SourcePath.substr(ColonPosition + 1);
    return true;
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
