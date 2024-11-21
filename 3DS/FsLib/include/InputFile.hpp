#pragma once
#include "FileBase.hpp"
#include "Path.hpp"
#include <string>

namespace FsLib
{
    class InputFile : public FsLib::FileBase
    {
        public:
            InputFile(void) = default;
            // Constructor that calls open.
            InputFile(const FsLib::Path &FilePath);
            // Opens file for reading. IsOpen can be used to see if this was successful.
            void Open(const FsLib::Path &FilePath);
            /*
                Attempts to read ReadSize in bytes into buffer. Returns the number of bytes read on success, 0 on failure.
                AccurateRead is optional. When true, AccurateRead meticulously reads the stream byte by byte to ensure certain
                error codes do not corrupt the end result.
            */
            // Attempts to read ReadSize into Buffer. Returns number of bytes read on success, 0 on failure.
            size_t Read(void *Buffer, size_t ReadSize);
            // Attempts to read a line from the file. Returns true on success, false on failure. Line is written to LineOut.
            bool ReadLine(std::string &LineOut);
            // Reads a character from file. Returns -1 on failure.
            signed char GetCharacter(void);
    };
} // namespace FsLib
