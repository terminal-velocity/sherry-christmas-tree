#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include "WiFiManager.h"
#include "PubSubClient.h"

// Which pin on the Arduino is connected to the WS2812Bs?
const int PIN = D3;

char *MQTT_HOST = "0.0.0.0";
int MQTT_PORT = 1337;

// How many NeoPixels are attached to the Arduino?
const int PIXEL_COUNT = 50;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIN, NEO_GRB + NEO_KHZ800);

const int TIME_DELAY = 800; // delay for half a second

WiFiManager wifiManage;

WiFiClient wfClient;
PubSubClient pubSub(wfClient);

void setAll(Adafruit_NeoPixel&, uint8_t, uint8_t, uint8_t);
int readInteger(Stream&, char);

void setup() {

  pixels.begin(); // This initializes the NeoPixel library.
  wifiManage.autoConnect("AP-Christmas");

  Serial.begin(9600);
  Serial.println(WiFi.localIP());
}

void setAll(Adafruit_NeoPixel& pix, uint8_t r, uint8_t g, uint8_t b) {
	for (int i = 0; i < PIXEL_COUNT; i++) {
		pix.setPixelColor(i, r, g, b);
	}
}

int readInteger(Stream& s, char delim) {
	while (!s.available()) {}

	String str = s.readStringUntil(delim);
	s.print(str);

	str.trim();
	return str.toInt();
}

void loop() {
	

}
