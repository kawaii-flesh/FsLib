#pragma once
#include "Stream.hpp"
#include <cstdint>
#include <string>
#include <switch.h>

namespace FsLib
{
    // To do: Child classes that separate reading/writing. Not super important, but down the line.
    class File : public FsLib::Stream
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
            // Reads ReadSize in bytes from file to buffer.
            size_t Read(void *Buffer, size_t ReadSize);
            // Reads a line from the file. Returns true on success, false on failure or EOF
            bool ReadLine(std::string &LineOut);
            // Write WriteSize in bytes to file from Buffer
            size_t Write(const void *Buffer, size_t WriteSize);
            // Writes formatted string to file.
            bool Writef(const char *Format, ...);
            // Operator for lazy file writing.
            void operator<<(const char *String);
            // Reads a byte from file
            char GetCharacter(void);
            // Writes a byte to file
            bool PutCharacter(char C);
            // Flushes file
            void Flush(void);

        private:
            // The underlying switch file handle.
            FsFile m_FileHandle;
            // To save open mode to block operations that can't be performed in the current mode.
            uint32_t m_OpenMode;
            // These are the functions called to open files according to mode passed. To keep stuff more readable.
            bool OpenForReading(FsFileSystem *FileSystem, const char *FilePath);
            bool OpenForWriting(FsFileSystem *FileSystem, const char *FilePath);
            bool OpenForAppending(FsFileSystem *FileSystem, const char *FilePath);
            // This function resizes the file according to the size passed. Not needed outside of class.
            bool ResizeIfNeeded(size_t BufferSize);
    };
} // namespace FsLib
