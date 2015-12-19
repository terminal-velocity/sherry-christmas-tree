#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include "WiFiManager.h"
#include "PubSubClient.h"

#include "StringStream.h"

extern "C" {
#include <user_interface.h>
}

// Which pin on the Arduino is connected to the WS2812Bs?
const int PIN = D3;

const char *MQTT_HOST = "192.168.3.97";
int MQTT_PORT = 1883;

// How many NeoPixels are attached to the Arduino?
const int PIXEL_COUNT = 50;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIN, NEO_GRB + NEO_KHZ800);

const int TIME_DELAY = 800; // delay for half a second

WiFiManager wifiManage;

WiFiClient wfClient;
PubSubClient pubSub(wfClient);

void setAll(Adafruit_NeoPixel&, uint8_t, uint8_t, uint8_t);
int readInteger(Stream&, char);
void callback(char*, byte*, unsigned int);
void connect_mqtt(void);

void setup() {
	Serial.begin(9600);
	Serial.println("Booting up.");

	pixels.begin(); // This initializes the NeoPixel library.
	wifiManage.autoConnect("AP-Christmas");

	
	Serial.println(WiFi.localIP());

	pubSub.setServer(MQTT_HOST, MQTT_PORT);
	pubSub.setCallback(callback);
}

void setAll(Adafruit_NeoPixel& pix, uint8_t r, uint8_t g, uint8_t b) {
	for (int i = 0; i < PIXEL_COUNT; i++) {
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
	if (!pubSub.connected()) {
		connect_mqtt();
	}
	pubSub.loop();

	pixels.show();

	pinMode(D4, OUTPUT);
	digitalWrite(D4, !digitalRead(D4));
	delay(75);

	size_t freemem = system_get_free_heap_size();
	if (freemem < 10 * 1024) {
		Serial.print("WARN: Only ");
		Serial.print(freemem);
		Serial.println(" free bytes of heap!");
	}
}

void callback(char* topic, byte* message, unsigned int length) {
	Serial.print("Message recieved (");
	Serial.print(topic);
	Serial.println(")");

	pixels.clear();

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
				pixels.setPixelColor(ledCount++, colours[0], colours[1], colours[2]);
			}
		}
		else {
			buffer.concat(current);
		}
	}

}

void connect_mqtt(void) {
	while (!pubSub.connected()) {
		Serial.print("Attempting to connect to MQTT on ");
		Serial.print(MQTT_HOST);
		Serial.print(":");
		Serial.println(MQTT_PORT);

		if (pubSub.connect("ChristmasTree")) {
			Serial.println("Connection successful!");

			pubSub.subscribe("led");
			pubSub.publish("connected", "Connected OK");
		}
		else {
			Serial.print("Connection failure:");
			Serial.println(pubSub.state());

			Serial.println("Preparing for retry");
			delay(2500);
		}
	}

	
}