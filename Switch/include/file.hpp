#pragma once
#include <cstdint>
#include <string>
#include <switch.h>

namespace fslib
{
    // To do: Child classes that separate reading/writing. Not super important, but down the line.
    class file
    {
        public:
            file(void) = default;
            // Attempts to open the file at filePath. Calls open. isOpen can be used to tell if operation was successful.
            file(const std::string &filePath, FsOpenMode openMode);
            ~file();
            // Writes string and then flushes the stream.
            void operator<<(const char *string);
            void operator<<(const std::string &string);
            /*
                Attempts to open file at filePath with openMode.
                openMode can be one of the following:
                    FsOpenMode_Read
                    FsOpenMode_Write
                    FsOpenMode_Append
                If modes are or'd together, you're in for a bad time.
                isOpen can be checked to see if the operation was successful.
            */
            void open(const std::string &filePath, FsOpenMode openMode);
            // Flushes and closes file handle.
            void close(void);
            // Returns whether file was opened successfully or not.
            bool isOpen(void) const;
            // Reads readSize of bytes from stream. Returns size that was actually read.
            size_t read(void *buffer, size_t readSize);
            // Writes writeSize from buffer to file. Switch/libnx provides no way to really return what size was written?
            size_t write(const void *buffer, size_t writeSize);

        private:
            // The underlying switch file handle.
            FsFile m_FileHandle;
            // Whether creation/opening succeeded.
            bool m_IsOpen = false;
            // File size and current offset.
            int64_t m_Offset;
            int64_t m_FileSize;
            // These are the functions called to open files according to mode passed. To keep stuff more readable.
            bool openForReading(FsFileSystem &fileSystem, const std::string &filePath);
            bool openForWriting(FsFileSystem &fileSystem, const std::string &filePath);
            bool openForAppending(FsFileSystem &fileSystem, const std::string &filePath);
            // This function resizes the file according to the size passed. Not needed outside of class.
            bool resizeIfNeeded(size_t dataSize);
    };
} // namespace fslib
