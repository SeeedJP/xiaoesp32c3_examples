/*
 * WiFiStation.hpp
 * Copyright (C) 2022 Takashi Matsuoka <matsujirushi@live.jp>
 * MIT License
 */
#pragma once

#include <cstdint>
#include <functional>

class WiFiStationClass
{
public:
    WiFiStationClass();
    ~WiFiStationClass();

    void begin(const char* ssid = nullptr, const char* passphrase = nullptr);
    void end();

    bool isConnected() const;
    bool waitForConnected(unsigned long timeout = 0, const std::function<void(unsigned long)>& reportProgress = nullptr) const;

    int8_t RSSI() const;

};

extern WiFiStationClass WiFiStation;
