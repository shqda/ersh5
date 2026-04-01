#include <Arduino.h>
#include <ESP8266WiFi.h>

const char *ssid = "micro_gad";
const char *password = "11111111";

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  delay(1000);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  server.begin();

  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String request = client.readStringUntil('\n');
  request.trim();

  if (request.indexOf("GET /ON") >= 0) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("ON");
  }

  if (request.indexOf("GET /OFF") >= 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("OFF");
  }

  while (client.available()) client.read();

  String html = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  body {
    font-family: Arial, sans-serif;
    background: #0f172a;
    color: white;
    text-align: center;
    margin: 0;
    padding: 40px;
  }

  h1 {
    margin-bottom: 30px;
    font-size: 28px;
  }

  .card {
    background: #1e293b;
    padding: 30px;
    border-radius: 16px;
    display: inline-block;
    box-shadow: 0 10px 25px rgba(0,0,0,0.4);
  }

  button {
    width: 120px;
    height: 50px;
    margin: 10px;
    border: none;
    border-radius: 10px;
    font-size: 16px;
    cursor: pointer;
    transition: 0.2s;
  }

  .on {
    background: #22c55e;
    color: white;
  }

  .off {
    background: #ef4444;
    color: white;
  }

  button:hover {
    transform: scale(1.05);
  }
</style>
</head>

<body>

<h1>ESP8266 Control Panel</h1>

<div class="card">
  <p>LED Control</p>

  <button class="on" onclick="location.href='/ON'">ON</button>
  <button class="off" onclick="location.href='/OFF'">OFF</button>
</div>

</body>
</html>
)rawliteral";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n\r\n");
  client.print(html);

  client.stop();
}