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
            /// @brief Default initializer for FsLib::Directory.
            Directory(void) = default;

            /// @brief Opens the directory at DirectoryPath and gets a listing. IsOpen can be checked to see if this was successful.
            /// @param DirectoryPath Path to directory as either FsLib::Path or UTF-16 formatted string. Ex: u"sdmc:/"
            Directory(const FsLib::Path &DirectoryPath);

            /// @brief Opens the directory at DirectoryPath and gets a listing. IsOpen can be checked to see if this was successful.
            /// @param DirectoryPath Path to directory as either FsLib::Path or UTF-16 formatted string. Ex: u"sdmc:/"
            void Open(const FsLib::Path &DirectoryPath);

            /// @brief Returns whether or not opening the directory and reading its contents was successful.
            /// @return True on success. False on failure.
            bool IsOpen(void) const;

            /// @brief Returns the number of entries succefully read from the directory.
            /// @return Number of entries read from directory.
            uint32_t GetEntryCount(void) const;

            /// @brief Returns whether or not the entry at Index in directory listing is a directory or not.
            /// @param Index Index of entry to check.
            /// @return True if the entry is a directory. False if not or Index is out of bounds.
            bool EntryAtIsDirectory(int Index) const;

            /// @brief Returns Entry at index as a UTF-16 AKA u16_string view.
            /// @param Index Index of entry to retrieve.
            /// @return Entry at index or empty if out of bounds.
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
