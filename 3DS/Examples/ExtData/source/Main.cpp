#include "FsLib.hpp"
#include <vector>

// You can use this, but might not want to because if something else uses the same naming for anything it will cause issues.
// using namespace FsLib;

/*
    This is only the NA version of the game. You're going to need to handle the versions from other regions too.
    Different regions generally use separate, different archive ids.
*/
static constexpr uint64_t TITLE_ID = 0x00040000001B8700;

// This will convert the UTF16 strings the 3DS FS uses to printable ASCII/UTF-8 even if it's a bit unsafe.
static std::string UTF16ToUTF8(std::u16string_view Str)
{
    // This isn't the best idea.
    uint8_t UTF8Buffer[Str.length() + 1] = {0};
    utf16_to_utf8(UTF8Buffer, reinterpret_cast<const uint16_t *>(Str.data()), Str.length() + 1);
    return std::string(reinterpret_cast<const char *>(UTF8Buffer));
}

static void PrintDirectory(const FsLib::Path &DirectoryPath)
{
    // This will open and read the directory entries from the path passed.
    FsLib::Directory Directory(DirectoryPath);
    // If it didn't open, print error and bail.
    if (!Directory.IsOpen())
    {
        printf("%s\n", FsLib::GetErrorString());
        return;
    }

    // Loop through the directory entries and either open and print another directory or print the file name.
    for (unsigned int i = 0; i < Directory.GetEntryCount(); i++)
    {
        if (Directory.EntryAtIsDirectory(i))
        {
            // Print the entry name and that it's a directory.
            printf("\tDIR %s\n", UTF16ToUTF8(Directory.GetEntryAt(i)).c_str());
            /*
                Make a new path to use and pass it to this function so we can print the contents of other, subdirectories too.
                Note: I'm considering handling these differently and adding an operator/ instead of operator+ to make sure paths are valid.
            */
            FsLib::Path NewDirectoryPath = DirectoryPath + Directory.GetEntryAt(i) + u"/";
            PrintDirectory(NewDirectoryPath);
        }
        else
        {
            // Just print that it's a file.
            printf("\tFIL %s\n", UTF16ToUTF8(Directory.GetEntryAt(i)).c_str());
        }
    }
}

static void CopyDirectoryToDirectory(const FsLib::Path &Source, const FsLib::Path &Destination)
{
    FsLib::Directory SourceDir(Source);
    if (!SourceDir.IsOpen())
    {
        printf("%s:%s: %s\n",
               UTF16ToUTF8(Source.GetDeviceName().data()).c_str(),
               UTF16ToUTF8(Source.GetPathData()).c_str(),
               FsLib::GetErrorString());
        return;
    }

    for (unsigned int i = 0; i < SourceDir.GetEntryCount(); i++)
    {
        if (SourceDir.EntryAtIsDirectory(i))
        {
            FsLib::Path NewSource = Source + SourceDir.GetEntryAt(i) + u"/";
            FsLib::Path NewDestination = Destination + SourceDir.GetEntryAt(i);
            // To do: Add SubPath for this instead of adding the / afterwards.
            FsLib::CreateDirectory(NewDestination);
            NewDestination += u"/";
            CopyDirectoryToDirectory(NewSource, NewDestination);
        }
        else
        {
            FsLib::Path SourcePath = Source + SourceDir.GetEntryAt(i);
            FsLib::Path DestinationPath = Destination + SourceDir.GetEntryAt(i);
            FsLib::InputFile SourceFile(SourcePath);
            FsLib::OutputFile DestinationFile(DestinationPath, false);

            std::vector<unsigned char> FileBuffer(SourceFile.GetSize());
            SourceFile.Read(FileBuffer.data(), SourceFile.GetSize());
            DestinationFile.Write(FileBuffer.data(), SourceFile.GetSize());
        }
    }
}

int main(void)
{
    // This is standard ctrulib stuff.
    gfxInitDefault();
    if (!consoleInit(GFX_TOP, NULL))
    {
        return -1;
    }

    // This makes sure fsInit is called and maps the sd card to "sdmc".
    if (!FsLib::Initialize())
    {
        /*
            This will print the internal error string to the screen for a split second before the program returns and closes.
            Can't really log the error to a file if the lib didn't start correctly... If you want, you can call fsInit() and use C
            stdio or std::ofstream for logging errors to be sure of what is happening, but this shouldn't fail ever.
        */
        printf("%s\n", FsLib::GetErrorString());
        return -2;
    }

    /*
        This will open and map the Archive to u"ExtData:/" if it succeeds.
        It's written like this so you have an idea of how to handle this with full title ids, but using the truncated, shortened one will work
        fine for this.
    */
    if (!FsLib::OpenExtData(u"ExtData", static_cast<uint32_t>(TITLE_ID >> 8 & 0x00FFFFFF)))
    {
        printf("%s\n", FsLib::GetErrorString());
        return -3;
    }

    printf("Press A to backup ExtData\nY to print the ExtData contents\nStart to exit.\n");
    while (aptMainLoop())
    {
        hidScanInput();
        uint32_t ButtonsDown = hidKeysDown();

        if (ButtonsDown & KEY_A)
        {
            /*
                The 3DS accepts two different path types: ASCII and UTF-16. Most of 3DS and ctrulib's internal FS structs use UTF-16 so that's
                why I went with that over ASCII. The path class of FsLib will automatically handle assignment and appending to and from the
                following types:
                    FsLib::Path
                    const char16_t* and const uint16_t*. UTF-16 C strings.
                    const std::u16string and std::u16string_view. UTF-16 C++ strings.

                Typing u before a string will indicate it's unicode and the path class will take care of the rest even for functions which is
                how the following works.
            */
            printf("Backing up save data to sdmc:/MinecraftSave/... ");
            FsLib::CreateDirectory(u"sdmc:/MinecraftSave/");
            CopyDirectoryToDirectory(u"ExtData:/", u"sdmc:/MinecraftSave/");
            printf("Done!\n");
        }
        else if (ButtonsDown & KEY_Y)
        {
            // Same as above except printing instead of copying.
            PrintDirectory(u"ExtData:/");
        }
        else if (ButtonsDown & KEY_START)
        {
            break;
        }
    }

    FsLib::Exit();
    gfxExit();
    return 0;
}
