#include <ESP32Ping.h>
#include "WiFi.h"
#include "main.h"
#include "Credentials.h"

void test_ping() {
  if (connect_wifi() == false) {
    Serial.println("WiFi not connected, cannot start PING");
    return;
  }
  Serial.printf("Local IP: %s\n", WiFi.localIP().toString().c_str());
  Serial.printf("WIFI Gateway: %s\n", WiFi.gatewayIP().toString().c_str());

  // Ping a remote host (e.g., Google's public DNS server) 3 times
  IPAddress ip(142, 250, 124, 113);  // bool success = Ping.ping("142.250.124.113", 3);

  bool success = Ping.ping(ip, 3);

  bool test_result = false;
  if (success) {
    test_result = true;
    Serial.println("Ping successful.");
  } else {
    Serial.println("Ping failed.");
  }
}

#define TEST_WIFI_SERVER "ftp.bom.gov.au"
#define TEST_WIFI_PORT 21
void test_WIFI() {
  WiFiClient client;

  int res;

  Serial.printf("Connecting to FTP server %s:%d...\n", TEST_WIFI_SERVER, TEST_WIFI_PORT);
  res = client.connect(TEST_WIFI_SERVER, TEST_WIFI_PORT);
  Serial.printf("Connect result: %d\n", res);
  if (res) {
    client.stop();
  }
}

WiFiClient httpTestClient;
IPAddress httpTestServer(142, 250, 124, 113);

void setup_http_test() {
  int status = WL_IDLE_STATUS;

  int wifi_connect_count = 0;
  Serial.begin(115200);
  Serial.println("Attempting to connect to WPA network...");

  status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wifi hotspot...");
  while ((status = WiFi.status()) != WL_CONNECTED) {
    Serial.print("o");
    delay(500);
    wifi_connect_count++;
    if (wifi_connect_count > 20) {
      break;
    }
  }

  if (status != WL_CONNECTED) {
    Serial.println("Couldn't get a wifi connection");
    // don't do anything else:
    while (true)
      ;
  } else {
    Serial.println("Connected to wifi");
    Serial.println("\nStarting connection...");
    // if you get a connection, report back via serial:
    if (httpTestClient.connect(httpTestServer, 80)) {
      Serial.println("connected");
      // Make a HTTP request:
      httpTestClient.println("GET /search?q=arduino HTTP/1.0");
      httpTestClient.println();
    }
  }
}

void loop_http_test() {
  if (httpTestClient.available()) {
    char c;
    while (httpTestClient.available() && ((c = httpTestClient.read()) >= 0)) {
      Serial.print(c);
    }
  }

  if (!httpTestClient.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    httpTestClient.stop();
    for (;;)
      ;
  }
}