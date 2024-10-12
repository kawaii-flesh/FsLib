#pragma once
#include <string>
#include <switch.h>

namespace fslib
{
    // Used to grab device name from incoming path strings. Will return empty string on failure.
    std::string getDeviceFromPath(const std::string &path);
    // Used to remove the device from incoming path strings. Will return empty string on failure.
    std::string removeDeviceFromPath(const std::string &path);
    // This is for creating error messages.
    std::string getFormattedString(const char *format, ...);
} // namespace fslib
