#pragma once
#include <3ds.h>
#include <string>

static inline FS_Path CreatePath(const char16_t *Path)
{
    return {PATH_UTF16, (std::char_traits<char16_t>::length(Path) * sizeof(char16_t)) + sizeof(char16_t), Path};
}
