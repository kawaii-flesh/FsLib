#pragma once
#include <cstdint>

namespace FsLib
{

    enum class SeekOrigin
    {
        Beginning,
        Current,
        End
    };
    // This is a base class for Files, Storage, and whatever else it can be used for down the line.
    class Stream
    {
        public:
            Stream(void) = default;
            // Returns if stream was successfully opened.
            bool IsOpen(void) const;
            // Returns the current offset of the stream.
            int64_t Tell(void) const;
            // Returns the stream's size;
            int64_t GetSize(void) const;
            // Returns if offset is >= to StreamSize
            bool EndOfStream(void) const;
            /*
                Seeks to the position specified in stream.
                Origin can be:
                    FsLib::SeekOrigin::Beginning,
                    FsLib::SeekOrigin::Current
                    FsLib::SeekOrigin::End
                Offset will be corrected if it is out of bounds.
            */
            void Seek(int64_t Offset, FsLib::SeekOrigin Origin);

        protected:
            // Offset and size.
            int64_t m_Offset = 0;
            int64_t m_StreamSize = 0;
            // Whether opening was successful
            bool m_IsOpen = false;
            // This can be called to ensure the current offset is valid
            void EnsureOffsetIsValid(void);
    };
} // namespace FsLib
