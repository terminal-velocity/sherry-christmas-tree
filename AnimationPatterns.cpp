#include "AnimationPatterns.h"

#include <Adafruit_NeoPixel.h>

extern float floorf(float);

uint8_t g_backBuffer[NUMBER_OF_PIXELS][3];

void rotateDown(Adafruit_NeoPixel& pix) {
	uint32_t bottomPixel = pix.getPixelColor(0);
	for (int i = 0; i < pix.numPixels() - 1; i++) {
		pix.setPixelColor(i, pix.getPixelColor(i + 1));
	}
	pix.setPixelColor(pix.numPixels() - 1, bottomPixel);
}

void rotateUp(Adafruit_NeoPixel& pix) {
	uint32_t topPixel = pix.getPixelColor(pix.numPixels() - 1);
	for (int i = pix.numPixels() - 1; i > 0; i--) {
		pix.setPixelColor(i, pix.getPixelColor(i - 1));
	}
	pix.setPixelColor(0, topPixel);
}

void rotateSmooth(Adafruit_NeoPixel& target, float offset) {
	for (int i = 0; i < NUMBER_OF_PIXELS; i++) {
		float fi = i + offset;
		uint32_t value = lerpColor(fi);
		target.setPixelColor(i, value);
	}
}

uint32_t lerpColor(float index) {
	float floored = floorf(index);
	float prop = index - floored;
	int lower = (int)floored;
	int upper = lower + 1;

	lower %= NUMBER_OF_PIXELS;
	upper %= NUMBER_OF_PIXELS;

	uint8_t lowerR = g_backBuffer[lower][0];
	uint8_t lowerG = g_backBuffer[lower][1];
	uint8_t lowerB = g_backBuffer[lower][2];

	uint8_t upperR = g_backBuffer[upper][0];
	uint8_t upperG = g_backBuffer[upper][1];
	uint8_t upperB = g_backBuffer[upper][2];

	uint8_t resultR = (int) ((lowerR * (1.0 - prop)) + (upperR * prop));
	uint8_t resultG = (int)((lowerG * (1.0 - prop)) + (upperG * prop));
	uint8_t resultB = (int)((lowerB * (1.0 - prop)) + (upperB * prop));

	return Adafruit_NeoPixel::Color(resultR, resultG, resultB);
}

void directCopy(Adafruit_NeoPixel& pix) {
	for (int i = 0; i < NUMBER_OF_PIXELS; i++) {
		g_pixels.setPixelColor(i, g_backBuffer[i][0], g_backBuffer[i][1], g_backBuffer[i][2]);
	}
}

const int GAP = 600;
void rainbowMode(int ms, Adafruit_NeoPixel& target) {
	static unsigned int phase = 0;

	phase += ms;

	for (int led = 0; led < NUMBER_OF_PIXELS; led++) {
		int localPhase = phase + (2 * GAP * led) / NUMBER_OF_PIXELS;
		localPhase %= GAP * 6;

		uint8_t r, g, b;
		if (localPhase < GAP) {
			r = 255;
			g = map(localPhase % GAP, 0, GAP, 0, 255);
			b = 0;
		}
		else if (localPhase < 2 * GAP) {
			r = map(localPhase % GAP, 0, GAP, 255, 0);
			g = 255;
			b = 0;
		}
		else if (localPhase < 3 * GAP) {
			r = 0;
			g = 255;
			b = map(localPhase % GAP, 0, GAP, 0, 255);
		}
		else if (localPhase < 4 * GAP) {
			r = 0;
			g = map(localPhase % GAP, 0, GAP, 255, 0);
			b = 255;
		}
		else if (localPhase < 5 * GAP) {
			r = map(localPhase % GAP, 0, GAP, 0, 255);
			g = 0;
			b = 255;
		}
		else {
			r = 255;
			g = 0;
			b = map(localPhase % GAP, 0, GAP, 255, 0);
		}

		target.setPixelColor(led, r, g, b);
	}
}