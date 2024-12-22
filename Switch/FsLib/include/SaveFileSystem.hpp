#pragma once
#include <cstdint>
#include <string>
#include <switch.h>

namespace FsLib
{
    /// @brief Attempts to open system save data with SystemSaveID and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param SystemSaveID ID of system save.
    /// @param SaveDataSpaceID (Optional) FsSaveDataSpaceID.
    /// @param SaveDataRank (Optional) Rank of save data.
    /// @param AccountID (Optional) AccountID to use to open system save. This is usually 0, but can be tied to accounts.
    /// @return True on success. False on failure.
    bool OpenSystemSaveFileSystem(std::string_view DeviceName,
                                  uint64_t SystemSaveID,
                                  FsSaveDataSpaceId SaveDataSpaceID = FsSaveDataSpaceId_System,
                                  FsSaveDataRank SaveDataRank = FsSaveDataRank_Primary,
                                  AccountUid AccountID = {0});

    /// @brief Attempts to open account save data with ApplicationID and UserID and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param ApplicationID Title ID of game or title.
    /// @param UserID Account ID of user that "owns" the filesystem.
    /// @param SaveDataSpaceID (Optional) Save data space ID.
    /// @param SaveDataRank (Optional) SaveDataRank.
    /// @return True on success. False on failure.
    bool OpenAccountSaveFileSystem(std::string_view DeviceName,
                                   uint64_t ApplicationID,
                                   AccountUid UserID,
                                   FsSaveDataSpaceId SaveDataSpaceID = FsSaveDataSpaceId_User,
                                   FsSaveDataRank SaveDataRank = FsSaveDataRank_Primary);

    /// @brief Attempts to open BCAT save data with ApplicationID and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param ApplicationID Title ID of game or title.
    /// @return True on success. False on failure.
    bool OpenBCATSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID);

    /// @brief Attempts to open device save data with ApplicationID and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param ApplicationID Title ID of game or title.
    /// @return True on success. False on failure.
    bool OpenDeviceSaveFileSystem(std::string_view DeviceName, uint64_t ApplicationID);

    /// @brief Attempts to open temporary save data and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @return True on success. False on failure.
    /// @note I've only ever seen this used once before...
    bool OpenTemporarySaveFileSystem(std::string_view DeviceName);

    /// @brief Attempts to open cache save data and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param ApplicationID Title ID of game or title.
    /// @param SaveIndex Index of the cache save.
    /// @param SaveDataSpaceID (Optional) Save data space ID.
    /// @param SaveDataRank (Optional) Save data rank.
    /// @return True on success. False on failure.
    bool OpenCacheSaveFileSystem(std::string_view DeviceName,
                                 uint64_t ApplicationID,
                                 uint16_t SaveIndex,
                                 FsSaveDataSpaceId SaveDataSpaceID = FsSaveDataSpaceId_User,
                                 FsSaveDataRank SaveDataRank = FsSaveDataRank_Primary);

    /// @brief Attempts to open system BCAT save data and map it to DeviceName.
    /// @param DeviceName Name of device to map to.
    /// @param SystemSaveID ID of system save.
    /// @return True on success. False on failure.
    /// @note I've never seen this used before. Literally <b>ever</b>.
    bool OpenSystemBCATSaveFileSystem(std::string_view DeviceName, uint64_t SystemSaveID);
} // namespace FsLib
