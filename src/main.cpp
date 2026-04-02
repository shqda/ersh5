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
    background: #1a1a1a;
    color: #d4af37;
  }

  .card {
    background: #262626;
    border: 1px solid #3d3d3d;
  }

  .on {
    background: #d4af37;
    color: #1a1a1a;
  }

  .off {
    background: #404040;
    color: #a6a6a6;
  }

</style>
</head>
<body>

<h1></h1>

<div class="card">
  <div class="status-text"></div>

  <div class="btn-group">
    <button class="on" onclick="location.href='/ON'">on</button>
    <button class="off" onclick="location.href='/OFF'">off</button>
  </div>
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