/*
 * TimeManager.hpp
 * Copyright (C) 2022 Takashi Matsuoka <matsujirushi@live.jp>
 * MIT License
 */
#pragma once

#include <functional>
#include <vector>

class TimeManagerClass
{
public:
    TimeManagerClass();

    void setTimeZone(const char* timeZone);

    bool needSync() const;
    void syncStart(const std::vector<const char*>& sntpServers);
    bool syncStartAndWaitForSynced(const std::vector<const char*>& sntpServers, unsigned long timeout = 0, const std::function<void(unsigned long)>& reportProgress = nullptr);
    bool isTimeValid() const;

private:
    int getSyncedCount() const;
    bool waitForSynced(int syncCount, unsigned long timeout = 0, const std::function<void(unsigned long)>& reportProgress = nullptr) const;

};

extern TimeManagerClass TimeManager;
