#include "path.hpp"
#include <cstring>
#include <string>
#include <switch.h>

namespace
{
    const char *FORBIDDEN_PATH_CHARACTERS = "<>:\"|?*";
} // namespace

// This will get the trimmed down version of the path with no beginning or trailing slashes. Passing NULL or nullptr to pathBegin will skip trimming the beginning.
static void getTrimmedPath(const char *path, const char **pathBegin, size_t &pathLengthOut)
{
    // Get where the beginning of the path begins after slashes.
    if (pathBegin)
    {
        while (*path == '/')
        {
            ++path;
        }
        *pathBegin = path;
    }

    // Get the length of the path starting at that beginning. Loop backwards until we have
    for (pathLengthOut = std::char_traits<char>::length(path); pathLengthOut > 0; pathLengthOut--)
    {
        if (path[pathLengthOut - 1] != '/')
        {
            break;
        }
    }
}

fslib::Path::Path(const fslib::Path &path)
{
    *this = path;
}

fslib::Path::Path(const char *pathData)
{
    *this = pathData;
}

fslib::Path::Path(const std::string &pathData)
{
    *this = pathData;
}

fslib::Path::Path(std::string_view pathData)
{
    *this = pathData;
}

fslib::Path::Path(const std::filesystem::path &pathData)
{
    *this = pathData;
}

fslib::Path::~Path()
{
    Path::freePath();
}

bool fslib::Path::isValid(void) const
{
    return m_path && m_deviceEnd && std::char_traits<char>::length(m_deviceEnd + 1) > 0 &&
           std::strpbrk(m_deviceEnd + 1, FORBIDDEN_PATH_CHARACTERS) == NULL;
}

fslib::Path fslib::Path::subPath(size_t pathLength) const
{
    if (pathLength > m_pathLength)
    {
        pathLength = m_pathLength;
    }

    fslib::Path newPath;
    if (newPath.allocatePath(m_pathSize))
    {
        // Copy the sub path to newPath's buffer.
        std::memcpy(newPath.m_path, m_path, pathLength);
        // Make sure newPath's data is its own.
        newPath.m_deviceEnd = std::strchr(newPath.m_path, ':');
        newPath.m_pathLength = std::char_traits<char>::length(newPath.m_path);
    }
    // Not sure returning this empty on failure is the best idea, but it might be the only option.
    return newPath;
}

size_t fslib::Path::findFirstOf(char character) const
{
    for (size_t i = 0; i < m_pathLength; i++)
    {
        if (m_path[i] == character)
        {
            return i;
        }
    }
    return Path::notFound;
}

size_t fslib::Path::findFirstOf(char character, size_t begin) const
{
    if (begin >= m_pathLength)
    {
        return Path::notFound;
    }

    for (size_t i = begin; i < m_pathLength; i++)
    {
        if (m_path[i] == character)
        {
            return i;
        }
    }
    return Path::notFound;
}

size_t fslib::Path::findLastOf(char character) const
{
    for (size_t i = m_pathLength; i > 0; i--)
    {
        if (m_path[i] == character)
        {
            return i;
        }
    }
    return Path::notFound;
}

size_t fslib::Path::findLastOf(char character, size_t begin) const
{
    if (begin > m_pathLength)
    {
        begin = m_pathLength;
    }

    for (size_t i = begin; i > 0; i--)
    {
        if (m_path[i] == character)
        {
            return i;
        }
    }
    return Path::notFound;
}

const char *fslib::Path::cString(void) const
{
    return m_path;
}

std::string_view fslib::Path::getDeviceName(void) const
{
    return std::string_view(m_path, m_deviceEnd - m_path);
}

const char *fslib::Path::getPath(void) const
{
    return m_deviceEnd + 1;
}

size_t fslib::Path::getLength(void) const
{
    return m_pathLength;
}

fslib::Path &fslib::Path::operator=(const fslib::Path &path)
{
    if (!Path::allocatePath(path.m_pathSize))
    {
        // Not sure this is the best idea. To do: Throw allocation error or something?
        return *this;
    }

    // Copy path's data and correct m_deviceEnd so it points to this instance's m_Path.
    std::memcpy(m_path, path.m_path, path.m_pathSize);
    m_pathSize = path.m_pathSize;
    m_pathLength = path.m_pathLength;
    m_deviceEnd = std::strchr(m_path, ':');

    return *this;
}

fslib::Path &fslib::Path::operator=(const char *pathData)
{
    // Need to calculate m_path's size since Switch expects FS_MAX_PATH starting from first '/'.
    m_deviceEnd = std::strchr(pathData, ':');
    if (!m_deviceEnd)
    {
        // This means the path is invalid. Should figure something out here.
        return *this;
    }

    // To do: Double check if the + 1 is really needed.
    m_pathSize = FS_MAX_PATH + ((m_deviceEnd - pathData) + 1);
    if (!Path::allocatePath(m_pathSize))
    {
        // To do: Throw some kind of allocation error?
        return *this;
    }

    // Going to use this and ignore where it says the path begins.
    size_t pathLength = 0;
    const char *pathBegin = NULL;
    getTrimmedPath(m_deviceEnd + 1, &pathBegin, pathLength);

    // Copy the device string first.
    std::memcpy(m_path, pathData, (m_deviceEnd - pathData) + 2);
    // Copy the rest of the path beginning where the slashes end.
    std::memcpy(&m_path[std::char_traits<char>::length(m_path)], pathBegin, pathLength);
    // Set the rest
    m_deviceEnd = std::strchr(m_path, ':');
    m_pathLength = std::char_traits<char>::length(m_path);

    // Should be good to go.
    return *this;
}

fslib::Path &fslib::Path::operator=(const std::string &pathData)
{
    return *this = pathData.c_str();
}

fslib::Path &fslib::Path::operator=(std::string_view pathData)
{
    return *this = pathData.data();
}

fslib::Path &fslib::Path::operator=(const std::filesystem::path &pathData)
{
    return *this = pathData.string().c_str();
}

fslib::Path &fslib::Path::operator/=(const char *pathData)
{
    // Get trimmed path without beginning and trailing slashes.
    size_t pathLength = 0;
    const char *pathBegin = nullptr;
    getTrimmedPath(pathData, &pathBegin, pathLength);

    if ((m_pathLength + pathLength) + 1 >= m_pathSize)
    {
        // Something here someday.
        return *this;
    }

    // This is to avoid doubling up slashes after the device root.
    if (m_path[m_pathLength - 1] != '/')
    {
        m_path[m_pathLength++] = '/';
    }

    // Memcpy to the end of the current path starting at pathBegin.
    std::memcpy(&m_path[m_pathLength], pathBegin, pathLength);

    m_pathLength += pathLength;

    return *this;
}

fslib::Path &fslib::Path::operator/=(const std::string &pathData)
{
    return *this /= pathData.c_str();
}

fslib::Path &fslib::Path::operator/=(std::string_view pathData)
{
    return *this /= pathData.data();
}

fslib::Path &fslib::Path::operator/=(const std::filesystem::path &pathData)
{
    return *this /= pathData.string().c_str();
}

fslib::Path &fslib::Path::operator+=(const char *pathData)
{
    // If it's too long to fit into the buffer, bail.
    size_t pathLength = std::char_traits<char>::length(pathData);
    if (m_pathLength + pathLength >= m_pathSize)
    {
        return *this;
    }
    // Memcpy to the end.
    std::memcpy(&m_path[m_pathLength], pathData, pathLength);
    // Should be good. segfault
    return *this;
}

fslib::Path &fslib::Path::operator+=(const std::string &pathData)
{
    return *this += pathData.c_str();
}

fslib::Path &fslib::Path::operator+=(std::string_view pathData)
{
    return *this += pathData.data();
}

fslib::Path &fslib::Path::operator+=(const std::filesystem::path &pathData)
{
    return *this += pathData.string().c_str();
}

bool fslib::Path::allocatePath(uint16_t pathSize)
{
    // Call this first just incase so we don't be leakin all over.
    Path::freePath();

    m_path = new (std::nothrow) char[pathSize];
    if (!m_path)
    {
        return false;
    }
    // memset to all 0x00 just to be safe.
    std::memset(m_path, 0x00, pathSize);
    return true;
}

void fslib::Path::freePath(void)
{
    if (m_path)
    {
        delete[] m_path;
    }
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const char *pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const std::string &pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, std::string_view pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator/(const fslib::Path &pathA, const std::filesystem::path &pathB)
{
    fslib::Path newPath = pathA;
    newPath /= pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const char *pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const std::string &pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, std::string_view pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}

fslib::Path fslib::operator+(const fslib::Path &pathA, const std::filesystem::path &pathB)
{
    fslib::Path newPath = pathA;
    newPath += pathB;
    return newPath;
}
