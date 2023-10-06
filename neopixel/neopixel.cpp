#include "neopixel.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>


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
		true
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
	printf("Wrote:");
	for(size_t i = 0; i < pcount; ++i)
		printf(" 0x%08x", buffer[i]);
	printf("\n");
}


bool NeoPixel::setColor(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w){
	if (index >= pcount) return false;
	buffer[index] = ((uint32_t)g << 24) | ((uint32_t)r << 16) | ((uint32_t)b << 8) | (uint32_t)w;
	printf("Buffer: 0x%08x\n", buffer[index]);
	return true;
}
