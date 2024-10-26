#include "FsLib.hpp"
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <minizip/zip.h>
#include <switch.h>
#include <vector>

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

    accountInitialize(AccountServiceType_Application);
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

    Print("Opening ProdInfo... ");
    FsLib::Storage Cal0(FsBisPartitionId_CalibrationBinary);
    if (Cal0.IsOpen())
    {
        Print("Succeeded.\nDumping to SD card... ");
        std::vector<unsigned char> ProdInfoBuffer(Cal0.GetSize());
        if (Cal0.Read(ProdInfoBuffer.data(), Cal0.GetSize()) != Cal0.GetSize())
        {
            Print("Failed.");
        }
        else
        {
            FsLib::OutputFile ProdInfoFile("sdmc:/ProdInfo.bin", false);
            if (ProdInfoFile.Write(ProdInfoBuffer.data(), Cal0.GetSize()) == Cal0.GetSize())
            {
                Print("Succeeded!\n");
            }
            else
            {
                Print("Failed.\n");
            }
        }
    }
    else
    {
        Print("Failed.\n");
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
    accountExit();
    return 0;
}
