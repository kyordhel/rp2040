#ifndef __NEOPIXEL_h__
#define __NEOPIXEL_h__

#include "ws2812.pio.h"

class NeoPixel{
public:
	/**
	 * Initializes a new instance of the NeoPixel driver
	 * @param  pin         Pin with the NeoPixel stripe
	 * @param  pixel_count Number of neopixels in series
	 */
	NeoPixel(uint8_t pin, uint8_t pixel_count);
	/**
	 * Destructor
	 */
	~NeoPixel();

	NeoPixel(const NeoPixel&) = delete; // Disable copy constructor
	void operator=(const NeoPixel&) = delete; // Disable assignment operator

	/**
	 * [write description]
	 */
	void write();

	/**
	 * Sets the color of a particular pixel in the strip
	 * @param  index Index of the pixel in the strip whose color will be changed
	 * @param  r     Color's Red component
	 * @param  g     Color's Green component
	 * @param  b     Color's Blue component
	 * @param  w     Color's White component (Optional, for RGBW leds only)
	 * @return       True if the color data was latched successfully, false otherwise (index out of range).
	 */
	bool setColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w=0);

	/**
	 * Gets the color of a particular pixel in the strip
	 * @param  index Index of the pixel in the strip whose color will be changed
	 * @return       True if the color data has been latched successfully, false otherwise (index out of range).
	 */
	uint32_t getColor(uint8_t index);

	/**
	 * Sets the color of a particular pixel in the strip in HSV
	 * @param  index Index of the pixel in the strip whose color will be changed
	 * @param  h     Color's hue
	 * @param  s     Color's saturation
	 * @param  v     Color's brightness
	 * @return       True if the color data was latched successfully, false otherwise (index out of range).
	 */
	bool setHSV(uint8_t index, uint16_t h, float s, float v);

private:
	uint8_t WS2812_sm;
	uint8_t pcount;
	uint32_t *buffer;
	static NeoPixel* instance;
};


#endif // __NEOPIXEL_h__