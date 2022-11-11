/*
 * HttpsOTA.ino
 * Copyright (C) 2022 Seeed K.K.
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// IDE:
//   Arduino 2.0.1
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

#include <atomic>
#include <esp_sntp.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPUpdate.h>
#include "esp_ota_ops.h"
#include <elapsedMillis.h>

////////////////////////////////////////////////////////////////////////////////
// Constants

static const char WIFI_SSID[] = "";
static const char WIFI_PASSPHRASE[] = "";

static const char TZ[] = "JST-9";
static const char SNTP_SERVER[] = "pool.ntp.org";

static const char FIRMWARE[] = "https://www.seeed.co.jp/xiaoesp32c3_examples/HttpsOTA.ino.bin";

static constexpr uint8_t ENTER_FIRMWARE_UPDATE_BUTTON_PIN = D9;
static constexpr uint8_t ENTER_FIRMWARE_UPDATE_BUTTON_PUSH_VALUE = LOW; // {HIGH, LOW}
static constexpr unsigned long ENTER_FIRMWARE_UPDATE_TIME = 1000;       // [msec.]

////////////////////////////////////////////////////////////////////////////////
// Variables

static WiFiClientSecure TcpClient;

////////////////////////////////////////////////////////////////////////////////
// Certificates

static const char SERVER_CA_CERT[] = \
////////////////////////////////////////
// Common Name: ISRG Root X1
// Organizational Name: Internet Security Research Group
// Organizational Unit: 
// From Date: 2015-06-04 20:04:38
// To Date: 2035-06-04 20:04:38
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----\n";

////////////////////////////////////////////////////////////////////////////////
// TimeSyncNotificationCallback

static std::atomic_bool TimeSyncCompleted;

static void TimeSyncNotificationCallback(struct timeval* tv)
{
  if (sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED)
  {
    TimeSyncCompleted = true;
  }
}

////////////////////////////////////////////////////////////////////////////////
// setup and loop

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println();

  setenv("TZ", TZ, 1);
  tzset();

  ////////////////////////////////////////
  // Enter provisioning?

  Serial.print("Enter firmware update?...");
  pinMode(ENTER_FIRMWARE_UPDATE_BUTTON_PIN, INPUT);
  bool enterFirmwareUpdate = true;
  for (elapsedMillis enterFirmwareUpdateElapsed; enterFirmwareUpdateElapsed < ENTER_FIRMWARE_UPDATE_TIME; )
  {
    if (digitalRead(ENTER_FIRMWARE_UPDATE_BUTTON_PIN) != ENTER_FIRMWARE_UPDATE_BUTTON_PUSH_VALUE)
    {
      enterFirmwareUpdate = false;
      break;
    }
    delay(10);
  }
  if (enterFirmwareUpdate)
  {
    Serial.println("Yes.");
  }
  else
  {
    Serial.println("No.");
    return;
  }

  ////////////////////////////////////////
  // Configure

  Serial.println("TCP: Configure.");
  TcpClient.setCACert(SERVER_CA_CERT);

  ////////////////////////////////////////
  // Start Wi-Fi

  Serial.println("WIFI: Start.");
  WiFi.enableSTA(true);
  if (WIFI_SSID[0] != '\0')
  {
    WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);
  }
  else
  {
    WiFi.begin();
  }

  ////////////////////////////////////////
  // Wait to connect to Wi-Fi

  Serial.print("WIFI: Wait to connect...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(200);
  }
  Serial.println("Connected.");

  ////////////////////////////////////////
  // Sync clock with SNTP server

  Serial.print("SNTP: Syncing clock...");
  sntp_set_time_sync_notification_cb(TimeSyncNotificationCallback);
  TimeSyncCompleted = false;
  configTzTime(getenv("TZ"), SNTP_SERVER);

  while (!TimeSyncCompleted)
  {
    Serial.print('.');
    delay(200);
  }
  Serial.println("Synced.");

  ////////////////////////////////////////
  // Update firmware

  Serial.print("Update firmware...");
  httpUpdate.rebootOnUpdate(false);
  const t_httpUpdate_return result = httpUpdate.update(TcpClient, FIRMWARE);
  switch (result)
  {
    case HTTP_UPDATE_OK:
      Serial.println("Updated.");
      delay(1000);
      ESP.restart();
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("No update.");
      break;
    case HTTP_UPDATE_FAILED:
      Serial.println("ERROR.");
      break;
  }
}

void loop()
{
  ////////////////////////////////////////
  // Print running partition information

  const esp_partition_t* running = esp_ota_get_running_partition();

  Serial.print("Running partition Type=");
  switch (running->type)
  {
  case ESP_PARTITION_TYPE_APP:
    Serial.print("app");
    break;
  case ESP_PARTITION_TYPE_DATA:
    Serial.print("data");
    break;
  default:
    Serial.print("?");
    break;
  }

  Serial.print(" SubType=");
  switch (running->subtype)
  {
  case ESP_PARTITION_SUBTYPE_APP_FACTORY:
    Serial.print("factory");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_0:
    Serial.print("ota_0");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_1:
    Serial.print("ota_1");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_2:
    Serial.print("ota_2");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_3:
    Serial.print("ota_3");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_4:
    Serial.print("ota_4");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_5:
    Serial.print("ota_5");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_6:
    Serial.print("ota_6");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_7:
    Serial.print("ota_7");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_8:
    Serial.print("ota_8");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_9:
    Serial.print("ota_9");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_10:
    Serial.print("ota_10");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_11:
    Serial.print("ota_11");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_12:
    Serial.print("ota_12");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_13:
    Serial.print("ota_13");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_14:
    Serial.print("ota_14");
    break;
  case ESP_PARTITION_SUBTYPE_APP_OTA_15:
    Serial.print("ota_15");
    break;
  default:
    Serial.print("?");
    break;
  }

  Serial.println();

  delay(5000);
}

////////////////////////////////////////////////////////////////////////////////
