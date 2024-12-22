#include "FsLib.hpp"
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <switch.h>

namespace
{
    constexpr int VA_BUFFER_SIZE = 0x1000;
    constexpr uint64_t TARGET_SYSTEM_SAVE = 0x8000000000000011;
} // namespace

// Feels stupid but needed to get actual output in real time on switch.
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

int main(void)
{
    if (!FsLib::Initialize())
    {
        return -1;
    }

    if (!FsLib::Dev::InitializeSDMC())
    {
        return -2;
    }

    consoleInit(NULL);

    PadState GamePad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&GamePad);

    if (!FsLib::OpenSystemSaveFileSystem("system", TARGET_SYSTEM_SAVE))
    {
        Print("%s\n", FsLib::GetErrorString());
    }

    FsLib::Directory SaveRoot("system:/");
    for (int64_t i = 0; i < SaveRoot.GetEntryCount(); i++)
    {
        Print("%s\n", SaveRoot[i]);
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
