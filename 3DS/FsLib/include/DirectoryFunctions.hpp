#pragma once
#include "Path.hpp"

namespace FsLib
{
    // Attempts to open DirectoryPath. Returns true on success, false on failure indicating whether or not it exists.
    bool DirectoryExists(const FsLib::Path &DirectoryPath);
    // Attempts to create DirectoryPath. Returns true on success.
    bool CreateDirectory(const FsLib::Path &DirectoryPath);
    // Attempts to create all directories in path.
    bool CreateDirectoriesRecursively(const FsLib::Path &DirectoryPath);
    // Attempts to delete directory
    bool DeleteDirectory(const FsLib::Path &DirectoryPath);
    // Attempts to recursively delete directory.
    bool DeleteDirectoryRecursively(const FsLib::Path &DirectoryPath);
} // namespace FsLib
