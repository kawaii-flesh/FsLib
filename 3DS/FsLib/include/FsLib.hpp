#pragma once
#include "Directory.hpp"
#include "InputFile.hpp"
#include "OutputFile.hpp"
#include "Path.hpp"
#include "SaveDataArchive.hpp"
#include <3ds.h>
#include <string>

namespace FsLib
{
    // Opens and mounts sdmc
    bool Initialize(void);
    // Exits and closes all open FS_Archives.
    void Exit(void);
    // Returns internal error string with slightly more information than a bool.
    const char *GetErrorString(void);
    // Maps the archive for use with FsLib.
    bool MapArchiveToDevice(std::u16string_view DeviceName, FS_Archive Archive);
    // Attempts to retrieve archive mapped to device. Returns false if it's not found.
    bool GetArchiveByDeviceName(std::u16string_view DeviceName, FS_Archive *ArchiveOut);
    // Controls archive associate with DeviceName. Only needed for save data. Extdata does not need this.
    bool ControlDevice(std::u16string_view DeviceName);
    // Closes archive associated with DeviceName.
    bool CloseDevice(std::u16string_view DeviceName);
} // namespace FsLib
