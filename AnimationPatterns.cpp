#include "AnimationPatterns.h"

#include <Adafruit_NeoPixel.h>

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