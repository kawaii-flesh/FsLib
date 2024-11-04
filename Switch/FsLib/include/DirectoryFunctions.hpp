#pragma once
#include "Path.hpp"

namespace FsLib
{
    // These are shortcut functions for working with directories.
    // Creates a directory. Returns false on failure.
    bool CreateDirectory(const FsLib::Path &DirectoryPath);
    // Tries to create all directories in path. Returns false on failure. Path must end with a trailing slash for this to work correctly.
    bool CreateDirectoryRecursively(const FsLib::Path &DirectoryPath);
    // Tries to delete the directory. Returns false on failure.
    bool DeleteDirectory(const FsLib::Path &DirectoryPath);
    // Tries to recursively delete directory and all contents. Returns false on failure.
    bool DeleteDirectoryRecursively(const FsLib::Path &DirectoryPath);
    // Returns if directory exists. Returns false if it doesn't.
    bool DirectoryExists(const FsLib::Path &DirectoryPath);
    // Tries to rename directory. Returns false on failure.
    bool RenameDirectory(const FsLib::Path &OldPath, const FsLib::Path &NewPath);
} // namespace FsLib
