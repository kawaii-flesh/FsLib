#include "FsLib.hpp"
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <switch.h>

namespace
{
    constexpr int VA_BUFFER_SIZE = 0x1000;
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
        return;
    }

    for (int64_t i = 0; i < Dir.GetEntryCount(); i++)
    {
        if (Dir.EntryAtIsDirectory(i))
        {
            Print("DIR %s\n", Dir.GetEntryAt(i).data());
            FsLib::Path NewDirPath = DirectoryPath + Dir.GetEntryAt(i) + "/";
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

    // This is a lazy dangerous way to do this.
    for (int i = 0; i < 37; i++)
    {
        // FsLib will automatically close on a request to open a duplicate handle to the same DeviceName.
        if (!FsLib::OpenBisFileSystem("bis", static_cast<FsBisPartitionId>(i)))
        {
            Print("Error opening Bis %i: %s.\n", i, FsLib::GetErrorString());
            continue;
        }

        Print("Printing Bis Partition ID: %i\n", i);
        FsLib::Directory BisRoot("bis:/");
        for (int64_t i = 0; i < BisRoot.GetEntryCount(); i++)
        {
            const char *EntryName = BisRoot.GetEntryAt(i).data();
            if (BisRoot.EntryAtIsDirectory(i))
            {
                Print("\tDIR %s\n", EntryName);
            }
            else
            {
                Print("\tFIL %s\n", EntryName);
            }
        }
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
