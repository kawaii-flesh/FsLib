#pragma once
#include "Stream.hpp"
#include <switch.h>

namespace FsLib
{
    class FileBase : public FsLib::Stream
    {
        public:
            FileBase(void) = default;
            // Closes handle automatically at destruction
            virtual ~FileBase();
            // This is the only real function both file classes share.
            void Close(void);

        protected:
            // File handle.
            FsFile m_FileHandle;
    };
} // namespace FsLib
