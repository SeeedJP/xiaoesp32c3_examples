[Japanese]

# MqttsPubClient

MqttsPubClientは、一定時間間隔にMQTTサーバー[test.mosquitto.org](https://test.mosquitto.org/)にJSONメッセージをパブリッシュ（送信）するサンプルコードです。

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
| `QOS` | MQTTに送信するときのQoS(Quality of Service)です。LWMQTT_QOS0, LWMQTT_QOS1, LWMQTT_QOS2のいずれかを指定します。 |

## トピック名とペイロード

### 一定時間間隔にパブリッシュ

* トピック名
  * dt/`APPLICATION_NAME`/`GROUP_NAME`/`DEVICE_NAME`/uptime
* ペイロード
  * {"uptime":`Uptime`,"rssi":`Wi-Fi RSSI`}

## MQTTメッセージのモニタリング

```
$ mosquitto_sub -h test.mosquitto.org -t "dt/mqtts-pub-client/test/#" -v -q 2
```

## シーケンス

```mermaid

sequenceDiagram
    participant main
    participant WiFi
    participant MQTTClient
    participant JsonDocument
    participant Arduino

    note left of main: Start Wi-Fi
    main->>WiFi: enableSTA(true)
    main->>WiFi: begin()

    note left of main: Wait to connect to Wi-Fi
    loop != WL_CONNECTED
      main->>WiFi: status()
    end

    note left of main: Sync clock with SNTP server
    main->>Arduino: configTzTime()
    loop == false
      Arduino-->>main: TimeSyncCompleted = true
    end

    loop
      note left of main: Connected to Wi-Fi?
      main->>WiFi: status()

      note left of main: Publish to MQTT server
      main->>MQTTClient: connect()
      main->>JsonDocument: Create JSON string
      main->>MQTTClient: publish()
      main->>MQTTClient: disconnect()

      note left of main: Wait for time
      main->>Arduino: delay()
    end
```

## 消費電流

| 処理 | 電気量[mC] | 電流[mA] | 時間[msec.] |
| :--- | ---: | ---: | ---: |
| MQTT通信 1回目 | 356.79 | 38.51 | 9264 |
| MQTT通信 2回目 | 205.19 | 37.09 | 5532 |
| delay | 1620 | 29.05 | 55740 |

**全体（MQTT通信を2回）:**  
<img src="../../../media/1.png" width="600">

**初回通信時（SNTP時刻同期あり）:**  
<img src="../../../media/2.png" width="600">

**2回目通信時（SNTP時刻同期なし）:**  
<img src="../../../media/3.png" width="600">

**delay時:**  
<img src="../../../media/4.png" width="600">

## 認証を相互認証に変更

[MutualAuthentication](/doc/MutualAuthentication.md)

## ライセンス

[MIT](LICENSE.txt)
