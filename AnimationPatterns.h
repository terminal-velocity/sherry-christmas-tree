#pragma once

#include "SherryTree.h"

#include <Adafruit_NeoPixel.h>

extern uint8_t g_backBuffer[NUMBER_OF_PIXELS][3];

void rotateUp(Adafruit_NeoPixel&);
void rotateDown(Adafruit_NeoPixel&);

void rotateSmooth(Adafruit_NeoPixel&, float offset);
uint32_t lerpColor(float);
void directCopy(Adafruit_NeoPixel&);

void rainbowMode(int, Adafruit_NeoPixel&);