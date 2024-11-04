#pragma once
#include "FileBase.hpp"
#include "Path.hpp"
#include <string>

namespace FsLib
{
    class OutputFile : public FsLib::FileBase
    {
        public:
            OutputFile(void) = default;
            // Constructor calls Open. IsOpen can be checked to see if operation succeeded.
            OutputFile(const FsLib::Path &FilePath, bool Append);
            /*
                Opens file for writing, creating it if it doesn't exist. Append is whether to open for appending.
                IsOpen can be checked to see if operation succeeded.
            */
            void Open(const FsLib::Path &FilePath, bool Append);
            // Attempts to write Buffer of WriteSize to file. Returns WriteSize on success, 0 on failure.
            size_t Write(const void *Buffer, size_t WriteSize);
            // Writes formatted string to file.
            bool Writef(const char *Format, ...);
            // Operator for quick, lazy string writing.
            OutputFile &operator<<(const char *String);
            // Writes a single byte to file.
            bool PutCharacter(char C);
            // Flushs file.
            bool Flush(void);

        private:
            // These functions open the file for whatever mode and return if it was successful
            bool OpenForWriting(FsFileSystem *FileSystem, const FsLib::Path &FilePath);
            bool OpenForAppending(FsFileSystem *FileSystem, const FsLib::Path &FilePath);
            // This functions resizes the file if need be.
            bool ResizeIfNeeded(size_t BufferSize);
    };
} // namespace FsLib
