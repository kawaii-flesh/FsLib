#include "string.hpp"
#include <array>
#include <cstdarg>

namespace
{
    constexpr int VA_BUFFER_SIZE = 0x1000;
}

std::string fslib::getDeviceFromPath(const std::string &path)
{
    size_t colonPosition = path.find_first_of(':');
    if (colonPosition == path.npos)
    {
        // Just return an empty string.
        return std::string("");
    }
    // Return the substring
    return path.substr(0, colonPosition);
}

std::string fslib::removeDeviceFromPath(const std::string &path)
{
    size_t colonPosition = path.find_first_of(':');
    if (colonPosition == path.npos)
    {
        return std::string("");
    }
    return path.substr(colonPosition + 1, path.length());
}

std::string fslib::getFormattedString(const char *format, ...)
{
    std::array<char, VA_BUFFER_SIZE> vaBuffer;

    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer.data(), VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    return std::string(vaBuffer.data());
}
