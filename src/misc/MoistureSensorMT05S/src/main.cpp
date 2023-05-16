/*
 * main.cpp
 * Copyright (C) 2023 MATSUOKA Takashi
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// Includes

#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <SeeedArduinoFramework.h>
#include <OneWire.h>

namespace saf = seeed_arduino_framework;

////////////////////////////////////////////////////////////////////////////////
// Constants

static const char APPLICATION_NAME[] = "moisture";
static const char GROUP_NAME[] = "test";
static const char DEVICE_NAME[] = "xiao1234";

static constexpr unsigned long SPIN_WAIT = 1;               // [msec.]
static constexpr unsigned long INTERVAL = 60;               // [sec.]

static const char WIFI_SSID[] = "";
static const char WIFI_PASSPHRASE[] = "";
static constexpr uint32_t WIFI_CONNECT_TIMEOUT = 30000;     // [msec.]

static const char TZ[] = "JST-9";
static const char SNTP_SERVER[] = "pool.ntp.org";
static constexpr unsigned long TIME_SYNC_TIMEOUT = 30000;   // [msec.]

static const char MQTT_SERVER[] = "test.mosquitto.org";
static constexpr uint16_t MQTT_SERVER_PORT = 8883;
static constexpr lwmqtt_qos_t QOS = LWMQTT_QOS0;            // MQTT Quality of Service {LWMQTT_QOS0, LWMQTT_QOS1, LWMQTT_QOS2}

////////////////////////////////////////////////////////////////////////////////
// Variables

static RTC_DATA_ATTR time_t StartTime = 0;

static WiFiClientSecure TcpClient;
static MQTTClient MqttClient;

static StaticJsonDocument<256> JsonDoc; // https://arduinojson.org/v6/assistant

static OneWire ds(2);  

////////////////////////////////////////////////////////////////////////////////
// Certificates

////////////////////////////////////////
// Common Name: mosquitto.org
// Organizational Name: Mosquitto
// Organizational Unit: CA
// From Date: 2020-06-09 20:06:39
// To Date: 2030-06-07 20:06:39
static const char SERVER_CA_CERT[] = \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIEAzCCAuugAwIBAgIUBY1hlCGvdj4NhBXkZ/uLUZNILAwwDQYJKoZIhvcNAQEL\n" \
    "BQAwgZAxCzAJBgNVBAYTAkdCMRcwFQYDVQQIDA5Vbml0ZWQgS2luZ2RvbTEOMAwG\n" \
    "A1UEBwwFRGVyYnkxEjAQBgNVBAoMCU1vc3F1aXR0bzELMAkGA1UECwwCQ0ExFjAU\n" \
    "BgNVBAMMDW1vc3F1aXR0by5vcmcxHzAdBgkqhkiG9w0BCQEWEHJvZ2VyQGF0Y2hv\n" \
    "by5vcmcwHhcNMjAwNjA5MTEwNjM5WhcNMzAwNjA3MTEwNjM5WjCBkDELMAkGA1UE\n" \
    "BhMCR0IxFzAVBgNVBAgMDlVuaXRlZCBLaW5nZG9tMQ4wDAYDVQQHDAVEZXJieTES\n" \
    "MBAGA1UECgwJTW9zcXVpdHRvMQswCQYDVQQLDAJDQTEWMBQGA1UEAwwNbW9zcXVp\n" \
    "dHRvLm9yZzEfMB0GCSqGSIb3DQEJARYQcm9nZXJAYXRjaG9vLm9yZzCCASIwDQYJ\n" \
    "KoZIhvcNAQEBBQADggEPADCCAQoCggEBAME0HKmIzfTOwkKLT3THHe+ObdizamPg\n" \
    "UZmD64Tf3zJdNeYGYn4CEXbyP6fy3tWc8S2boW6dzrH8SdFf9uo320GJA9B7U1FW\n" \
    "Te3xda/Lm3JFfaHjkWw7jBwcauQZjpGINHapHRlpiCZsquAthOgxW9SgDgYlGzEA\n" \
    "s06pkEFiMw+qDfLo/sxFKB6vQlFekMeCymjLCbNwPJyqyhFmPWwio/PDMruBTzPH\n" \
    "3cioBnrJWKXc3OjXdLGFJOfj7pP0j/dr2LH72eSvv3PQQFl90CZPFhrCUcRHSSxo\n" \
    "E6yjGOdnz7f6PveLIB574kQORwt8ePn0yidrTC1ictikED3nHYhMUOUCAwEAAaNT\n" \
    "MFEwHQYDVR0OBBYEFPVV6xBUFPiGKDyo5V3+Hbh4N9YSMB8GA1UdIwQYMBaAFPVV\n" \
    "6xBUFPiGKDyo5V3+Hbh4N9YSMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEL\n" \
    "BQADggEBAGa9kS21N70ThM6/Hj9D7mbVxKLBjVWe2TPsGfbl3rEDfZ+OKRZ2j6AC\n" \
    "6r7jb4TZO3dzF2p6dgbrlU71Y/4K0TdzIjRj3cQ3KSm41JvUQ0hZ/c04iGDg/xWf\n" \
    "+pp58nfPAYwuerruPNWmlStWAXf0UTqRtg4hQDWBuUFDJTuWuuBvEXudz74eh/wK\n" \
    "sMwfu1HFvjy5Z0iMDU8PUDepjVolOCue9ashlS4EB5IECdSR2TItnAIiIwimx839\n" \
    "LdUdRudafMu5T5Xma182OC0/u/xRlEm+tvKGGmfFcN0piqVl8OrSPBgIlb+1IKJE\n" \
    "m/XriWr/Cq4h/JfB7NTsezVslgkBaoU=\n" \
    "-----END CERTIFICATE-----\n";

static bool ConvertT(float& temperature, float& moisture, int& conductivity)
{
    //--------------------------------------------
    // Tested - Convert T and Read Scratchpad
    // Attention: 
    //--------------------------------------------
    Serial.print("Test Function Command- Convert T [0x44]  & Read Scratchpad [0xBE]: \n");
    byte scratchpad[9];

    Serial.print("Convert T [0x44]: ");
    ds.reset();     // Send RESET
    ds.skip();      // Send ROM Command-Skip ROM
    ds.write(0x44); // Send Function command- convert T

    // Wait conversion done (Whether to pull down the DQ line during operation can be configured in scratchpad CONFIG0-Bit3- SensorPowerMode)
    while (ds.read_bit() == 0)
    {
        delay(10);
        Serial.print(".");
    }
    Serial.print("Conversion Done\n");

    Serial.print("Read Scratchpad [0xBE]: ");
    ds.reset();     // Send RESET
    ds.skip();      // Send ROM Command-Skip ROM
    ds.write(0xBE); // Send Function command- Read Scratchpad
    for (int i = 0; i < 9; ++i) // we need 9 bytes
    {
        scratchpad[i] = ds.read();
        Serial.printf("%.2X ", scratchpad[i]);
    }
    Serial.printf("CrcCalculated=%.2X", OneWire::crc8(scratchpad, 8));
    Serial.println();

    if (OneWire::crc8(scratchpad, 8) != scratchpad[8])
    {
        Serial.print("CRC ERROR!\n");
        return false;
    }

    temperature = makeWord(scratchpad[0], scratchpad[1]) / 100.0;
    moisture = makeWord(scratchpad[2], scratchpad[3]) / 100.0;
    conductivity = static_cast<int16_t>(makeWord(scratchpad[4], scratchpad[5]));
    Serial.printf("TEMP=%.2f, MOISTURE=%.2f, CONDUCTIVITY=%d, CONFIG0=%.2X, CONFIG1=%.2X, CRC8=%.2X\n", temperature, moisture, conductivity, scratchpad[6], scratchpad[7], scratchpad[8]);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// setup and loop

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println();
    Serial.println();

    saf::Time::SetTimeZone(TZ);

    ////////////////////////////////////////
    // Configure

    Serial.println("TCP: Configure.");
    TcpClient.setCACert(SERVER_CA_CERT);

    Serial.println("MQTT: Configure.");
    MqttClient.begin(MQTT_SERVER, MQTT_SERVER_PORT, TcpClient);

    ////////////////////////////////////////
    // Start Wi-Fi

    Serial.println("WIFI: Start.");
    saf::WiFiStation.begin(WIFI_SSID, WIFI_PASSPHRASE);

    ////////////////////////////////////////
    // Wait to connect to Wi-Fi

    Serial.print("WIFI: Wait to connect...");
    if (!saf::WiFiStation.waitForConnected(WIFI_CONNECT_TIMEOUT, [](unsigned long elapsed){ delay(SPIN_WAIT); }))
    {
        Serial.println("ERROR.");
    }
    else
    {
        Serial.println("Connected.");

        ////////////////////////////////////////
        // Sync clock with SNTP server

        if (saf::TimeSync.needSync())
        {
            Serial.print("SNTP: Syncing clock...");
            saf::TimeSync.syncStart({ SNTP_SERVER });
            if (!saf::TimeSync.waitForSync(TIME_SYNC_TIMEOUT, [](unsigned long elapsed){ delay(SPIN_WAIT); }))
            {
                Serial.println("ERROR.");
            }
            else
            {
                Serial.println("Synced.");
            }
        }

        Serial.println(saf::Time::LocalTimeString(time(nullptr)));

        if (saf::TimeSync.isSyncedOnce())
        {
            ////////////////////////////////////////
            // Remember start time

            if (StartTime == 0)
            {
                StartTime = time(nullptr);
            }

            ////////////////////////////////////////
            // Capture telemetry data

            const time_t uptime = time(nullptr) - StartTime;
            float temperature;
            float moisture;
            int conductivity;
            if (!ConvertT(temperature, moisture, conductivity))
            {
                temperature = 0.0;
                moisture = 0.0;
                conductivity = 0;
            }

            ////////////////////////////////////////
            // Publish to MQTT server

            Serial.print("MQTT: Connect...");
            if (!MqttClient.connect(DEVICE_NAME))
            {
                Serial.println("ERROR.");
            }
            else
            {
                Serial.println("Connected.");

                Serial.println("MQTT: Publish.");

                String topic = String("dt/") + APPLICATION_NAME + "/" + GROUP_NAME + "/" + DEVICE_NAME + "/moisture";
                JsonDoc.clear();
                JsonDoc["uptime"] = uptime;
                JsonDoc["temperature"] = temperature;
                JsonDoc["moisture"] = moisture;
                JsonDoc["conductivity"] = conductivity;
                String payload;
                serializeJson(JsonDoc, payload);
                Serial.print("Payload=");
                Serial.println(payload);

                MqttClient.publish(topic, payload, false, QOS);

                Serial.println("MQTT: Disconnect.");
                MqttClient.disconnect();
            }
        }
    }

    ////////////////////////////////////////
    // Stop Wi-Fi

    Serial.println("WIFI: Stop.");
    saf::WiFiStation.end();

    ////////////////////////////////////////
    // Transition to deep sleep

    Serial.println("Transition to deep sleep.");
    delay(1000);
    const int sleepSec = StartTime == 0 ? 0 : INTERVAL - (time(nullptr) - StartTime) % INTERVAL;
    esp_sleep_enable_timer_wakeup(sleepSec * 1000 * 1000);
    esp_deep_sleep_start();
}

void loop()
{
    Serial.println("//-------------------------");
    Serial.println("// Start Testing MT05S     ");
    Serial.println("//-------------------------");

    float temperature;
    float moisture;
    int conductivity;
    ConvertT(temperature, moisture, conductivity);

    Serial.println();
    delay(1000);
}

////////////////////////////////////////////////////////////////////////////////
