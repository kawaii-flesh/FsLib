#pragma once
#include "Stream.hpp"
#include <switch.h>

namespace FsLib
{
    // This is the storage reading/writing class. Still WIP.
    // I'm leaving this read only for reasons.
    class Storage : public FsLib::Stream
    {
        public:
            Storage(void) = default;
            // These constructors just call the matching open function for you.
            Storage(FsBisPartitionId PartitionID);
            // Closes handle on destruction
            ~Storage();
            // Opens Bis partition. Mostly useful for dumping nand and stuff.
            void Open(FsBisPartitionId PartitionID);
            // Closes storage handle
            void Close(void);
            // Reads from storage. Returns size read on success, 0 on failure.
            size_t Read(void *Buffer, size_t BufferSize);

        private:
            // Storage handle.
            FsStorage m_StorageHandle;
    };
} // namespace FsLib
