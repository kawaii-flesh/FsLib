#pragma once
#include "Path.hpp"

namespace FsLib
{
    // Returns whether file exists or not.
    bool FileExists(const FsLib::Path &FilePath);
    // Attempts to delete file. Returns true on success.
    bool DeleteFile(const FsLib::Path &FilePath);
    // Attempts to get file's size. Returns -1 on failure.
    int64_t GetFileSize(const FsLib::Path &FilePath);
    // Attempts to rename file. Returns true on success.
    bool RenameFile(const FsLib::Path &OldPath, const FsLib::Path &NewPath);
} // namespace FsLib
