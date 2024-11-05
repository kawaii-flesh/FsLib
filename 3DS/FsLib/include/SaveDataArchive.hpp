#pragma once
#include <3ds.h>
#include <string>

namespace FsLib
{
    // Opens the respective Archive type. Might add more if I can remember how later.
    bool OpenSaveData(std::u16string_view DeviceName); // This is only really useful for old *hax
    bool OpenExtData(std::u16string_view DeviceName, uint32_t ExtDataID);
    bool OpenSharedExtData(std::u16string_view DeviceName, uint32_t SharedExtDataID);
    bool OpenSystemSaveData(std::u16string_view DeviceName, uint32_t UniqueID);
    bool OpenGameCardSaveData(std::u16string_view DeviceName);
    bool OpenUserSaveData(std::u16string_view DeviceName, FS_MediaType MediaType, uint32_t LowerID, uint32_t UpperID);
} // namespace FsLib
