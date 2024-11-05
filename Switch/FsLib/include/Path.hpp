#pragma once
#include <filesystem>
#include <string>
#include <switch.h>

/*
    The Switch always expects a path FS_MAX_PATH in length and throws 0xD401 if the path buffer is too small? I got tired of other ways.
    If std::string is too short and you pass .c_str(), it throws it, string_view is pretty much the same.
    This has operators for C/C++ string and path types so it's not a pain to work with.
*/

namespace FsLib
{
    class Path
    {
        public:
            Path(void) = default;
            Path(const char *P);
            Path(const std::string &P);
            Path(std::string_view P);
            Path(const std::filesystem::path &P);

            // Returns whether path is a valid path for use with FsLib & Switch filesystem.
            bool IsValid(void) const;
            // Returns the Device name used in the path as a string_view.
            std::string_view GetDeviceName(void) const;
            // Returns the absolute path as a C string for use with Switch FS functions.
            const char *GetPathData(void) const;
            // Returns a subpath. If length is > current length of path, the rest of the path starting from Begin is used.
            Path SubPath(size_t Begin, size_t Length) const;
            // Searchs the path for the first occurrence of character. Second version starts from Begin to search.
            size_t FindFirstOf(char Character) const;
            size_t FindFirstOf(size_t Begin, char Character) const;
            // Searchs backwards through path for character. Second starts from begin for search.
            size_t FindLastOf(char Character) const;
            size_t FindLastOf(size_t Begin, char Character) const;
            // Might add more, but not really needed at the moment.

            // These are the most important part because they make stuff easier to work with.
            Path &operator=(const Path &P);
            Path &operator=(const char *P);
            Path &operator=(const std::string &P);
            Path &operator=(std::string_view P);
            Path &operator=(const std::filesystem::path &P);

            // To do: These right. Next time, I guess.
            Path operator+(const Path &P, const char *P2);
            Path operator+(const char *P);
            Path operator+(const std::string &P);
            Path operator+(std::string_view P);
            Path operator+(const std::filesystem::path &p);

            Path &operator+=(const Path &P);
            Path &operator+=(const char *P);
            Path &operator+=(const std::string &P);
            Path &operator+=(std::string_view P);
            Path &operator+=(const std::filesystem::path &P);

            // This is for find and stuff.
            static constexpr size_t EndPos = -1;

        private:
            /*
                To do: Decide whether to keep these both on stack or not.
                I can't really predict the size of either besides one needs the be FS_MAX_PATH for sure.
            */
            char m_DeviceName[FS_MAX_PATH + 1];
            char m_PathData[FS_MAX_PATH + 1];
            // Length/current offset in m_PathData.
            size_t m_PathLength = 0;
    };
} // namespace FsLib
