/*
 * WiFiStation.cpp
 * Copyright (C) 2022 Takashi Matsuoka <matsujirushi@live.jp>
 * MIT License
 */
#include "WiFiStation.hpp"
#include <WiFi.h>

WiFiStationClass::WiFiStationClass()
{
}

WiFiStationClass::~WiFiStationClass()
{
    end();
}

void WiFiStationClass::begin(const char* ssid, const char* passphrase)
{
    WiFi.enableSTA(true);
    if (ssid != nullptr && ssid[0] != '\0')
    {
        WiFi.begin(ssid, passphrase);
    }
    else
    {
        WiFi.begin();
    }
}

void WiFiStationClass::end()
{
    WiFi.enableSTA(false);
}

bool WiFiStationClass::isConnected() const
{
    return WiFi.isConnected();
}

bool WiFiStationClass::waitForConnected(unsigned long timeout, const std::function<void(unsigned long)>& reportProgress) const
{
    // First time.
    if (isConnected()) return true;
    if (timeout == 0) return false;

    // 2nd time onwards.
    const unsigned long start = millis();
    for (unsigned long elapsed = millis() - start; elapsed < timeout; elapsed = millis() - start)
    {
        if (isConnected()) return true;
        if (reportProgress) reportProgress(elapsed);
    }

    return false;
}

int8_t WiFiStationClass::RSSI() const
{
    return WiFi.RSSI();
}
