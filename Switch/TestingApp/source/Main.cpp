#include "FsLib.hpp"
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <minizip/zip.h>
#include <switch.h>

static constexpr int FILE_BUFFER_SIZE = 0x100000;
static constexpr int VA_BUFFER_SIZE = 0x1000;

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

int main(void)
{
    std::srand(time(NULL));

    consoleInit(NULL);

    PadState GamePad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&GamePad);

    if (FsLib::Initialize())
    {
        Print("FsLib Initialized.\n");
    }
    else
    {
        return -1;
    }

    // Don't know what else to check for that's always there.
    Print("Checking if \"sdmc:/hbmenu.nro\" exists...");
    if (FsLib::FileExists("sdmc:/hbmenu.nro"))
    {
        Print("It does!\n");
    }
    else
    {
        Print("It doesn't. How are you even running this?\n");
    }


    Print("Checking if \"sdmc:/switch/\" exists... ");
    if (FsLib::DirectoryExists("sdmc:/switch/"))
    {
        Print("It does!\n");
    }
    else
    {
        Print("It doesn't. How are you even running this?\n");
    }

    Print("Creating test directory... ");
    if (FsLib::CreateDirectory("sdmc:/TestDirectory/"))
    {
        Print("Done!.\n");
    }
    else
    {
        Print("Failed.\n");
    }

    Print("Checking if that same directory exists cause who knows? Maybe I'm lying... ");
    if (FsLib::DirectoryExists("sdmc:/TestDirectory/"))
    {
        Print("It does... or does it?\n");
    }
    else
    {
        Print("It doesn't. Definitely doesn't.\n");
    }

    Print("Deleting that same directory so you can't even find out if I'm a big, fat, liar grandpappy of all liars... ");
    if (FsLib::DeleteDirectory("sdmc:/TestDirectory/"))
    {
        Print("Done.\n");
    }
    else
    {
        Print("Uh oh.\n");
    }

    Print("Opening switch Directory... ");
    FsLib::Directory SDRoot("sdmc:/switch/");
    if (SDRoot.IsOpen())
    {
        Print("\n\tEntries: %li\n", SDRoot.GetEntryCount());
        for (int i = 0; i < SDRoot.GetEntryCount(); i++)
        {
            if (SDRoot.EntryAtIsDirectory(i))
            {
                Print("\t\tDIR %s\n", SDRoot.GetEntryNameAt(i).c_str());
            }
            else
            {
                Print("\t\tFIL %s\n", SDRoot.GetEntryNameAt(i).c_str());
            }
        }
    }
    else
    {
        Print("Failed.\n");
    }

    {
        Print("Creating test file... ");
        FsLib::OutputFile TestFile("sdmc:/switch/SpecialMessage.txt", false);
        if (TestFile.IsOpen())
        {
            TestFile << "Hi, JK wrote this special message to your SD card for shits and giggles.";
            Print("Special message written to \"sdmc:/switch/SpecialMessage.txt\"!\n");
        }
        else
        {
            Print("Failed.\n");
        }
    }

    {
        Print("Opening that test file... ");
        FsLib::InputFile TestFile("sdmc:/switch/SpecialMessage.txt");
        if (TestFile.IsOpen())
        {
            std::array<char, 0x100> MessageBuffer;
            TestFile.Read(MessageBuffer.data(), 0x100);
            Print("\n\t%s\n", MessageBuffer.data());
        }
        else
        {
            Print("Failed.\n");
        }
    }

    Print("Deleting that special message because you're not special enought to read it... ");
    if (FsLib::DeleteFile("sdmc:/switch/SpecialMessage.txt"))
    {
        Print("HAHAHAHA TAKE THAT!\n");
    }
    else
    {
        Print("Your specialness has out maneuvered my deletion function. I'll get you next time.");
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
