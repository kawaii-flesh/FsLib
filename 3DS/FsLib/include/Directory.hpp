#pragma once
#include "Path.hpp"
#include <3ds.h>
#include <string>
#include <vector>

namespace FsLib
{
    class Directory
    {
        public:
            Directory(void) = default;
            // Just calls the open function for you.
            Directory(const FsLib::Path &DirectoryPath);
            // Opens directory and reads contents. Whether operation was successful or not can be checked with IsOpen()
            void Open(const FsLib::Path &DirectoryPath);
            // Returns whether directory was successfully opened and read or not.
            bool IsOpen(void) const;
            // Returns number of entries in directory.
            uint32_t GetEntryCount(void) const;
            // Returns whether or not the entry at index is a directory.
            bool EntryAtIsDirectory(int Index) const;
            // I've decided to leave converting this to other things up to the user. Returns empty view if index is out of bounds.
            std::u16string_view GetEntryAt(int Index) const;

        private:
            // Directory handle.
            Handle m_DirectoryHande;
            // Whether Open was successful
            bool m_WasOpened = false;
            // Directory entry vector.
            std::vector<FS_DirectoryEntry> m_DirectoryList;
            // Shortcut to close directory handle. Automatically called after read and not needed outside of here.
            bool Close(void);
    };
} // namespace FsLib
