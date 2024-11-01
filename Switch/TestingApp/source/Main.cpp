#include "FsLib.hpp"
#include <array>
#include <cstdarg>
#include <switch.h>
#include <vector>

static constexpr unsigned int VA_BUFFER_SIZE = 0x1000;
static constexpr std::string_view TEST_MESSAGE_PATH = "sdmc:/switch/TestMessage.txt";
static constexpr std::string_view REALLY_STUPID_LONG_DIR_PATH =
    "sdmc:/This/is/a/really/long/chain/of/folders/that/you/do/not/even/want/on/your/sd/card/lol/The/End/Or/is/it/";

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
    consoleInit(NULL);

    PadState GamePad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&GamePad);

    FsLib::Initialize();

    // Test run of functions.
    Print("Testing if homebrew menu exists... ");
    if (FsLib::FileExists("sdmc:/hbmenu.nro"))
    {
        Print("It does.\n");
    }
    else
    {
        Print("Nope. %s\n", FsLib::GetErrorString());
    }

    {
        Print("Checking if I can open it for reading... ");
        FsLib::InputFile HBMenu("sdmc:/hbmenu.nro");
        if (HBMenu.IsOpen())
        {
            Print("It does.");
        }
        else
        {
            Print("Nope. %s", FsLib::GetErrorString());
        }
    }

    Print("Checking if Switch directory exists... ");
    if (FsLib::DirectoryExists("sdmc:/switch/"))
    {
        Print("It does.\n");
    }
    else
    {
        Print("Nope. %s\n", FsLib::GetErrorString());
    }

    {
        Print("Checking if I can list everything in the Switch folder... ");
        FsLib::Directory SwitchDir("sdmc:/switch/");
        if (!SwitchDir.IsOpen())
        {
            Print("Nope. %s\n", FsLib::GetErrorString());
        }
        else
        {
            Print("I did it:\n");
            for (int64_t i = 0; i < SwitchDir.GetEntryCount(); i++)
            {
                if (SwitchDir.EntryAtIsDirectory(i))
                {
                    Print("\tDIR %s\n", SwitchDir.GetEntryNameAt(i).c_str());
                }
                else
                {
                    Print("\tFIL %s\n", SwitchDir.GetEntryNameAt(i).c_str());
                }
            }
        }
    }

    {
        Print("Testing if I can write something to the SD card... ");
        FsLib::OutputFile TestMessage("sdmc:/switch/TestMessage.txt", false);
        if (!TestMessage.IsOpen())
        {
            Print("Nope. %s\n", FsLib::GetErrorString());
        }
        else
        {
            TestMessage.Writef("Test message inside this file super secret don't read.");
            Print("I guess it worked.\n");
        }
    }

    {
        Print("Testing if I can open and read that file... ");
        FsLib::InputFile TestMessage(TEST_MESSAGE_PATH);
        if (TestMessage.IsOpen())
        {
            Print("\nDid it: ");
            std::array<char, 0x1000> MessageBuffer;
            // I should check this, but I'm not going to bother.
            TestMessage.Read(MessageBuffer.data(), 0x1000);
            Print("%s\n", MessageBuffer.data());
        }
        else
        {
            Print("Nope. %s\n", FsLib::GetErrorString());
        }
    }

    Print("Testing if I can delete that file... ");
    if (FsLib::DeleteFile(TEST_MESSAGE_PATH))
    {
        Print("Yep.\n");
    }
    else
    {
        Print("Nope. %s\n", FsLib::GetErrorString());
    }

    Print("Testing is I can create a stupid long chain of folders... ");
    if (FsLib::CreateDirectoryRecursively(REALLY_STUPID_LONG_DIR_PATH))
    {
        Print("Yep.\n");
    }
    else
    {
        Print("Nope. %s\n", FsLib::GetErrorString());
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
