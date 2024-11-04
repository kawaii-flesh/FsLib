#include "FsLib.hpp"
#include <array>
#include <cstdarg>
#include <switch.h>

static constexpr unsigned int VA_BUFFER_SIZE = 0x1000;

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
        return;
    }

    for (int64_t i = 0; i < Dir.GetEntryCount(); i++)
    {
        if (Dir.EntryAtIsDirectory(i))
        {
            Print("DIR %s\n", Dir.GetEntryAt(i).data());
            // To do: Get this sorted so + works the way I want it to. I don't do operators too often.
            FsLib::Path NewDirPath = DirectoryPath;
            NewDirPath += Dir.GetEntryAt(i);
            NewDirPath += "/";

            PrintDir(NewDirPath);
        }
        else
        {
            Print("FIL %s\n", Dir.GetEntryAt(i).data());
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

    FsLib::OutputFile PathTestFile("sdmc:/switch/PathTest.txt", false);
    PathTestFile << "Hello there. Testing FsLib path class." << "\n"
                 << "LOL IT WORKS.\n"
                 << "Sorry I printed your entire SD card to your screen...\n";

    PrintDir("sdmc:/");

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
