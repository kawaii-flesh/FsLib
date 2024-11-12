#include "IO.hpp"
#include <memory>

// This is the size of the buffer used to copy files from ExtData. This is only 256KB, but the 3DS isn't a speed demon so I'd leave it at that.
static constexpr size_t FILE_BUFFER_SIZE = 0x40000;

// This will read a chunk of the file to buffer and return how many bytes were read.
size_t ReadChunk(FsLib::InputFile &SourceFile, unsigned char *Buffer, size_t BufferSize)
{
    size_t BytesRead = 0;
    char CurrentByte = 0;
    /*
        This will loop and read bytes one by one so we get all of the good data possible without errors making certain chunks
        invalid and corrupting the save.
    */
    while (BytesRead < BufferSize && (CurrentByte = SourceFile.GetCharacter()) != -1)
    {
        *Buffer++ = CurrentByte;
        ++BytesRead;
    }
    return BytesRead;
}

void CopyFile(const FsLib::Path &Source, const FsLib::Path &Destination)
{
    FsLib::InputFile SourceFile(Source);
    FsLib::OutputFile DestinationFile(Destination, SourceFile.GetSize());
    if (!SourceFile.IsOpen() || !DestinationFile.IsOpen())
    {
        return;
    }
    // This the buffer we're using.
    std::unique_ptr<unsigned char[]> FileBuffer(new unsigned char[FILE_BUFFER_SIZE]);
    for (uint64_t i = 0; i < SourceFile.GetSize();)
    {
        // This will read 256KB of data from the file one byte at a time and return how much was read.
        size_t BytesRead = ReadChunk(SourceFile, FileBuffer.get(), FILE_BUFFER_SIZE);
        DestinationFile.Write(FileBuffer.get(), BytesRead);

        i += BytesRead;
    }
}

void CopyDirectoryTo(const FsLib::Path &Source, const FsLib::Path &Destination)
{
    FsLib::Directory SourceDir(Source);
    if (!SourceDir.IsOpen())
    {
        return;
    }

    for (unsigned int i = 0; i < SourceDir.GetEntryCount(); i++)
    {
        if (SourceDir.EntryAtIsDirectory(i))
        {
            // New source and destination.
            FsLib::Path NewSource = Source + SourceDir.GetEntryAt(i) + u"/";
            FsLib::Path NewDestination = Destination + SourceDir.GetEntryAt(i) + u"/";
            // Make sure destination folder exists.
            if (!FsLib::CreateDirectory(NewDestination))
            {
                // Continue the loop if we can't create it.
                continue;
            }
            CopyDirectoryTo(NewSource, NewDestination);
        }
        else
        {
            FsLib::Path SourcePath = Source + SourceDir.GetEntryAt(i);
            FsLib::Path DestinationPath = Destination + SourceDir.GetEntryAt(i);
            CopyFile(SourcePath, DestinationPath);
        }
    }
}
