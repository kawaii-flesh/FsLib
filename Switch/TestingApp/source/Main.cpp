#include "FsLib.hpp"
#include <array>
#include <cstdarg>
#include <cstdio>
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

    // Scoped so directory is destructed afterward for testing.
    {
        Print("Opening switch Directory... ");
        FsLib::Directory SDRoot("sdmc:/");
        if (SDRoot.IsOpen())
        {
            Print("\n\tEntries: %li\n", SDRoot.GetEntryCount());
            for (int i = 0; i < SDRoot.GetEntryCount(); i++)
            {
                Print("\t\t%s\n", SDRoot.GetEntryNameAt(i).c_str());
            }
        }
        else
        {
            Print("Failed.\n");
        }
    }

    {
        Print("Creating test file... ");
        FsLib::File TestFile("sdmc:/switch/SpecialMessage.txt", FsOpenMode_Write);
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
        Print("Testing Storage... ");
        FsLib::Storage NAND(FsBisPartitionId_System);
        if (NAND.IsOpen())
        {
            Print("Copying system partition to \"sdmc:/switch/SystemNAND.zip\".\nHope you have time to wait :)\n");
            std::unique_ptr<unsigned char[]> NANDBuffer(new unsigned char[FILE_BUFFER_SIZE]);
            zipFile NANDZip = zipOpen64("sdmc:/switch/SystemNAND.zip", 0);
            int ZipError = zipOpenNewFileInZip64(NANDZip, "/System.bin", NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 1);
            if (ZipError == ZIP_OK)
            {
                while (!NAND.EndOfStream())
                {
                    size_t BytesRead = NAND.Read(NANDBuffer.get(), FILE_BUFFER_SIZE);
                    zipWriteInFileInZip(NANDZip, NANDBuffer.get(), BytesRead);
                    Print("\tProgress: %liMB/%liMB.\n", NAND.Tell() / 1024 / 1024, NAND.GetSize() / 1024 / 1024);
                }
                zipCloseFileInZip(NANDZip);
                zipClose(NANDZip, "");
            }
        }
        else
        {
            Print("Failed.\n");
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
