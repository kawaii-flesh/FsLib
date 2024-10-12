#pragma once
#include <string>
#include <switch.h>
#include <vector>

namespace fslib
{
    class directory
    {
        public:
            directory(void) = default;
            // Just constructs and calls open for you. isOpen will return whether operation was successful or not.
            directory(const std::string &directoryPath);
            ~directory();
            // Opens directory. isOpen can be checked to see if operation succeeded.
            void open(const std::string &directoryPath);
            // Closes directory handle.
            void close(void);
            // Returns whether opening directory was successful
            bool isOpen(void) const;
            // Returns number of entries for directory.
            int64_t getEntryCount(void) const;
            // Returns name of entry at index.
            std::string getEntryNameAt(int index) const;
            // Returns if entry [index] is a directory or not.
            bool entryAtIsDirectory(int index) const;

        private:
            // Directory Handle/service.
            FsDir m_DirectoryHandle;
            // Number of entries in directory
            int64_t m_EntryCount = 0;
            // Entries in directory.
            std::vector<FsDirectoryEntry> m_DirectoryList;
            // Whether or not directory is open
            bool m_IsOpen = false;
    };
} // namespace fslib
