[Japanese]

# xiaoesp32c3_examples

[Seeed Studio XIAO ESP32C3](https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/)のサンプルコードです。

## Examples

| スケッチ名 | ディレクトリ | 説明 |
| :--- | :--- | :--- |
| [PingClient](src/WiFi/PingClient) | src/WiFi | 一定時間間隔にサーバー[www.google.com](https://www.google.com/)にPingしてネットワークの開通状況を確認します。 |
| [MqttsPubClient](src/WiFi/MqttsPubClient) | src/WiFi | 一定時間間隔にMQTTサーバー[test.mosquitto.org](https://test.mosquitto.org/)にJSONメッセージをパブリッシュ（送信）します。 |
| [MqttsPubClientLP](src/WiFi/MqttsPubClientLP) | src/WiFi | 一定時間間隔にMQTTサーバー[test.mosquitto.org](https://test.mosquitto.org/)にJSONメッセージをパブリッシュ（送信）します。 |
| [SmartConfig](src/WiFi/SmartConfig) | src/WiFi | Wi-FiアクセスポイントのSSIDとパスフレーズをスマートフォンからXIAO ESP32C3に設定します。 |
| [HttpsOTA](src/WiFi/HttpsOTA) | src/WiFi | Webサーバー[www.seeed.co.jp](https://www.seeed.co.jp/)に置いたファームウェアでXIAO ESP32C3のファームウェアをアップデートします。 |
