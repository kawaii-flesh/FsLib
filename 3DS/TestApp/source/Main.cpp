#include "FsLib.hpp"
#include <3ds.h>
#include <string>

static std::string UTF16ToUTF8(std::u16string_view Str)
{
    // This isn't the best idea.
    uint8_t UTF8Buffer[Str.length() + 1] = {0};
    utf16_to_utf8(UTF8Buffer, reinterpret_cast<const uint16_t *>(Str.data()), Str.length() + 1);
    return std::string(reinterpret_cast<const char *>(UTF8Buffer));
}

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

    if (!FsLib::OpenGameCardSaveData(u"GameCard"))
    {
        // This is kinda pointless cause of returning right away.
        printf("%s\n", FsLib::GetErrorString());
        return -2;
    }

    FsLib::Directory CardSaveRoot(u"GameCard:/");
    if (!CardSaveRoot.IsOpen())
    {
        printf("%s\n", FsLib::GetErrorString());
    }
    else
    {
        printf("Game card save data root:\n");
        for (unsigned int i = 0; i < CardSaveRoot.GetEntryCount(); i++)
        {
            if (CardSaveRoot.EntryAtIsDirectory(i))
            {
                printf("\tDIR %s\n", UTF16ToUTF8(CardSaveRoot.GetEntryAt(i)).c_str());
            }
            else
            {
                printf("\tFIL %s\n", UTF16ToUTF8(CardSaveRoot.GetEntryAt(i)).c_str());
            }
        }
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
