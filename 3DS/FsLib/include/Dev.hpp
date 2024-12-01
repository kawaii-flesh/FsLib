#pragma once

namespace FsLib
{
    namespace Dev
    {
        /// @brief Initializes a bare-bones compatibility layer so devkitPro libraries still work with the SD card of the 3DS.
        /// @return True on success. False on failure.
        bool InitializeSDMC(void);
    } // namespace Dev
} // namespace FsLib
