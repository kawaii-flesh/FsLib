#pragma once
#include <cstdint>
#include <memory>
#include <string>

namespace FsLib
{
    class Path
    {
        public:
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

            Path &operator+=(const Path &P);
            Path &operator+=(const char16_t *P);
            Path &operator+=(const uint16_t *P);
            Path &operator+=(const std::u16string &P);
            Path &operator+=(std::u16string_view P);

        private:
            // Normally I avoid new now but it's the only way I see doing this without constantly allocating memory.
            std::u16string m_DeviceName;
            std::u16string m_PathData;
    };
    FsLib::Path operator+(const FsLib::Path &P, const FsLib::Path &P2);
    FsLib::Path operator+(const FsLib::Path &P, const char16_t *P2);
    FsLib::Path operator+(const FsLib::Path &P, const uint16_t *P2);
    FsLib::Path operator+(const FsLib::Path &P, const std::u16string &P2);
    FsLib::Path operator+(const FsLib::Path &P, std::u16string_view P2);
} // namespace FsLib
