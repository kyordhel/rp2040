#include "neopixel.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <math.h>

static inline
bool hsv2rgb(uint16_t h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b);


NeoPixel::NeoPixel(uint8_t pin, uint8_t pixel_count) {
	WS2812_sm = 0; // State machine being used (zero)
	// Load program and get offset
	uint offset = pio_add_program(pio0, &ws2812_program);
	ws2812_program_init(
		pio0,        // PIO used
		WS2812_sm,   // State machine being used (zero)
		offset,      // Program offset
		pin,         // Pin used to control the neopixel lane
		800000,      // Frequency: run at 800kHz
		false        // RGBW: true. RGB false.
	);
	buffer = new uint32_t[pcount = pixel_count];
}

NeoPixel::~NeoPixel(){
	delete buffer;
}

void NeoPixel::write() {
	for(size_t i = 0; i < pcount; ++i)
		pio_sm_put_blocking(pio0, WS2812_sm, buffer[i]);
	vTaskDelay(pdMS_TO_TICKS(1)); // Sleep for at least 1ms between writes
}

bool NeoPixel::setColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w){
	if (index >= pcount) return false;
	buffer[index] = ((uint32_t)g << 24) | ((uint32_t)r << 16) | ((uint32_t)b << 8) | (uint32_t)w;
	return true;
}

uint32_t NeoPixel::getColor(uint8_t index){
	return (index < pcount) ?  buffer[index] : 0;
}

bool NeoPixel::setHSV(uint8_t index, uint16_t h, float s, float v){
	uint8_t r, g, b;
	return hsv2rgb(h, s, v, &r, &g, &b) && setColor(index, r, g, b);
}

bool hsv2rgb(uint16_t h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b){
	float R, G, B;

	if(
		(s < 0) || (v < 0) ||
		(h > 360) || (s > 1) || (v > 1)
	) return false;

	float C = s * v;
	float X = C * ( 1 - abs(fmod(h/60.0, 2) -1) );
	float m = v - C;

	if( h < 60 ){      // Case 1: h in [0, 60)
		R = C;     G = X;     B = 0;
	}
	else if (h < 120){ // Case 2: h in [60, 120)
		R = X;     G = C;     B = 0;
	}
	else if (h < 180){ // Case 3: h in [120, 180)
		R = 0;     G = C;     B = X;
	}
	else if (h < 240){ // Case 4: h in [180, 240)
		R = 0;     G = X;     B = C;
	}
	else if (h < 300){ // Case 4: h in [240, 300)
		R = X;     G = 0;     B = C;
	}
	else{              // Case 4: h in [300, 360)
		R = C;     G = 0;     B = X;
	}

	*r = R * 255;
	*g = G * 255;
	*b = B * 255;
	return true;
}