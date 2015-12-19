#include "SherryTree.h"

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include "WiFiManager.h"
#include "PubSubClient.h"

#include "StringStream.h"

// Which pin on the Arduino is connected to the WS2812Bs?
const int LED_STRIP_PIN{ D3 };

const char * const REMOTE_MQTT_HOST{ "192.168.3.97" };
const unsigned int REMOTE_MQTT_PORT{ 1883 };

// How many NeoPixels are attached to the Arduino?
const unsigned int NUMBER_OF_PIXELS{ 50 };

Adafruit_NeoPixel g_pixels{ NUMBER_OF_PIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800 };
WiFiManager g_wifi_manager{};
WiFiClient g_wifi_client{};
PubSubClient g_pubsub_client{ g_wifi_client };

void setup() {
	Serial.begin(9600);
	Serial.println("Booting up.");

	g_pixels.begin(); // This initializes the NeoPixel library.
	g_wifi_manager.autoConnect("AP-Christmas");

	Serial.println(WiFi.localIP());

	g_pubsub_client.setServer(REMOTE_MQTT_HOST, REMOTE_MQTT_PORT);
	g_pubsub_client.setCallback(callback);
}

void setAll(Adafruit_NeoPixel& pix, uint8_t r, uint8_t g, uint8_t b) {
	for (int i = 0; i < NUMBER_OF_PIXELS; i++) {
		pix.setPixelColor(i, r, g, b);
	}
}

int readInteger(Stream& s, char delim) {
	while (!s.available()) {}

	String str = s.readStringUntil(delim);
	//s.read();
	s.print(str);

	str.trim();
	return str.toInt();
}

void loop() {
	if (!g_pubsub_client.connected()) {
		connect_mqtt();
	}
	g_pubsub_client.loop();

	g_pixels.show();

	pinMode(D4, OUTPUT);
	digitalWrite(D4, !digitalRead(D4));
	delay(75);
}

void callback(char* topic, byte* message, unsigned int length) {
	Serial.print("Message recieved (");
	Serial.print(topic);
	Serial.println(")");

	g_pixels.clear();

	String buffer = "";
	uint8_t colours[3];
	int channelCount = 0;
	int ledCount = 0;
	for (size_t i = 0; i < length; i++) {
		char current = (char)message[i];

		if (current == ',') {
			colours[channelCount++] = buffer.toInt();
			buffer = "";

			if (channelCount == 3) {
				channelCount = 0;
				g_pixels.setPixelColor(ledCount++, colours[0], colours[1], colours[2]);
			}
		}
		else {
			buffer.concat(current);
		}
	}

}

void connect_mqtt(void) {
	while (!g_pubsub_client.connected()) {
		Serial.print("Attempting to connect to MQTT on ");
		Serial.print(REMOTE_MQTT_HOST);
		Serial.print(":");
		Serial.println(REMOTE_MQTT_PORT);

		if (g_pubsub_client.connect("ChristmasTree")) {
			Serial.println("Connection successful!");

			g_pubsub_client.subscribe("led");
			g_pubsub_client.publish("connected", "Connected OK");
		}
		else {
			Serial.print("Connection failure:");
			Serial.println(g_pubsub_client.state());

			Serial.println("Preparing for retry");
			delay(2500);
		}
	}

	
}