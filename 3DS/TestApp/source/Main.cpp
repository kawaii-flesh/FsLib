#include "FsLib.hpp"
#include <3ds.h>
#include <string>

static std::u16string_view REALLY_LONG_DIR_PATH =
    u"sdmc:/A/Really/Long/Chain/Of/Folders/To/Make/Sure/Create/Directories/Recursively/Works/Right/And/Doesnt/Crash/The/3DS/";

static std::string UTF16ToUTF8(std::u16string_view Str)
{
    // This isn't the best idea.
    uint8_t UTF8Buffer[Str.length() + 1] = {0};
    utf16_to_utf8(UTF8Buffer, reinterpret_cast<const uint16_t *>(Str.data()), Str.length() + 1);
    return std::string(reinterpret_cast<const char *>(UTF8Buffer));
}

static void PrintDirectory(const FsLib::Path &DirectoryPath)
{
    FsLib::Directory Dir(DirectoryPath);
    for (unsigned int i = 0; i < Dir.GetEntryCount(); i++)
    {
        if (Dir.EntryAtIsDirectory(i))
        {
            printf("\tDIR %s\n", UTF16ToUTF8(Dir.GetEntryAt(i)).c_str());
            FsLib::Path NewPath = DirectoryPath + Dir.GetEntryAt(i) + u"/";
            PrintDirectory(NewPath);
        }
        else
        {
            printf("\tFIL %s\n", UTF16ToUTF8(Dir.GetEntryAt(i)).c_str());
        }
    }
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

    if (!FsLib::CreateDirectoriesRecursively(REALLY_LONG_DIR_PATH))
    {
        printf("%s\n", FsLib::GetErrorString());
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
