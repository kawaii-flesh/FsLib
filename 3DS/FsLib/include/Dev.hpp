#pragma once

namespace FsLib
{
    namespace Dev
    {
        /*
            This provides a bare minimum newlib -> FsLib wrapper that allows FsLib to handle file read and write calls to the SD card from
            devkitpro libs. To enable this, override libctru's void __appInit function with the following:
                void __appInit(void)
                {
                    srvInit();
                    aptInit();
                    fsInit();
                    hidInit();
                }
            Call FsLib::Dev::Initialize() and do the same as above with __appExit and you're archive_dev.c free!
        */
        bool InitializeSDMC(void);
    } // namespace Dev
} // namespace FsLib
