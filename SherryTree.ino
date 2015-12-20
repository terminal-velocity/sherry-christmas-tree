#include "SherryTree.h"

#include "AnimationPatterns.h"

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include "WiFiManager.h"
#include "PubSubClient.h"

volatile static bool displayData = true;
void toggleDisplay(void) { 
	displayData = !displayData;
}

// Which pin on the Arduino is connected to the WS2812Bs?
const int LED_STRIP_PIN{ D3 };

// How long between each animation frame?
const int FRAME_DURATION{ 33 };

// Connection details for our MQTT protocol server.
const char * const REMOTE_MQTT_HOST{ "192.168.3.97" };
const unsigned int REMOTE_MQTT_PORT{ 1883 };

// Buffer & manager for the WS2812Bs
Adafruit_NeoPixel g_pixels{ NUMBER_OF_PIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800 };
// Automatic configuration host in the event of a missing SSID.
WiFiManager g_wifi_manager{};
// Wifi client device for communicating.
WiFiClient g_wifi_client{};
// Protocol manager for the wifi client.
PubSubClient g_pubsub_client{ g_wifi_client };

void setup() {
	// Debug text over slow serial.
	Serial.begin(9600);
	Serial.println("Booting up.");

	g_pixels.begin(); // This initializes the NeoPixel library.

	// Launches an AP for the user to apply configuration.
	g_wifi_manager.autoConnect("AP-Sherry");

	Serial.println(WiFi.localIP());

	// Setup the connection to the server.
	g_pubsub_client.setServer(REMOTE_MQTT_HOST, REMOTE_MQTT_PORT);
	g_pubsub_client.setCallback(callback);

	pinMode(D5, INPUT_PULLUP);
	attachInterrupt(D5, toggleDisplay, FALLING);
}

/// Set all the pixels to one color.
void setAll(Adafruit_NeoPixel& pix, uint8_t r, uint8_t g, uint8_t b) {
	for (int i = 0; i < pix.numPixels(); i++) {
		pix.setPixelColor(i, r, g, b);
	}
}


void loop() {
	// The timestamp of the last iteration.
	static long int lastTime = -1;
	// Workaround for https://github.com/esp8266/Arduino/issues/500
	if (lastTime == -1) lastTime = millis();

	// The time left before a refresh.
	static long int timeLeft = FRAME_DURATION;

	// Make sure we can get out data!
	if (!g_pubsub_client.connected()) {
		connect_mqtt();
	}
	g_pubsub_client.loop();

	// Timing
	if (timeLeft < 0) {
		if (displayData) {
			directCopy(g_pixels);
		}
		else {
			rainbowMode(FRAME_DURATION, g_pixels);
		}
		g_pixels.show();

		timeLeft += FRAME_DURATION;

		//rotateUp(g_pixels);

		pinMode(D4, OUTPUT);
		digitalWrite(D4, !digitalRead(D4));
	}
	if (timeLeft < -500) { timeLeft = 0; }
	long int currentTime = millis();
	timeLeft -= (currentTime - lastTime);
	lastTime = currentTime;

}

/// Called every time we get a message from our MQTT server.
void callback(char* topic, byte* message, unsigned int length) {
	Serial.print("Message recieved (");
	Serial.print(topic);
	Serial.println(")");

	// Blank the pixels.
	g_pixels.clear();

	// Black magic
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
				g_backBuffer[ledCount][0] = colours[0];
				g_backBuffer[ledCount][1] = colours[1];
				g_backBuffer[ledCount++][2] = colours[2];
			}
		}
		else {
			buffer.concat(current);
		}
	}

}

/// Retry until we get a working connection onto our previously configured MQTT server.
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
