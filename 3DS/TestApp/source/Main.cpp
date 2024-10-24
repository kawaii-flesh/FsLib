#include "FsLib.hpp"
#include <3ds.h>
#include <array>

int main(void)
{
    hidInit();
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    printf("FsLib Test App\n");

    if (!FsLib::Initialize())
    {
        printf("%s\n", FsLib::GetErrorString());
        return -1;
    }

    // Array of shared extdata archives. This is the easiest, quickest thing every 3DS has to test this.
    static constexpr uint32_t SharedArchives[] = {0xF0000001, 0xF0000002, 0xF0000009, 0xF000000B, 0xF000000C, 0xF000000D, 0xF000000E};
    // I really don't feel like writing some coversion sprintf....
    static const std::u16string SharedMountPoints[] = {u"f0000001", u"f0000002", u"f00000009", u"f000000b", u"f000000c", u"f000000d", u"f000000e"};

    printf("Opening and printing shared extdata archives: \n");
    for (int i = 0; i < 7; i++)
    {
        // Since I'm testing, none of these are going to be closed after reading. FsLib should clean that up itself.
        if (!FsLib::OpenSharedExtData(SharedMountPoints[i], SharedArchives[i]))
        {
            printf("Failed to open shared archive %08lX: %s.\n", SharedArchives[i], FsLib::GetErrorString());
            continue;
        }

        printf("Opened shared archive %08lX: ", SharedArchives[i]);

        // Need to make this a path FsLib can read correctly.
        std::u16string ArchivePath = SharedMountPoints[i] + u":/";
        FsLib::Directory SharedDirectory(ArchivePath);
        if (!SharedDirectory.IsOpen())
        {
            printf("Failed to open directory!\n");
            continue;
        }

        // Print the listing.
        printf("\n");
        for (int i = 0; i < static_cast<int>(SharedDirectory.GetEntryCount()); i++)
        {
            if (SharedDirectory.EntryAtIsDirectory(i))
            {
                printf("\tDIR %s\n", SharedDirectory.GetEntryPathAtAsUTF8(i).c_str());
            }
            else
            {
                printf("\tFIL %s\n", SharedDirectory.GetEntryPathAtAsUTF8(i).c_str());
            }
        }
        // Add an exta line here cause 3DS screen is really cramped.
        printf("\n");
    }


    printf("Press Start to exit.");
    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_START)
        {
            break;
        }
    }

    FsLib::Exit();
    gfxExit();
    hidExit();
    return 0;
}
