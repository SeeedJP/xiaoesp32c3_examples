[Japanese]

# 相互認証

MqttsPubClientとMqttsPubClientLPは相互認証に変更することができます。

## コードの変更

1. `MUTUAL_AUTHENTICATION`を`(1)`に変更します。
2. `CLIENT_CERT`にクライアント証明書を設定します。
3. `CLIENT_KEY`にクライアント秘密鍵を設定します。

また、接続するサーバーに応じて、`MQTT_SERVER`と`MQTT_SERVER_PORT`、`SERVER_CA_CERT`を変更してください。

## MQTTメッセージのモニタリング

```
$ mosquitto_sub -h <hostname> -p 8884 --capath . --cafile ca.crt  --cert client.crt --key client.key -t "dt/mqtts-pub-client/test/#" -v -q 2
```

## MQTTサーバーのセットアップ

> 参考：[演習 - プライベート MQTT ブローカーを配置する](https://docs.microsoft.com/ja-jp/learn/modules/altair-azure-sphere-deploy-mqtt-broker/03-exercise-set-up)

```
sudo apt update && sudo apt -y upgrade
sudo apt install -y mosquitto mosquitto-clients

mkdir -p ~/mosquitto_certs && cd ~/mosquitto_certs

openssl req -new -x509 -days 730 -nodes -extensions v3_ca -keyout ca.key -out ca.crt

openssl genrsa -out server.key 2048 && \
openssl req -new -out server.csr -key server.key -subj "/CN=$CommonName" && \
openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 730 && \
openssl rsa -in server.key -out server.key

openssl genrsa -out client.key 2048 && \
openssl req -new -out client.csr -key client.key -subj "/CN=$CommonName" && \
openssl x509 -req -in client.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out client.crt -days 730 && \
openssl rsa -in client.key -out client.key

sudo chmod a+r *.key

sudo cp ca.crt /etc/mosquitto/ca_certificates &&
sudo cp server.crt /etc/mosquitto/ca_certificates &&
sudo cp server.key /etc/mosquitto/ca_certificates

sudo sh -c "cat > /etc/mosquitto/conf.d/default.conf" << 'EOL'
per_listener_settings true

listener 1883 localhost
allow_anonymous true

listener 8884
allow_anonymous false
cafile /etc/mosquitto/ca_certificates/ca.crt
keyfile /etc/mosquitto/ca_certificates/server.key
certfile /etc/mosquitto/ca_certificates/server.crt
require_certificate true
use_identity_as_username true
tls_version tlsv1.2
EOL

sudo systemctl enable mosquitto && sudo systemctl start mosquitto
```
