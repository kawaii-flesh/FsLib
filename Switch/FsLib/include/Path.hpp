#pragma once
#include <filesystem>
#include <string>

namespace FsLib
{

    class Path
    {
        public:
            Path(void) = default;
            Path(const Path &P);
            Path(const char *P);
            Path(const std::string &P);
            Path(std::string_view P);
            Path(const std::filesystem::path &P);
            ~Path();
            /*
                This returns whether or not the path is valid to use based on 3 conditions:
                    1. The path was properly allocated and m_Path isn't nullptr.
                    2. There was a device found in the path.
                    3. The path length following the device is not empty.
            */
            bool IsValid(void) const;
            // This returns a sub-path ending at Length. If PathLength >= the current, you've defeated the purpose of this.
            Path SubPath(size_t PathLength) const;
            // This searches for the first occurrence of Character in path. Overload starts at begin.
            size_t FindFirstOf(char Character) const;
            size_t FindFirstOf(char Character, size_t Begin) const;
            // This searches backwards for the last occurrence of Character in path. Overload starts at begin.
            size_t FindLastOf(char Character) const;
            size_t FindLastOf(char Character, size_t Begin) const;
            // This returns the device as string view for use with FsLib's device map.
            std::string_view GetDevice(void) const;
            // This returns the path for use with LibNX/Switch FS.
            const char *GetPath(void) const;

            // These operators are the most important part and assign most string types.
            Path &operator=(const Path &P);
            Path &operator=(const char *P);
            Path &operator=(const std::string &P);
            Path &operator=(std::string_view P);
            Path &operator=(const std::filesystem::path &P);
            // These append most string types.
            Path &operator+=(const char *P);
            Path &operator+=(const std::string &P);
            Path &operator+=(std::string_view P);
            Path &operator+=(const std::filesystem::path &P);

            // This was for debugging stuff but I left it here anyways. Really convenient for printing or testing.
            operator const char *(void) const
            {
                return m_Path;
            }

            // This is for returning failure in the find methods.
            static constexpr size_t npos = -1;

        private:
            char *m_Path = nullptr;
            char *m_DeviceEnd = nullptr;
            size_t m_PathSize = 0;
            size_t m_PathLength = 0;
            // This allocates memory for the path.
            bool AllocatePath(size_t PathSize);
            // This frees the path data.
            void FreePath(void);
    };
    FsLib::Path operator+(const FsLib::Path &Path1, const char *Path2);
    FsLib::Path operator+(const FsLib::Path &Path1, const std::string &Path2);
    FsLib::Path operator+(const FsLib::Path &Path1, std::string_view Path2);
    FsLib::Path operator+(const FsLib::Path &Path1, const std::filesystem::path &Path2);
} // namespace FsLib
