[Japanese]

# MqttsPubClientLP

MqttsPubClientLPは、一定時間間隔にMQTTサーバー[test.mosquitto.org](https://test.mosquitto.org/)にJSONメッセージをパブリッシュ（送信）するサンプルコードです。

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
    participant WiFi
    participant PubSubClient
    participant StaticJsonDocument
    participant TIME
    participant ESP

    note right of main: Start Wi-Fi
    main->>WiFi: mode(WIFI_STA)
    main->>WiFi: begin()

    note right of main: Wait to connect to Wi-Fi
    loop != WL_CONNECTED
        main->>WiFi: status()
    end

    note right of main: Sync clock with SNTP server
    main->>TIME: configTime()
    TIME-->>main: Sync completed

    note right of main: Create JSON string
    main->>StaticJsonDocument: Create JSON string
    StaticJsonDocument-->>main: JSON string

    note right of main: Publish to MQTT server
    main->>PubSubClient: connect()
    main->>PubSubClient: publish()
    main->>PubSubClient: disconnect()

    note right of main: Stop Wi-Fi
    main->>WiFi: mode(WIFI_MODE_NULL)

    note right of main: Transition to deep sleep
    main->>ESP: esp_sleep_enable_timer_wakeup()
    main->>ESP: esp_deep_sleep_start()
```

## 消費電流

| 処理 | 電気量[mC] | 電流[mA] | 時間[msec.] |
| :--- | ---: | ---: | ---: |
| MQTT通信 1回目（SNTPあり） | 269.46 | 37.66 | 7154 |
| MQTT通信 2回目（SNTPなし） | 148.97 | 39.36 | 3785 |
| ディープスリープ | 7.27 | 0.13 | 56770 |

**全体（MQTT通信を2回）:**  
<img src="media/1.png" width="600">

**初回通信時（SNTP時刻同期あり）:**  
<img src="media/2.png" width="600">

**2回目通信時（SNTP時刻同期なし）:**  
<img src="media/3.png" width="600">

**ディープスリープ時:**  
<img src="media/4.png" width="600">

## ライセンス

[MIT](LICENSE.txt)
