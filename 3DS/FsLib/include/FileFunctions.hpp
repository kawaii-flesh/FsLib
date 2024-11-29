#pragma once
#include "Path.hpp"

namespace FsLib
{
    // This tries to create the file at path with size.
    bool CreateFile(const FsLib::Path &FilePath, uint64_t FileSize);
    // Tries to open FilePath for reading. If it fails, returns false.
    bool FileExists(const FsLib::Path &FilePath);
    // Attempts to rename OldPath to NewPath. Device must match for this to work.
    bool RenameFile(const FsLib::Path &OldPath, const FsLib::Path &NewPath);
    // Attempts to delete file.
    bool DeleteFile(const FsLib::Path &FilePath);
} // namespace FsLib
