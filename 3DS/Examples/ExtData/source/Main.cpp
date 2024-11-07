#include "FsLib.hpp"
#include <memory>

// You can use this, but might not want to because if something else uses the same naming for anything it will cause issues.
// using namespace FsLib;

/*
    This is only the NA version of the game. You're going to need to handle the versions from other regions too.
    Different regions generally use separate, different archive ids.
*/
static constexpr uint64_t TITLE_ID = 0x00040000001B8700;

// This is the size of the buffer used to copy files from ExtData. This is only 256KB, but the 3DS isn't a speed demon so I'd leave it at that.
static constexpr size_t FILE_BUFFER_SIZE = 0x40000;

// This is the target folder on SD to backup and restore from.
static constexpr std::u16string_view TARGET_DIRECTORY = u"sdmc:/MinecraftSave/";

// This will convert the UTF16 strings the 3DS FS uses to printable ASCII/UTF-8... even if it's a bit unsafe.
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
                Make a new path to use and pass it to this function so we can print the contents of other subdirectories too.
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

static void CopyFile(const FsLib::Path &Source, const FsLib::Path &Destination)
{
    /*
        These are our file handles/instances. Destination uses the second constructor/Open function that takes a size to use when creating the
        file since we're working with ExtData here and it's needed to write to it properly. This isn't needed when working with files in normal
        saves or on the SD card.
    */
    FsLib::InputFile SourceFile(Source);
    if (!SourceFile.IsOpen())
    {
        printf("%s\n", FsLib::GetErrorString());
        return;
    }

    FsLib::OutputFile DestinationFile(Destination, SourceFile.GetSize());
    if (!DestinationFile.IsOpen())
    {
        printf("%s\n", FsLib::GetErrorString());
        return;
    }

    /*
        This is the buffer we're using to read and write the file. It's up to you how you want to do this. Allocating an array with new or malloc
        or using vector also works. Just remember to delete[] or free the array after the loop if that's the way you want to go.
    */
    std::unique_ptr<unsigned char[]> FileBuffer(new unsigned char[FILE_BUFFER_SIZE]);
    // This is my preferred way of doing this loop, but you *should* be able to also use while(!SourceFile.EndOfFile()).
    for (uint64_t i = 0; i < SourceFile.GetSize();)
    {
        // Read into buffer from SourceFile. Read returns the number of bytes read from the file.
        size_t BytesRead = SourceFile.Read(FileBuffer.get(), FILE_BUFFER_SIZE);
        if (BytesRead == 0) // This will indicate an error occured reading.
        {
            printf("%s\n", FsLib::GetErrorString());
            return;
        }

        // Same routine as above.
        size_t BytesWritten = DestinationFile.Write(FileBuffer.get(), BytesRead);
        if (BytesWritten == 0 || BytesWritten != BytesRead)
        {
            printf("%s\n", FsLib::GetErrorString());
            return;
        }
        // Update the current offset so the loop ends when it should.
        i += BytesRead;
    }
    /*
        Since we used unique_ptr and FsLib::[X]File, there is no cleanup needed. Everything should cleanup with destructors.
    */
}

static void CopyDirectoryToDirectory(const FsLib::Path &Source, const FsLib::Path &Destination)
{
    FsLib::Directory SourceDir(Source);
    if (!SourceDir.IsOpen())
    {
        printf("%s\n", FsLib::GetErrorString());
        return;
    }

    for (unsigned int i = 0; i < SourceDir.GetEntryCount(); i++)
    {
        if (SourceDir.EntryAtIsDirectory(i))
        {
            // These are the new paths being passed to this function since this is recursive.
            FsLib::Path NewSource = Source + SourceDir.GetEntryAt(i) + u"/";
            FsLib::Path NewDestination = Destination + SourceDir.GetEntryAt(i);
            // Need to make sure the directory actually exists in destination.
            if (!FsLib::CreateDirectory(NewDestination))
            {
                printf("%s\n", FsLib::GetErrorString());
                continue;
            }

            // To do: Add SubPath for this instead of adding the / afterwards.
            NewDestination += u"/";
            // Call this function again, but with the new source and destination Paths.
            CopyDirectoryToDirectory(NewSource, NewDestination);
        }
        else
        {
            // These are the full paths to each item.
            FsLib::Path SourcePath = Source + SourceDir.GetEntryAt(i);
            FsLib::Path DestinationPath = Destination + SourceDir.GetEntryAt(i);
            // This function will copy the file in chunks.
            CopyFile(SourcePath, DestinationPath);
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

    printf("Press A to backup ExtData\nPress Y to print the ExtData contents\nPress X to copy SD card to ExtData\nStart to exit.\n");
    while (aptMainLoop())
    {
        hidScanInput();

        /*
            This is only supposed to return if buttons are pressed for the current frame, but it seems broken now and has issues.
            Holding the buttons will make this example repeat operations which shouldn't happen.
        */
        uint32_t ButtonsDown = hidKeysDown();

        if (ButtonsDown & KEY_A)
        {
            printf("Backing up save data to sdmc:/MinecraftSave/... \n");
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
            // This if statement will check for and try to create the target folder if it doesn't exist.
            if (!FsLib::DirectoryExists(TARGET_DIRECTORY) && !FsLib::CreateDirectory(TARGET_DIRECTORY))
            {
                printf("%s\n", FsLib::GetErrorString());
                continue;
            }

            // This will recursively copy the ExtData archive/device to the folder on SD we just created.
            CopyDirectoryToDirectory(u"ExtData:/", u"sdmc:/MinecraftSave/");

            printf("Done!\n");
        }
        else if (ButtonsDown & KEY_X)
        {
            // Check if the directory on SD exists. If it doesn't, just continue the loop.
            if (!FsLib::DirectoryExists(u"sdmc:/MinecraftSave/"))
            {
                continue;
            }

            printf("Importing Data from SD card... \n");
            CopyDirectoryToDirectory(u"sdmc:/MinecraftSave/", u"ExtData:/");
            printf("Done!\n");
        }
        else if (ButtonsDown & KEY_Y)
        {
            // Same as above except printing instead of copying.
            PrintDirectory(u"ExtData:/");
        }
        else if (ButtonsDown & KEY_START)
        {
            // Just break the loop.
            break;
        }
    }

    FsLib::Exit();
    gfxExit();
    return 0;
}
