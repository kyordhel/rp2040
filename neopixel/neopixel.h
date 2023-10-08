#ifndef __NEOPIXEL_h__
#define __NEOPIXEL_h__

#include "ws2812.pio.h"

class NeoPixel{
public:
	NeoPixel(uint8_t pin, uint8_t pixel_count);
	~NeoPixel();
	NeoPixel(const NeoPixel&) = delete; // Disable copy constructor
	void operator=(const NeoPixel&) = delete; // Disable assignment operator

	void write();
	bool setColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w=0);
	uint32_t getColor(uint8_t index);
	bool setHSV(uint8_t index, uint16_t h, float s, float v);

private:
	uint8_t WS2812_sm;
	uint8_t pcount;
	uint32_t *buffer;
	static NeoPixel* instance;
};


#endif // __NEOPIXEL_h__