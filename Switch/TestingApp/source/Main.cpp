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

    if (!FsLib::OpenBisFileSystem("user", FsBisPartitionId_User))
    {
        Print("Error opening user partition: %s", FsLib::GetErrorString());
        return -1;
    }

    if (!FsLib::OpenBisFileSystem("system", FsBisPartitionId_System))
    {
        Print("Error opening system partition: %s", FsLib::GetErrorString());
        return -2;
    }

    int64_t SDFree, SDTotal;
    int64_t UserFree, UserTotal;
    int64_t SystemFree, SystemTotal;
    Print("Device: Free MB / Total MB\n");
    if (FsLib::GetDeviceFreeSpace("sdmc:/", SDFree) && FsLib::GetDeviceTotalSpace("sdmc:/", SDTotal))
    {
        Print("SDMC: %lli MB / %lli MB.\n", SDFree / 1024 / 1024, SDTotal / 1024 / 1024);
    }
    else
    {
        Print("%s\n", FsLib::GetErrorString());
    }

    if (FsLib::GetDeviceFreeSpace("user:/", UserFree) && FsLib::GetDeviceTotalSpace("user:/", UserTotal))
    {
        Print("USER: %lli MB / %lli MB.\n", UserFree / 1024 / 1024, UserTotal / 1024 / 1024);
    }
    else
    {
        Print("%s\n", FsLib::GetErrorString());
    }

    if (FsLib::GetDeviceFreeSpace("system:/", SystemFree) && FsLib::GetDeviceTotalSpace("system:/", SystemTotal))
    {
        Print("System: %lli MB / %lli MB.\n", SystemFree / 1024 / 1024, SystemTotal / 1024 / 1024);
    }
    else
    {
        Print("%s\n", FsLib::GetErrorString());
    }

    // This is to make sure root path works with these changes.
    FsLib::Path SDMCRoot = "sdmc:/";
    Print("%s\n", SDMCRoot.CString());

    // This is ridiculous but I'm trying to break this on purpose.
    FsLib::Path TestPath = "sdmc://////////////////////////////////JKSV////////////////////////////////////////////////";
    TestPath = TestPath / "///////////////////////////////////////////POKEMON SHIELD//////////////////////////////////////////" /
               "///////////////////////_HACKED CHEATER SAVE 99///////////////////////";
    Print("%s\n", TestPath.CString());

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
