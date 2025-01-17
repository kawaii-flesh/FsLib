#include "fslib.hpp"
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <switch.h>

namespace
{
    // Va buffer size for print function so we have output in real time.
    constexpr int VA_BUFFER_SIZE = 0x1000;

    // These are games on my switch that have the data needed to test stuff.
    // Mario Kart 8 Deluxe has account and device.
    constexpr uint64_t MARIO_KART_8_DELUXE_APPLICATION_ID = 0x0100152000022000;
    // Monster Hunter RISE has BCAT
    constexpr uint64_t MONSTER_HUNTER_RISE_APPLICATION_ID = 0x0100B04011742000;
    // Super Mario Maker 2 has cache
    constexpr uint64_t SUPER_MARIO_MAKER_TWO_APPLICATION_ID = 0x01009B90006DC000;
    // Save ID of test system save mount.
    constexpr uint64_t TARGET_SYSTEM_SAVE = 0x8000000000000011;
} // namespace

// Feels stupid but needed to get actual output in real time on switch.
void print(const char *format, ...)
{
    // va arg the string
    char vaBuffer[VA_BUFFER_SIZE] = {0};
    std::va_list vaList;
    va_start(vaList, format);
    vsnprintf(vaBuffer, VA_BUFFER_SIZE, format, vaList);
    va_end(vaList);

    // Write to console and update so it prints in realtime instead of all at once after everything is done.
    std::fputs(vaBuffer, stdout);
    consoleUpdate(NULL);
}

int main(void)
{
    // Initialize fslib
    if (!fslib::initialize())
    {
        return -1;
    }

    // This goes here if you're using romfs cause I don't feel like writing something for that.
    /*
    if(R_FAILED(romfsInit()))
    {
        return -2;
    }
    */

    // This shuts down fs_dev and puts it's own sdmc devop in its place. This only works for files on the SDMC. Everything else should use fslib.
    if (!fslib::dev::initializeSDMC())
    {
        return -3;
    }

    // Default libnx console.
    consoleInit(NULL);

    // Game pad stuff.
    PadState gamePad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&gamePad);

    // Tests. Printing next to nothing to the screen is a good thing for this.

    // Storage reading test.
    std::unique_ptr<unsigned char[]> cal0Buffer = nullptr;
    fslib::Storage cal0(FsBisPartitionId_CalibrationBinary);
    if (!cal0.isOpen())
    {
        print("Error opening cal0: %s\n", fslib::getErrorString());
    }
    else
    {
        // Allocate buffer and read something you're not really supposed to be able to.
        cal0Buffer = std::make_unique<unsigned char[]>(cal0.getSize());
        if (cal0.read(cal0Buffer.get(), cal0.getSize()) != cal0.getSize())
        {
            print("Error reading from storage: %s\n", fslib::getErrorString());
        }
    }

    // Directory reading test.
    fslib::Directory switchFolder("sdmc:/switch");
    if (!switchFolder.isOpen())
    {
        print("%s\n", fslib::getErrorString());
    }
    else
    {
        print("Switch folder contents:\n");
        for (int64_t i = 0; i < switchFolder.getEntryCount(); i++)
        {
            print("\t%s\n", switchFolder[i]);
        }
    }

    // Directory function testing.
    if (!fslib::createDirectory("sdmc:/fslibTestDir"))
    {
        print("%s\n", fslib::getErrorString());
    }

    if (!fslib::createDirectoriesRecursively("sdmc:/more/and/more/and/more/and/more/and/more/directories"))
    {
        print("%s\n", fslib::getErrorString());
    }

    if (!fslib::deleteDirectory("sdmc:/fslibTestDir"))
    {
        print("%s\n", fslib::getErrorString());
    }

    if (!fslib::deleteDirectoryRecursively("sdmc:/more"))
    {
        print("%s\n", fslib::getErrorString());
    }

    if (!fslib::renameDirectory("sdmc:/JKSV", "sdmc:/JKSM"))
    {
        print("%s\n", fslib::getErrorString());
    }

    // File function testing.
    if (!fslib::createFile("sdmc:/fslibTestFile.bin", 0x100000))
    {
        print("%s\n", fslib::getErrorString());
    }

    if (!fslib::fileExists("sdmc:/hbmenu.nro"))
    {
        print("%s\n", fslib::getErrorString());
    }

    print("fslibTestFile.bin's size: 0x%08X\n", fslib::getFileSize("sdmc:/fslibTestFile.bin"));

    if (!fslib::deleteFile("sdmc:/fslibTestfile.bin"))
    {
        print("%s\n", fslib::getErrorString());
    }
    // This is just a random file on my sdmc to test this with.
    if (!fslib::renameFile("sdmc:/File.txt", "sdmc:/notAFile.txt"))
    {
        print("%s\n", fslib::getErrorString());
    }


    // File test
    // First we're going to open and read a small part of the homebrew menu.
    fslib::File hbmenu("sdmc:/hbmenu.nro", FsOpenMode_Read);
    if (!hbmenu.isOpen())
    {
        print("%s\n", fslib::getErrorString());
    }
    else
    {
        // Seek to this position and read off the weird header thingy?
        char weirdHeaderThingy[0xD] = {0};
        hbmenu.seek(8, hbmenu.beginning);

        if (hbmenu.read(weirdHeaderThingy, 0xC) != 0xC)
        {
            print("%s\n", fslib::getErrorString());
        }
        else
        {
            print("%s\n", weirdHeaderThingy);
        }
    }

    fslib::File cal0File("sdmc:/cal0.bin", FsOpenMode_Create | FsOpenMode_Write);
    if (!cal0File.isOpen())
    {
        print("%s\n", fslib::getErrorString());
    }
    else if (cal0File.write(cal0Buffer.get(), cal0.getSize()) != cal0.getSize())
    {
        print("%s\n", fslib::getErrorString());
    }

    // Misc fslib testing functions.
    int64_t sdmcFreeSpace, sdmcTotalSpace;
    if (!fslib::getDeviceFreeSpace("sdmc:/", sdmcFreeSpace) || !fslib::getDeviceTotalSpace("sdmc:/", sdmcTotalSpace))
    {
        print("%s\n", fslib::getErrorString());
    }
    else
    {
        print("sdmc: %li/%li\n", sdmcFreeSpace, sdmcTotalSpace);
    }

    print("Press + to exit.");

    while (appletMainLoop())
    {
        padUpdate(&gamePad);
        if (padGetButtonsDown(&gamePad) & HidNpadButton_Plus)
        {
            break;
        }
        consoleUpdate(NULL);
    }

    fslib::exit();
    consoleExit(NULL);
    return 0;
}
