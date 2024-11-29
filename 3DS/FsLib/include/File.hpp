#pragma once
#include "Path.hpp"
#include <3ds.h>
#include <cstdint>

// This is to make this easier.
static constexpr uint32_t FS_OPEN_APPEND = BIT(3);

namespace FsLib
{
    enum class SeekOrigin
    {
        Beginning,
        Current,
        End
    };

    class File
    {
        public:
            File(void) = default;
            // These call open for you.
            File(const FsLib::Path &FilePath, uint32_t OpenFlags, uint64_t FileSize = 0);
            // This calls close.
            ~File();
            // Opens the file handle with mode and file size passed. Is open can be checked to see if it was successful.
            void Open(const FsLib::Path &FilePath, uint32_t OpenFlags, uint64_t FileSize = 0);
            // Closes file Handle.
            void Close(void);
            // Returns if file was successfully opened.
            bool IsOpen(void) const;
            // Returns the current offset in file.
            uint64_t Tell(void) const;
            // Returns the size of the file.
            uint64_t GetSize(void) const;
            // Returns whether end of file has been reached.
            bool EndOfFile(void) const;
            /*
                Seeks to position relative to Origin. Origin can be the following:
                    FsLib::SeekOrigin::Beginning
                    FsLib::SeekOrigin::Current
                    FsLib::SeekOrigin::End
                Offset will be checked and corrected if out of bounds.
            */
            void Seek(int64_t Offset, FsLib::SeekOrigin Origin);

            /*
                Reading functions.
            */

            // Attempts to read ReadSize into Buffer. Returns number of bytes read on success, 0 on failure.
            size_t Read(void *Buffer, size_t ReadSize);
            // Attempts to read a line from the file. Returns true on success, false on failure. Line is written to LineOut.
            bool ReadLine(std::string &LineOut);
            // Reads a character from file. Returns -1 on failure.
            signed char GetCharacter(void);

            /*
                Writing functions.
            */

            // Attempts to write Buffer of WriteSize bytes to file. Returns number of bytes written on success, zero on failure.
            size_t Write(const void *Buffer, size_t WriteSize);
            // Attempts to write a formatted string to file. Returns true on success.
            bool Writef(const char *Format, ...);
            // Operators for quick, lazy string writing
            File &operator<<(const char *String);
            File &operator<<(const std::string &String);
            // Writes a single byte to file. Returns true on success.
            bool PutCharacter(char C);
            // Flushes file. Returns true on success.
            bool Flush(void);

        protected:
            // File handle.
            Handle m_FileHandle;
            // Whether or not file is open
            bool m_IsOpen = false;
            // The modes used to open the file.
            uint32_t m_Flags;
            // Offset and size
            int64_t m_Offset, m_FileSize;
            // This can be called to check and correct the offset to a certain degree.
            void EnsureOffsetIsValid(void);
            // Attempts to resize file before write operation.
            bool ResizeIfNeeded(size_t BufferSize);
            // Returns whether or not file is open for reading.
            inline bool IsOpenForReading(void) const
            {
                return m_Flags & FS_OPEN_READ;
            }
            // Returns whether or not file is open for writing.
            inline bool IsOpenForWriting(void) const
            {
                return m_Flags & FS_OPEN_WRITE;
            }
    };
} // namespace FsLib
