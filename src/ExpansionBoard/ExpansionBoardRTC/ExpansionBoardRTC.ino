/*
 * ExpansionBoardRTC.ino
 * Copyright (C) 2022 Takashi Matsuoka <matsujirushi@live.jp>
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// IDE:
//   Arduino 2.0.0
// Platform:
//   esp32 2.0.5 - https://github.com/espressif/arduino-esp32
// Board:
//   XIAO_ESP32C3
// Board Settings:
//   USB CDC On Boot: Enabled
//   CPU Frequency: 160MHz(WiFi)
//   Core Debug Level: None
//   Erase All Flash Before Sketch Upload: Disabled
//   Flash Frequency: 80MHz
//   Flash Mode: QIO
//   Flash Size: 4MB(32Mb)
//   Partition Scheme: Default 4MB with spiffs(1.2MB APP/1.5MB SPIFFS)
//   Upload Speed: 921600
// Libraries:
//   PCF8563 0.1.0 - https://github.com/Bill2462/PCF8563-Arduino-Library

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <PCF8563.h>

////////////////////////////////////////////////////////////////////////////////
// Constants

static constexpr uint8_t BUTTON_PIN = D1;

////////////////////////////////////////////////////////////////////////////////
// Variables

static PCF8563 Rtc;

////////////////////////////////////////////////////////////////////////////////
// setup and loop

void setup()
{
	Serial.begin(115200);
	delay(1000);
	Serial.println();
	Serial.println();

	pinMode(BUTTON_PIN, INPUT_PULLUP);
  Rtc.init();

	if (!digitalRead(BUTTON_PIN))
	{
		Serial.println("Set date and time on RTC.");

		Rtc.stopClock();
		Rtc.setYear(22);
		Rtc.setMonth(1);
		Rtc.setDay(1);
		Rtc.setHour(0);
		Rtc.setMinut(0);
		Rtc.setSecond(0);
		Rtc.startClock();
	}
}

void loop()
{
  Time now = Rtc.getTime();

	Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\r\n", now.year + 2000, now.month, now.day, now.hour, now.minute, now.second);

  delay(1000);
}

////////////////////////////////////////////////////////////////////////////////
