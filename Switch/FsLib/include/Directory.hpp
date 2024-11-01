#pragma once
#include <memory>
#include <string>
#include <switch.h>

namespace FsLib
{
    class Directory
    {
        public:
            Directory(void) = default;
            // This constructor just calls Open for you. See that for more information.
            Directory(std::string_view DirectoryPath);
            /*
                Opens directory passed if possible. Entries are sorted Directories -> Files and pseudo-alphabetically.
                Path needs a trailing slash to work correctly. I'll change that later if I feel like it.
            */
            void Open(std::string_view DirectoryPath);
            // Returns whether opening was successful or not.
            bool IsOpen(void) const;
            // Returns number of entries in directory.
            int64_t GetEntryCount(void) const;
            // Returns file size of entry.
            int64_t GetEntrySizeAt(int Index) const;
            // Returns the full path to the entry. Returns an empty string if Index is out of bounds.
            std::string GetEntryPathAt(int Index) const;
            // Returns full file name of entry at index. Returns an empty string if Index is out of bounds.
            std::string GetEntryNameAt(int Index) const;
            // Returns whether entry at index is a directory or not. Returns false if Index is out of bounds.
            bool EntryAtIsDirectory(int Index) const;

        private:
            // Directory Handle/service.
            FsDir m_DirectoryHandle;
            // Directory path
            std::string m_DirectoryPath;
            // Number of entries in directory
            int64_t m_EntryCount = 0;
            // Entries in directory.
            std::unique_ptr<FsDirectoryEntry[]> m_DirectoryList;
            // Whether or not directory was opened and read.
            bool m_WasRead = false;
            // Closes the directory handle. Not needed publicly since directory handle is closed after reading entries.
            void Close(void);
    };
} // namespace FsLib
