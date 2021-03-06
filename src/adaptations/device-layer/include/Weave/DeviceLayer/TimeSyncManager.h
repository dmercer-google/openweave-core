/*
 *
 *    Copyright (c) 2018 Nest Labs, Inc.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef TIME_SYNC_MANAGER_H
#define TIME_SYNC_MANAGER_H

namespace nl {
namespace Weave {
namespace Profiles {
namespace StatusReporting {
class StatusReport;
}
}
}
}

namespace nl {
namespace Weave {
namespace DeviceLayer {

namespace Internal {
extern WEAVE_ERROR InitServiceDirectoryManager(void);
}

class TimeSyncManager
{
public:

    enum TimeSyncMode
    {
        kTimeSyncMode_NotSupported          = -1,
        kTimeSyncMode_Disabled,
        kTimeSyncMode_Service,

        kTimeSyncMode_Max,
    };

    TimeSyncMode GetMode();
    WEAVE_ERROR SetMode(TimeSyncMode newMode);

    uint32_t GetSyncInterval();
    void SetSyncInterval(uint32_t intervalSec);

    bool IsTimeSynchronized();

private:

    // NOTE: These members are for internal use by the following friends.

    friend class PlatformManager;
    friend WEAVE_ERROR Internal::InitServiceDirectoryManager();

    WEAVE_ERROR Init();

    void OnPlatformEvent(const WeaveDeviceEvent * event);

#if WEAVE_DEVICE_CONFIG_ENABLE_SERVICE_DIRECTORY_TIME_SYNC
    static void MarkServiceDirRequestStart();
    static void ProcessServiceDirTimeData(uint64_t timeQueryReceiptMsec, uint32_t timeProcessMsec);
#endif

private:

    // NOTE: These members are private to the class and should not be used by friends.

    uint64_t mLastSyncTimeMS; // in monotonic time
#if WEAVE_DEVICE_CONFIG_ENABLE_SERVICE_DIRECTORY_TIME_SYNC
    uint64_t mServiceDirTimeSyncStartUS;
#endif
#if WEAVE_DEVICE_CONFIG_ENABLE_WEAVE_TIME_SERVICE_TIME_SYNC
    ::nl::Weave::Binding * mTimeSyncBinding;
#endif
    uint32_t mSyncIntervalSec;
    TimeSyncMode mMode;

    void DriveTimeSync();
    void CancelTimeSync();
    void ApplySynchronizedTime(uint64_t syncedRealTimeUS);
    void TimeSyncFailed(WEAVE_ERROR reason, nl::Weave::Profiles::StatusReporting::StatusReport * statusReport);

#if WEAVE_DEVICE_CONFIG_ENABLE_WEAVE_TIME_SERVICE_TIME_SYNC
    static void TimeServiceSync_HandleBindingEvent(void * appState, ::nl::Weave::Binding::EventType event,
            const ::nl::Weave::Binding::InEventParam & inParam, ::nl::Weave::Binding::OutEventParam & outParam);
    static void TimeServiceSync_HandleSyncComplete(void * context, WEAVE_ERROR result, int64_t syncedRealTimeUS);
#endif

    static void DriveTimeSync(::nl::Weave::System::Layer * layer, void * appState, ::nl::Weave::System::Error err);
};

inline TimeSyncManager::TimeSyncMode TimeSyncManager::GetMode()
{
    return mMode;
}

inline uint32_t TimeSyncManager::GetSyncInterval()
{
    return mSyncIntervalSec;
}

} // namespace DeviceLayer
} // namespace Weave
} // namespace nl

#endif // TIME_SYNC_MANAGER_H
