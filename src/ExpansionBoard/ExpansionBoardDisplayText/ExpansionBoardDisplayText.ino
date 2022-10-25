/*
 * ExpansionBoardDisplayText.ino
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
//   U8g2 2.33.15 - https://github.com/olikraus/U8g2_Arduino

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <U8x8lib.h>

////////////////////////////////////////////////////////////////////////////////
// Constants

static const char* MESSAGES[] =
{
// 0         1
// 0123456789012345
	 "Arduino",
	 "Raspberry Pi",
	 "BeagleBone",
	 "ODYSSEY",
	 "XIAO",
	 "Wio Terminal",
	 "reTerminal",
	 "reServer",
	 "ReSpeaker",
	 "micro:bit",
	 "Wio Tracker",
	 "Arch",
};

////////////////////////////////////////////////////////////////////////////////
// Variables

static U8X8_SSD1306_128X64_NONAME_HW_I2C U8x8(SCL, SDA);  // 16x8 characters
static int LoopCount = 0;

////////////////////////////////////////////////////////////////////////////////
// setup and loop

void setup()
{
	////////////////////////////////////////
	// Initialize

	U8x8.begin();
	U8x8.setFlipMode(0);
	U8x8.setFont(u8x8_font_chroma48medium8_r);

	////////////////////////////////////////
	// Banner

	U8x8.drawString(1, 3, " Seeed Studio ");
	delay(2000);

	////////////////////////////////////////
	// Title

	U8x8.clear();
	U8x8.setInverseFont(1);
	U8x8.drawString(0, 0, "  Seeed Studio  ");
	U8x8.setInverseFont(0);
}

void loop()
{
	const int y = 1 + LoopCount % (U8x8.getRows() - 1);

	U8x8.clearLine(y);
	U8x8.drawString(1, y, MESSAGES[LoopCount % std::extent<decltype(MESSAGES)>::value]);
	delay(1000);

	++LoopCount;
}

////////////////////////////////////////////////////////////////////////////////
