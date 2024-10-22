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
            // This constructor just calls Open for you.
            Directory(const std::string &DirectoryPath);
            // Opens directory passed if possible.
            void Open(const std::string &DirectoryPath);
            // Returns whether opening was successful or not.
            bool IsOpen(void) const;
            // Returns number of entries in directory.
            int64_t GetEntryCount(void) const;
            // Returns file size of entry.
            int64_t GetEntrySizeAt(int Index) const;
            // Returns full file name of entry at index.
            std::string GetEntryNameAt(int Index) const;
            // Returns whether entry at index is a directory or not.
            bool EntryAtIsDirectory(int Index) const;

        private:
            // Directory Handle/service.
            FsDir m_DirectoryHandle;
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
