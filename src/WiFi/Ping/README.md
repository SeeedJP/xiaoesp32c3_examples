[Japanese]

# Ping

Pingは、一定時間間隔にサーバー[www.google.com](https://www.google.com/)にPingしてネットワークの開通状況を確認するサンプルコードです。

```mermaid
flowchart LR
    id1[XIAO ESP32C3]-- echo request -->id2[Server]
    id2-- echo reply -->id1

```

## コード内のパラメータ

| パラメータ名 | 説明 |
| :--- | :--- |
| `INTERVAL` | サーバーにPingする時間間隔です。（単純に、送信後の待ち時間として使うため、実際にPingする時間間隔は指定した値よりも長くなります。 |
| `WIFI_SSID` | 接続するWi-FiアクセスポイントのSSIDです。デバイスに記憶しているSSID/パスフレーズを使うときは空を指定します。 |
| `WIFI_PASSPHRASE` | 接続するWi-Fiアクセスポイントのパスフレーズです。 |
| `SERVER` | サーバー名です。 |

## ライセンス

[MIT](LICENSE.txt)
