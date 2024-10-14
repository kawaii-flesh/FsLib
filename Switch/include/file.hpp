#pragma once
#include <cstdint>
#include <string>
#include <switch.h>

namespace FsLib
{
    // To do: Child classes that separate reading/writing. Not super important, but down the line.
    class File
    {
        public:
            File(void) = default;
            // Constructor that calls Open for you.
            File(const std::string &FilePath, FsOpenMode OpenMode);
            // Closes file handle on destruction.
            ~File();
            /*
                Attempts to open file at filePath with openMode.
                openMode can be one of the following:
                    FsOpenMode_Read
                    FsOpenMode_Write
                    FsOpenMode_Append
                If modes are or'd together, defaults to reading to prevent data loss.
                isOpen can be checked to see if the operation was successful.
            */
            void Open(const std::string &FilePath, FsOpenMode OpenMode);
            // Flushes and closes file handle.
            void Close(void);
            // Returns whether file was opened successfully or not.
            bool IsOpen(void) const;
            // Returns whether the end of the file has been reached.
            bool EndOfFile(void) const;
            // Reads ReadSize in bytes from file to buffer.
            size_t Read(void *Buffer, size_t ReadSize);
            // Write WriteSize in bytes to file from Buffer
            size_t Write(const void *Buffer, size_t WriteSize);
            // Reads a byte from file
            char GetCharacter(void);
            // Writes a byte to file
            bool PutCharacter(char C);
            // Reads a line from the file. Returns true on success, false on failure or EOF
            bool ReadLine(std::string &LineOut);
            // Flushes file
            void Flush(void);

        private:
            // The underlying switch file handle.
            FsFile m_FileHandle;
            // Whether creation/opening succeeded.
            bool m_IsOpen = false;
            // To save open mode to block operations that can't be performed in the current mode.
            uint32_t m_OpenMode;
            // File size and current offset.
            int64_t m_Offset;
            int64_t m_FileSize;
            // These are the functions called to open files according to mode passed. To keep stuff more readable.
            bool OpenForReading(FsFileSystem *FileSystem, const std::string &FilePath);
            bool OpenForWriting(FsFileSystem *FileSystem, const std::string &FilePath);
            bool OpenForAppending(FsFileSystem *FileSystem, const std::string &FilePath);
            // This function resizes the file according to the size passed. Not needed outside of class.
            bool ResizeIfNeeded(size_t BufferSize);
    };
} // namespace FsLib
