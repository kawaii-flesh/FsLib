#pragma once
#include <string>
#include <switch.h>

namespace FsLib
{
    namespace String
    {
        // Processes and splits path at colon and writes device string/path to respective strings passed.
        bool ProcessPathString(const std::string &SourcePath, std::string &DeviceOut, std::string &PathOut);
        // This is for creating error messages.
        std::string GetFormattedString(const char *Format, ...);
    } // namespace String
} // namespace FsLib
