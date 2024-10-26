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
static const std::string SELFLOSS_TEST_DIR = "sdmc:/switch/SelfLoss Test Dir/";

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

    AccountUid AccountID = {0};
    if (R_FAILED(accountGetPreselectedUser(&AccountID)))
    {
        Print("Error getting AccountID for selected user.");
    }
    else
    {
        Print("Opening Selfloss for the selected user: ");
        if (!FsLib::OpenAccountSaveFileSystem("selfloss", 0x010036C01E244000, AccountID))
        {
            Print("Failed.");
        }
        else
        {
            Print("Succeeded.\n");
            FsLib::Directory SelfLoss("selfloss:/"); // Should probably check this, but fuck it.
            for (int64_t i = 0; i < SelfLoss.GetEntryCount(); i++)
            {
                Print("\t%s\n", SelfLoss.GetEntryPathAt(i).c_str());
            }

            if (FsLib::DirectoryExists(SELFLOSS_TEST_DIR))
            {
                Print("Deleting test directory.\n");
                FsLib::DeleteDirectoryRecursively(SELFLOSS_TEST_DIR);
            }

            Print("Creating folder style backup...\n");
            FsLib::CreateDirectoryRecursively(SELFLOSS_TEST_DIR);
            for (int64_t i = 0; i < SelfLoss.GetEntryCount(); i++)
            {
                std::string SDMCPath = SELFLOSS_TEST_DIR + SelfLoss.GetEntryNameAt(i);
                Print("Copying \"%s\" to \"%s\"...\n", SelfLoss.GetEntryPathAt(i).c_str(), SDMCPath.c_str());
                if (FsLib::FileExists(SDMCPath))
                {
                    Print("File \"%s\" exists already even though it shouldn't.\n", SDMCPath.c_str());
                }
                else
                {
                    FsLib::InputFile SourceFile(SelfLoss.GetEntryPathAt(i));
                    FsLib::OutputFile DestinationFile(SDMCPath, false);
                    std::vector<unsigned char> FileBuffer(SourceFile.GetSize());
                    size_t BytesRead = SourceFile.Read(FileBuffer.data(), SourceFile.GetSize());
                    DestinationFile.Write(FileBuffer.data(), BytesRead);
                }
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
    accountExit();
    return 0;
}
