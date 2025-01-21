#include "fslib.hpp"
#include "string.hpp"
#include <3ds.h>
#include <string>

extern std::string g_fslibErrorString;

bool fslib::deleteExtraData(FS_MediaType mediaType, uint32_t extraDataID)
{
    FS_ExtSaveDataInfo extraDataInfo = {.mediaType = mediaType, .unknown = 0, .reserved1 = 0, .saveId = extraDataID, .reserved2 = 0};

    Result fsError = FSUSER_DeleteExtSaveData(extraDataInfo);
    if (R_FAILED(fsError))
    {
        g_fslibErrorString = string::getFormattedString("Error deleting extra data %08X: 0x%08X.", extraDataID, fsError);
        return false;
    }
    return true;
}
