#include "FsLib.hpp"
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <switch.h>

namespace
{
    constexpr int VA_BUFFER_SIZE = 0x1000;
    constexpr std::string_view LONG_PATH_OF_DIRS = "sdmc:/This/is/a/really/long/path/of/directories/that/I/need/to/use/to/test/stuff////////";
    constexpr std::string_view JKSV_DIR = "sdmc:/";
} // namespace

// Feels stupid but needed to get actual output in real time
void Print(const char *Format, ...)
{
    std::va_list VaList;
    std::array<char, VA_BUFFER_SIZE> VaBuffer;
    va_start(VaList, Format);
    vsnprintf(VaBuffer.data(), VA_BUFFER_SIZE, Format, VaList);
    va_end(VaList);
    std::printf(VaBuffer.data());
    consoleUpdate(NULL);
}

void PrintDir(const FsLib::Path &DirectoryPath)
{
    FsLib::Directory Dir(DirectoryPath);
    if (!Dir.IsOpen())
    {
        Print("%s\n", FsLib::GetErrorString());
        return;
    }

    for (int64_t i = 0; i < Dir.GetEntryCount(); i++)
    {


        if (Dir.EntryAtIsDirectory(i))
        {
            Print("DIR %s\n", Dir.GetEntryAt(i));
            FsLib::Path NewDirPath = DirectoryPath / Dir.GetEntryAt(i);
            Print("%s\n", NewDirPath.CString());
            PrintDir(NewDirPath);
        }
        else
        {
            Print("FIL %s\n", Dir.GetEntryAt(i));
        }
    }
}

int main(void)
{
    consoleInit(NULL);

    PadState GamePad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&GamePad);

    FsLib::Initialize();

    FsLib::Path BasePath = "sdmc:/";
    /*
        Why would you even do this? This is just a stupid test to make sure this works how I want. I assume most people using C++ wouldn't do
        this to begin with. Then again, most college kids now ChatGPT their way through it...
    */
    FsLib::Path DerivedPath = BasePath / "/Directory/" / "/TestFile.txt";
    Print("Derived Path: %s\n", DerivedPath.CString());

    FsLib::Path LongDirPath = LONG_PATH_OF_DIRS;
    Print("LongPathOfDirs: %s\n", LongDirPath.CString());

    if (!FsLib::CreateDirectoriesRecursively(LONG_PATH_OF_DIRS))
    {
        Print("%s\n", FsLib::GetErrorString());
    }

    Print("Press + to exit.");

    while (true)
    {
        padUpdate(&GamePad);
        if (padGetButtonsDown(&GamePad) & HidNpadButton_Plus)
        {
            break;
        }
        consoleUpdate(NULL);
    }

    FsLib::Exit();
    consoleExit(NULL);
    return 0;
}
