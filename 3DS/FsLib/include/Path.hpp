#pragma once
#include <3ds.h>
#include <cstdint>
#include <memory>
#include <string>

namespace FsLib
{
    /*
        Stealing this from Switch. I'm not aware of a max path value for 3DS.
        I don't feel like constantly allocating memory when I can just memcpy.
    */
    static constexpr size_t MAX_PATH = 0x301;
    class Path
    {
        public:
            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(void) = default;

            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(const Path &P);

            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(const char16_t *P);

            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(const uint16_t *P);

            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(const std::u16string &P);

            /// @brief Creates a new path for use with FsLib
            /// @param P String to assign from.
            Path(std::u16string_view P);

            /// @brief Frees path buffer.
            ~Path();

            /// @brief Performs checks and returns if path is valid for use with FsLib.
            /// @return True if path is valid. False if it is not.
            bool IsValid(void) const;

            /// @brief Returns a sub-path ending at PathLength
            /// @param PathLength Length of subpath to return.
            /// @return Sub-path.
            Path SubPath(size_t PathLength) const;

            /// @brief Searches for first occurrence of Character in Path. Overload starts at Begin.
            /// @param Character Character to search.
            /// @return Position of Character or Path::NotFound on failure.
            size_t FindFirstOf(char16_t Character) const;
            size_t FindFirstOf(char16_t Character, size_t Begin) const;

            /// @brief Searches backwards to find last occurrence of Character in string. Overload starts at begin.
            /// @param Character Character to search for.
            /// @return Position of Character or Path::NotFound on failure.
            size_t FindLastOf(char16_t Character) const;
            size_t FindLastOf(char16_t Character, size_t Begin) const;

            /// @brief Returns the entire path as a C const char16_t* String
            /// @return Pointer to path string buffer.
            const char16_t *CString(void) const;

            /// @brief Returns the device as a UTF-16 u16string_view.
            /// @return Device string.
            std::u16string_view GetDevice(void) const;

            /// @brief Returns file name as u16string_view.
            /// @return File name
            std::u16string_view GetFileName(void) const;

            /// @brief Returns extension of path as u16string_view.
            /// @return Path's extension.
            std::u16string_view GetExtension(void) const;

            /// @brief Returns and FS_Path for use with 3DS FS functions.
            /// @return FS_Path
            FS_Path GetPath(void) const;

            /// @brief Returns length of the entire path string.
            /// @return Length of path string.
            size_t GetLength(void) const;

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param P Path to assign from
            /// @return Reference to current path.
            Path &operator=(const Path &P);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param P Path to assign from
            /// @return Reference to current path.
            Path &operator=(const char16_t *P);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param P Path to assign from
            /// @return Reference to current path.
            Path &operator=(const uint16_t *P);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param P Path to assign from
            /// @return Reference to current path.
            Path &operator=(const std::u16string &P);

            /// @brief Assigns Path from various standard UTF-16 string types.
            /// @param P Path to assign from
            /// @return Reference to current path.
            Path &operator=(std::u16string_view P);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param P String to append.
            /// @return Reference to current Path
            Path &operator/=(const char16_t *P);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param P String to append.
            /// @return Reference to current Path
            Path &operator/=(const uint16_t *P);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param P String to append.
            /// @return Reference to current Path
            Path &operator/=(const std::u16string &P);

            /// @brief Preferred appending operator. Adds / if needed between paths. Also trims slashes from input.
            /// @param P String to append.
            /// @return Reference to current Path
            Path &operator/=(std::u16string_view P);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param P String to append.
            /// @return Reference to current Path.
            Path &operator+=(const char16_t *P);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param P String to append.
            /// @return Reference to current Path.
            Path &operator+=(const uint16_t *P);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param P String to append.
            /// @return Reference to current Path.
            Path &operator+=(const std::u16string &P);

            /// @brief Unchecked appending operator. Input is not checked for validity of string appended.
            /// @param P String to append.
            /// @return Reference to current Path.
            Path &operator+=(std::u16string_view P);

            static constexpr uint16_t NotFound = -1;

        private:
            // First is the pointer to path data, second is pointer to the ':'.
            char16_t *m_Path = nullptr;
            const char16_t *m_DeviceEnd = nullptr;
            // First is the actual size of the path buffer, second is the current, actual length of the path.
            uint16_t m_PathSize = 0;
            uint16_t m_PathLength = 0;
            // This allocates memory to hold the string.
            bool AllocatePath(uint16_t PathSize);
            // This frees it.
            void FreePath(void);
    };
    FsLib::Path operator/(const FsLib::Path &Path1, const char16_t *Path2);
    FsLib::Path operator/(const FsLib::Path &Path1, const uint16_t *Path2);
    FsLib::Path operator/(const FsLib::Path &Path1, const std::u16string &Path2);
    FsLib::Path operator/(const FsLib::Path &Path1, std::u16string_view Path2);
    FsLib::Path operator+(const FsLib::Path &Path1, const char16_t *Path2);
    FsLib::Path operator+(const FsLib::Path &Path1, const uint16_t *Path2);
    FsLib::Path operator+(const FsLib::Path &Path1, const std::u16string &Path2);
    FsLib::Path operator+(const FsLib::Path &Path1, std::u16string_view Path2);
} // namespace FsLib
