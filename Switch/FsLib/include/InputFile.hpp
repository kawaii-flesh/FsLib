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
            // This contructor calls Open for you.
            InputFile(const FsLib::Path &FilePath);
            // Opens file for reading. IsOpen can be checked to see if operation succeeded.
            void Open(const FsLib::Path &FilePath);
            // Attempts to read ReadSize in bytes to Buffer. Returns number of bytes read on success or 0 on failure.
            size_t Read(void *Buffer, size_t ReadSize);
            // Attempts to read a line from file or until '\n' or '\r' is hit. Returns true on success or false on failure. Line is written to LineOut
            bool ReadLine(std::string &LineOut);
            // Reads a single byte from file. Returns -1 on failure.
            char GetCharacter(void);
    };
} // namespace FsLib
