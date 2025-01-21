#pragma once
#include "path.hpp"

namespace fslib
{
    /// @brief Attempts to create directory with directoryPath.
    /// @param directoryPath Path to new directory.
    /// @return True on success. False on failure.
    bool createDirectory(const fslib::Path &directoryPath);

    /**
     * @brief Attempts to create all directories in path if possible. Path does not need a trailing slash.
     *
     * @param directoryPath Path of directories.
     * @return True on success. False on failure.
     * @note This can be kind of slow. At the moment, I'm not sure if there's a bug or Switch has some kind of limit on folder depth, but
     * after a certain point this will fail. Use only where <b>absolutely needed</b>.
     */
    bool createDirectoriesRecursively(const fslib::Path &directoryPath);

    /// @brief Attempts to delete the directory passed.
    /// @param directoryPath Path to the target directory.
    /// @return True on success. False on failure.
    bool deleteDirectory(const fslib::Path &directoryPath);

    /// @brief Attempts to delete directory path recursively.
    /// @param directoryPath Path to target directory.
    /// @return True on success. False on failure.
    bool deleteDirectoryRecursively(const fslib::Path &directoryPath);

    /// @brief Attempts to open directory for reading to see if it exists. Can also be used to test if something is a directory.
    /// @param directoryPath Path to the target directory.
    /// @return True on success. False on failure.
    bool directoryExists(const fslib::Path &directoryPath);

    /// @brief Attempts to rename OldPath to NewPath.
    /// @param oldPath Original path to target directory.
    /// @param newPath New path to target directory.
    /// @return True on success. False on failure.
    bool renameDirectory(const fslib::Path &oldPath, const fslib::Path &newPath);
} // namespace fslib
