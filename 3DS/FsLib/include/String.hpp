#pragma once
#include <string>

namespace FsLib
{
    namespace String
    {
        /// @brief Used internally to create error strings. This is not exposed to the user of the library.
        /// @param Format Format of strings
        /// @param Arguments Arguments
        /// @return String containing formatted output.
        std::string GetFormattedString(const char *Format, ...);
    } // namespace String
} // namespace FsLib
