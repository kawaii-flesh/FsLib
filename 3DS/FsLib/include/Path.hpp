#pragma once
#include <cstdint>
#include <string>
/*
    We're going to steal this from the Switch.
    I don't really know if 3DS has a maximum path length.
    I don't feel like allocating and reallocating on the heap for paths.
    This should be more than enough.
*/
static constexpr unsigned int FSLIB_MAX_PATH = 0x301;

namespace FsLib
{
    class Path
    {
        public:
            Path(void) = default;
            Path(const char16_t *P);
            Path(const uint16_t *P);
            Path(const std::u16string &P);
            Path(std::u16string_view P);
            // Returns whether path is valid to use with FsLib.
            bool IsValid(void) const;
            // Returns Device Name from path.
            std::u16string_view GetDeviceName(void) const;
            // Returns path data for use with 3DS FS functions.
            const char16_t *GetPathData(void) const;
            // Returns a sub-path
            Path SubPath(size_t Begin, size_t Length) const;
            // Searches path for the first occurrence of character. Overload starts at begin.
            size_t FindFirstOf(char16_t Character) const;
            size_t FindFirstOf(size_t Begin, char16_t Character) const;
            // Searches path backwards for Character. Overload starts at begin.
            size_t FindLastOf(char16_t Character) const;
            size_t FindLastOf(size_t Begin, char16_t Character);

            Path &operator=(const Path &P);
            Path &operator=(const char16_t *P);
            Path &operator=(const uint16_t *P);
            Path &operator=(const std::u16string &P);
            Path &operator=(std::u16string_view P);

            Path operator+(const FsLib::Path &P);
            Path operator+(const char16_t *P);
            Path operator+(const uint16_t *P);
            Path operator+(const std::u16string &P);
            Path operator+(std::u16string_view P);

            Path &operator+=(const Path &P);
            Path &operator+=(const char16_t *P);
            Path &operator+=(const uint16_t *P);
            Path &operator+=(const std::u16string &P);
            Path &operator+=(std::u16string_view P);

        private:
            char16_t m_DeviceName[FSLIB_MAX_PATH];
            char16_t m_PathData[FSLIB_MAX_PATH];
            size_t m_PathLength = 0;
    };
} // namespace FsLib
