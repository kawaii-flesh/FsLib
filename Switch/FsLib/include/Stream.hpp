#pragma once
#include <cstdint>

namespace FsLib
{
    /// @brief Values used for seeking in streams.
    enum class SeekOrigin
    {
        Beginning,
        Current,
        End
    };

    /// @brief This is the base class all File and storage types are derived from.
    class Stream
    {
        public:
            /// @brief Default Stream constructor.
            Stream(void) = default;

            /// @brief Checks if stream was successfully opened.
            /// @return True on success. False on failure.
            bool IsOpen(void) const;

            /// @brief Gets the current offset in the stream.
            /// @return Current offset of the stream.
            int64_t Tell(void) const;

            /// @brief Gets the size of the current stream.
            /// @return Stream's size.
            int64_t GetSize(void) const;

            /// @brief Returns if the end of the stream has been reached.
            /// @return True if end of stream has been reached. False if it hasn't.
            bool EndOfStream(void) const;

            /**
             * @brief Seeks to Offset relative to Origin
             *
             * @param Offset Offset to seek to.
             * @param Origin Origin from whence to seek.
             * @note Origin can be one of the following:
             *      1. FsLib::SeekOrigin::Beginning
             *      2. FsLib::SeekOrigin::Current
             *      3. FsLib::SeekOrigin::End
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
