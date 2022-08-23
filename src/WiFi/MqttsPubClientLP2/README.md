[Japanese]

# MqttsPubClientLP2

MqttsPubClientLP2は、一定時間間隔にMQTTサーバー[test.mosquitto.org](https://test.mosquitto.org/)にJSONメッセージをパブリッシュ（送信）するサンプルコードです。

```mermaid
flowchart LR
    id1[XIAO ESP32C3]-- JSON message -->id2[MQTT Server]
```

## コード内のパラメータ

| パラメータ名 | 説明 |
| :--- | :--- |
| `APPLICATION_NAME` | パブリッシュするトピック名に使います。 |
| `GROUP_NAME` | パブリッシュするトピック名に使います。 |
| `DEVICE_NAME` | MQTT接続時のクライアントIDと、パブリッシュするトピック名に使います。 |
| `INTERVAL` | MQTTサーバーにパブリッシュする時間間隔です。 |
| `WIFI_SSID` | 接続するWi-FiアクセスポイントのSSIDです。デバイスに記憶しているSSID/パスフレーズを使うときは空を指定します。 |
| `WIFI_PASSPHRASE` | 接続するWi-Fiアクセスポイントのパスフレーズです。 |
| `MQTT_SERVER` | MQTTサーバー名です。 |
| `MQTT_SERVER_PORT` | MQTTサーバーに接続するポート番号です。 |

## トピック名とペイロード

### 一定時間間隔にパブリッシュ

* トピック名
  * dt/`APPLICATION_NAME`/`GROUP_NAME`/`DEVICE_NAME`/uptime
* ペイロード
  * {"uptime":`Uptime`,"rssi":`Wi-Fi RSSI`}

## MQTTメッセージのモニタリング

```
$ mosquitto_sub -h test.mosquitto.org -t "dt/mqtts-pub-client/test/#" -v
```

## シーケンス

```mermaid

sequenceDiagram
    participant main
    participant WiFiStation
    participant PubSubClient
    participant StaticJsonDocument
    participant TimeManager
    participant ESP

    note right of main: Start Wi-Fi
    main->>WiFiStation: begin()

    note right of main: Wait to connect to Wi-Fi
    main->>WiFiStation: waitForConnected()

    note right of main: Sync clock with SNTP server
    main->>TimeManager: syncStartAndWaitForSynced()

    note right of main: Create JSON string
    main->>StaticJsonDocument: Create JSON string
    StaticJsonDocument-->>main: JSON string

    note right of main: Publish to MQTT server
    main->>PubSubClient: connect()
    main->>PubSubClient: publish()
    main->>PubSubClient: disconnect()

    note right of main: Stop Wi-Fi
    main->>WiFiStation: end()

    note right of main: Transition to deep sleep
    main->>ESP: esp_sleep_enable_timer_wakeup()
    main->>ESP: esp_deep_sleep_start()
```

## ライセンス

[MIT](LICENSE.txt)
