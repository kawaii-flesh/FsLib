#include "FsLib.hpp"
#include <3ds.h>

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

    printf("Opening SD card root... ");
    FsLib::Directory SDMCRoot(u"sdmc:/");
    if (SDMCRoot.IsOpen())
    {
        printf("Succeeded!\n");
        for (uint32_t i = 0; i < SDMCRoot.GetEntryCount(); i++)
        {
            std::string EntryName = SDMCRoot.GetEntryNameAtAsUTF8(i);
            printf("\t%s\n", EntryName.c_str());
        }
    }
    else
    {
        printf("Failed.\n");
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
