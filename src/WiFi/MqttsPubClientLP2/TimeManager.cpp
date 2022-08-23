/*
 * TimeManager.cpp
 * Copyright (C) 2022 Takashi Matsuoka <matsujirushi@live.jp>
 * MIT License
 */
#include "TimeManager.hpp"
#include <Arduino.h>
#include <atomic>
#include <esp_attr.h>
#include <esp_sntp.h>

static RTC_DATA_ATTR time_t LastSyncTime{ 0 };
static std::atomic_int SyncedCount{ 0 };

static void TimeSyncNotificationCallback(struct timeval* tv)
{
	if (sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED)
	{
		LastSyncTime = time(nullptr);
		SyncedCount++;
	}
}

TimeManagerClass::TimeManagerClass()
{
}

void TimeManagerClass::setTimeZone(const char* timeZone)
{
	setenv("TZ", timeZone, 1);
	tzset();
}

bool TimeManagerClass::needSync() const
{
	return LastSyncTime == 0 || time(nullptr) >= LastSyncTime + SNTP_UPDATE_DELAY;
}

void TimeManagerClass::syncStart(const std::vector<const char*>& sntpServers)
{
	assert(1 <= sntpServers.size() && sntpServers.size() <= 3);

	const char* sntpServer1 = sntpServers.size() >= 1 ? sntpServers[0] : nullptr;
	const char* sntpServer2 = sntpServers.size() >= 2 ? sntpServers[1] : nullptr;
	const char* sntpServer3 = sntpServers.size() >= 3 ? sntpServers[2] : nullptr;

	sntp_set_time_sync_notification_cb(TimeSyncNotificationCallback);
	configTzTime(getenv("TZ"), sntpServer1, sntpServer2, sntpServer3);
}

bool TimeManagerClass::syncStartAndWaitForSynced(const std::vector<const char*>& sntpServers, unsigned long timeout, const std::function<void(unsigned long)>& reportProgress)
{
	const int syncedCount = getSyncedCount();
	syncStart(sntpServers);
	return waitForSynced(syncedCount, timeout, reportProgress);
}

bool TimeManagerClass::isTimeValid() const
{
	return LastSyncTime != 0;
}

int TimeManagerClass::getSyncedCount() const
{
	return SyncedCount;
}

bool TimeManagerClass::waitForSynced(int syncedCount, unsigned long timeout, const std::function<void(unsigned long)>& reportProgress) const
{
    // First time.
    if (getSyncedCount() != syncedCount) return true;
    if (timeout == 0) return false;

    // 2nd time onwards.
    const unsigned long start = millis();
    for (unsigned long elapsed = millis() - start; elapsed < timeout; elapsed = millis() - start)
    {
        if (getSyncedCount() != syncedCount) return true;
        if (reportProgress) reportProgress(elapsed);
    }

    return false;
}
