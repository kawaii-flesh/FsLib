#pragma once
#include <string>
#include <switch.h>

namespace FsLib
{
    namespace String
    {
        // Used to grab device name from incoming path strings. Will return empty string on failure.
        std::string GetDeviceNameFromPath(const std::string &Path);
        // Used to remove the device from incoming path strings. Will return empty string on failure.
        std::string GetTruePathFromPath(const std::string &Path);
        // This is for creating error messages.
        std::string GetFormattedString(const char *Format, ...);
    } // namespace String
} // namespace FsLib
