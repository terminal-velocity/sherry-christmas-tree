#pragma once

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include "WiFiManager.h"
#include "PubSubClient.h"

extern const int LED_STRIP_PIN;

extern const char * const REMOTE_MQTT_HOST;
extern const unsigned int REMOTE_MQTT_PORT;

extern const unsigned int NUMBER_OF_PIXELS;

extern const int FRAME_DURATION;

extern Adafruit_NeoPixel g_pixels;
extern WiFiManager g_wifi_manager;
extern WiFiClient  g_wifi_client;
extern PubSubClient g_pubsub_client;

void setAll(Adafruit_NeoPixel&, uint8_t, uint8_t, uint8_t);
void callback(char*, byte*, unsigned int);
void connect_mqtt(void);

void setup(void);
void loop(void);