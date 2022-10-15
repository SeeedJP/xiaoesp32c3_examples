/*
 * SmartConfig.ino
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
//   elapsedMillis 1.0.6 - https://github.com/pfeerick/elapsedMillis

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <WiFi.h>
#include <elapsedMillis.h>

////////////////////////////////////////////////////////////////////////////////
// Constants

#define SMARTCONFIG_PROTOCOL  (SC_TYPE_ESPTOUCH)                      // SmartConfig protocol {SC_TYPE_ESPTOUCH, SC_TYPE_ESPTOUCH_V2}
static char SMARTCONFIG_CRYPT_KEY[] = "0123456789012345";             // ESPTouch v2 crypt key, len should be 16.

static constexpr uint8_t ENTER_PROVISIONING_BUTTON_PIN = D9;
static constexpr uint8_t ENTER_PROVISIONING_BUTTON_PUSH_VALUE = LOW;  // {HIGH, LOW}
static constexpr unsigned long ENTER_PROVISIONING_TIME = 1000;        // [msec.]

////////////////////////////////////////////////////////////////////////////////
// setup and loop

void setup()
{
	Serial.begin(115200);
	delay(1000);
	Serial.println();
	Serial.println();

	////////////////////////////////////////
	// Enter provisioning?

  Serial.print("Enter provisioning?...");
  pinMode(ENTER_PROVISIONING_BUTTON_PIN, INPUT);
  bool enterProvisioning = true;
  for (elapsedMillis enterProvisioningElapsed; enterProvisioningElapsed < ENTER_PROVISIONING_TIME; )
  {
    if (digitalRead(ENTER_PROVISIONING_BUTTON_PIN) != ENTER_PROVISIONING_BUTTON_PUSH_VALUE)
    {
      enterProvisioning = false;
      break;
    }
    delay(10);
  }
  if (enterProvisioning)
  {
    Serial.println("Yes.");
  }
  else
  {
    Serial.println("No.");
  }

  if (enterProvisioning)
  {
    ////////////////////////////////////////
    // SmartConfig

    Serial.print("Start SmartConfig...");
#if SMARTCONFIG_PROTOCOL != SC_TYPE_ESPTOUCH_V2
    if (!WiFi.beginSmartConfig(SMARTCONFIG_PROTOCOL))
#else
    if (!WiFi.beginSmartConfig(SMARTCONFIG_PROTOCOL, SMARTCONFIG_CRYPT_KEY))
#endif // SMARTCONFIG_PROTOCOL
    {
      Serial.println("ERROR.");
      return;
    }
    else
    {
      Serial.println("Started.");

      Serial.print("Waiting for complete SmartConfig...");
      while (!WiFi.smartConfigDone())
      {
        Serial.print(".");
        delay(200);
      }
      Serial.println("Completed.");

      Serial.print("Stop SmartConfig...");
      if (!WiFi.stopSmartConfig())
      {
        Serial.println("ERROR.");
        return;
      }
      else
      {
        Serial.println("Stopped.");
      }
    }
  }
  else
  {
    ////////////////////////////////////////
    // Start Wi-Fi

    Serial.println("Start Wi-Fi.");
    WiFi.enableSTA(true);
		WiFi.begin();
  }

	////////////////////////////////////////
	// Wait to connect to Wi-Fi

  Serial.print("Wait to connect Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Connected.");

  Serial.print("SSID=");
  Serial.println(WiFi.SSID());
  Serial.print("IP address=");
  Serial.println(WiFi.localIP());
}

void loop()
{
  delay(1000);
}

////////////////////////////////////////////////////////////////////////////////
