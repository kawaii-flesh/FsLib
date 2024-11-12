#include "FsLib.hpp"
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <switch.h>

static constexpr unsigned int VA_BUFFER_SIZE = 0x1000;
static constexpr std::string_view FOLDER_TEST =
    "sdmc:/Really/Long/Stupid/Chain/Of/Folders/For/Testing/Path/SubPath/And/Find/Functions/When/Will/This/End/Who/Knows/Seems/Like/It/Goes/On/"
    "Forever/Thought/That/Was/The/End/Nope/Still/Going/For/Testing/And/Also/To/Be/A/Pain/In/The/Ass/Cause/I/Am/Not/Going/To/Test/The/Delete/"
    "Function/Afterward/lol/";

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

    if (!FsLib::CreateDirectoriesRecursively(FOLDER_TEST))
    {
        Print("Guess Not: %s\n", FsLib::GetErrorString());
    }
    else
    {
        Print("Maybe?");
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
