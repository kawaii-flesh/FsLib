#pragma once
#include <string>

namespace FsLib
{
    namespace String
    {
        // This is for creating error messages.
        std::string GetFormattedString(const char *Format, ...);
    } // namespace String
} // namespace FsLib
