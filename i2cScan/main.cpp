/* Kernel includes. */
#include <FreeRTOS.h>
#include <task.h>

/* Library includes. */
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <pico/binary_info.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include "lcd1602.h"

// I²C Pins
// I²C 0 - SDA: GP0/GP4/ GP8/GP12/GP16/GP20
// I²C 0 - SCL: GP1/GP5/ GP9/GP13/GP17/GP21
// I²C 1 - SDA: GP2/GP6/GP10/GP14/GP18/GP26
// I²C 1 - SCL: GP3/GP7/GP11/GP15/GP19/GP27
#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3


/**
 * Configures the hardware
 */
static void setup();

/**
 * Configures the I²C
 */
static void setup_i2c();

/**
 * Scans for I²C devices
 */
void i2c_scan();

/**
 * The main task. Waits for key and performs scan.
 * @param  Parameter passed to the task. Not used.
 */
static void mainTask( void *param );


int main( void ){
	setup();

	xTaskCreate( mainTask,                  /* The function that implements the task. */
				 "main",                    /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				 configMINIMAL_STACK_SIZE,  /* The size of the stack to allocate to the task. */
				 NULL,                      /* The parameter passed to the task - not used in this case. */
				 tskIDLE_PRIORITY,          /* The priority assigned to the task. */
				 NULL );                    /* The task handle is not required, so NULL is passed. */

	i2c_scan();
	vTaskStartScheduler();
	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the Idle and/or
	timer tasks to be created.  See the memory management section on the
	FreeRTOS web site for more details on the FreeRTOS heap
	http://www.freertos.org/a00111.html. */
	for( ;; );
	return 0;
}

static void setup(){
	stdio_init_all();
	setup_i2c();
}


static void setup_i2c(){
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
	#warning i2c/lcd_1602_i2c example requires a board with I2C pins
#else
	i2c_init(&i2c1_inst, 400000);
	gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(I2C_SDA_PIN);
	gpio_pull_up(I2C_SCL_PIN);
	bi_decl(bi_2pins_with_func(I2C_SDA_PIN, I2C_SCL_PIN, GPIO_FUNC_I2C));
#endif
}


static void mainTask( void *param ){
	vTaskDelay(pdMS_TO_TICKS(1000));
	while(true){
		printf("Press any key to scan for i2c devices...");
		getchar();
		printf("\n");
		i2c_scan();
		printf("\n\n\n");
	}
}

static inline
bool reserved_addr(uint8_t addr) {
	return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

static
void i2c_scan(){
	printf("I2C Bus Scan\n");
	printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

	for (int addr = 0; addr < 0x80; ++addr) {
		if (addr % 16 == 0)
			printf("%02x ", addr);

		// Perform a 1-byte dummy read from the probe address. If a slave
		// acknowledges this address, the function returns the number of bytes
		// transferred. If the address byte is ignored, the function returns
		// -1.

		// Skip over any reserved addresses.
		int ret;
		uint8_t rxdata;
		if (reserved_addr(addr))
			ret = PICO_ERROR_GENERIC;
		else
			ret = i2c_read_blocking(&i2c1_inst, addr, &rxdata, 1, false);

		printf(ret < 0 ? "." : "@");
		printf(addr % 16 == 15 ? "\n" : "  ");
	}
	printf("Done.\n");
}
