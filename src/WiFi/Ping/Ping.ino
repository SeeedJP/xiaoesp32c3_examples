/*
 * Ping.ino
 * Copyright (C) 2022 Takashi Matsuoka <matsujirushi@live.jp>
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// IDE:
//   Arduino 1.8.19
// Platform:
//   esp32 2.0.4 - https://github.com/espressif/arduino-esp32
// Board:
//   XIAO_ESP32C3
// Board Settings:
//   Upload Speed: 921600
//   USB CDC On Boot: Enabled
//   CPU Frequency: 160MHz(WiFi)
//   Flash Frequency: 80MHz
//   Flash Mode: QIO
//   Flash Size: 4MB(32Mb)
//   Partition Scheme: Default 4MB with spiffs(1.2MB APP/1.5MB SPIFFS)
//   Core Debug Level: None
// Libraries:
//   ESP32Ping 1.6 - https://github.com/marian-craciunescu/ESP32Ping

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <WiFi.h>
#include <ESP32Ping.h>

////////////////////////////////////////////////////////////////////////////////
// Constants

static constexpr unsigned long INTERVAL = 3000;	// [msec.]

static const char WIFI_SSID[] = "";
static const char WIFI_PASSPHRASE[] = "";

static const char SERVER[] = "www.google.com";

////////////////////////////////////////////////////////////////////////////////
// setup and loop

void setup()
{
	Serial.begin(115200);
	delay(1000);
	Serial.println();
	Serial.println();

	////////////////////////////////////////
	// Start Wi-Fi

	Serial.println("WIFI: Start.");
	WiFi.mode(WIFI_STA);
	if (WIFI_SSID[0] != '\0')
	{
		WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);
	}
	else
	{
		WiFi.begin();
	}
}

void loop()
{
	static int count = 0;

	////////////////////////////////////////
	// Check Wi-Fi connection status

	const bool wifiStatus = WiFi.status() == WL_CONNECTED;

	////////////////////////////////////////
	// Execute ping

	const bool pingResult = !wifiStatus ? false : Ping.ping(SERVER, 1);
	const float pingTime = !pingResult ? 0.f : Ping.averageTime();

	////////////////////////////////////////
	// Print

	Serial.print(count);
	Serial.print('\t');
	Serial.print(wifiStatus ? 1 : 0);
	Serial.print('\t');
	Serial.print(pingResult ? 1 : 0);
	Serial.print('\t');
	Serial.println(pingTime);
	count++;

	delay(INTERVAL);
}

////////////////////////////////////////////////////////////////////////////////
