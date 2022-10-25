/*
 * ExpansionBoardButton.ino
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

////////////////////////////////////////////////////////////////////////////////
// Constants

static constexpr uint8_t BUTTON_PIN = D1;

////////////////////////////////////////////////////////////////////////////////
// setup and loop

void setup()
{
	Serial.begin(115200);
	delay(1000);
	Serial.println();
	Serial.println();

	pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop()
{
	if (digitalRead(BUTTON_PIN))
	{
		// Button is released.
		Serial.print('.');
	}
	else
	{
		// Button is pressed.
		Serial.print('*');
	}
	delay(200);
}

////////////////////////////////////////////////////////////////////////////////
