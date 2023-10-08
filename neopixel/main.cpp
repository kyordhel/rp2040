/* Kernel includes. */
#include <FreeRTOS.h>
#include <task.h>

/* Library includes. */
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <hardware/gpio.h>

#include"neopixel.h"

// Neopixel pins in DualMCU
#define NEOPIX_PWR 17  // NeoPixel power (on/off)
#define NEOPIX_DIN 16  // NeoPixel data

/**
 * The delay variable that is shared by the two threads
*/
volatile uint16_t svHue = 0;

/**
 * The mutex used to syncronize the acces to the shared vars
 */
static mutex_t mutex;

/**
 * Configures the hardware
 */
static void setup();

/**
 * The task that will blink the led.
 * @param  Parameter passed to the task. Not used.
 */
static void blinkTask( void *param );

/**
 * The task that will read the USB
 * @param  Parameter passed to the task. Not used.
 */
static void usbTask( void *param );

/**
 * Reads an uint32_t from stdin echoing to stdout
 * @param  i When this function returns contains the read input
 * @return   True if an unsigned integer was read, false otherwise
 */
static bool readInt(uint32_t* i);

/**
 * Parses the C-string str interpreting its content as an integral number
 * @param  buffer C-string beginning with the representation of an integral number.
 * @param  endptr Reference to an object of type char*, whose value is set by the function to the next character in str after the numerical value. This parameter can also be a null pointer, in which case it is not used.
 * @return        On success, the function returns the converted integral number as a uint32_t value.
 */
static uint32_t atou(char* buffer, char** endptr);

int main( void ){
	setup();

	xTaskCreate( blinkTask,                 /* The function that implements the task. */
				 "blink",                   /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				 configMINIMAL_STACK_SIZE,  /* The size of the stack to allocate to the task. */
				 NULL,                      /* The parameter passed to the task - not used in this case. */
				 tskIDLE_PRIORITY,          /* The priority assigned to the task. */
				 NULL );                    /* The task handle is not required, so NULL is passed. */

	xTaskCreate( usbTask,                   /* The function that implements the task. */
				 "usb",                     /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				 configMINIMAL_STACK_SIZE,  /* The size of the stack to allocate to the task. */
				 NULL,                      /* The parameter passed to the task - not used in this case. */
				 tskIDLE_PRIORITY,          /* The priority assigned to the task. */
				 NULL );                    /* The task handle is not required, so NULL is passed. */

	mutex_init(&mutex);

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
	uint8_t pins[] = {PICO_DEFAULT_LED_PIN, NEOPIX_PWR, NEOPIX_DIN};
	for (uint8_t i = 0; i < 3; ++i){
		gpio_init(pins[i]);
		gpio_set_dir(pins[i], GPIO_OUT);
		gpio_put(pins[i], 0);
	}
}


static void blinkTask( void *param ){
	uint16_t hue;
	float brightness = 0;
	float delta = 0.01;
	uint32_t delay = 10;

	gpio_put(NEOPIX_PWR, 1);
	vTaskDelay(pdMS_TO_TICKS(500));
	printf("Starting...\n");

	NeoPixel np = NeoPixel(NEOPIX_DIN, 1);

	while(true){
		mutex_enter_blocking(&mutex);
		hue = svHue;
		mutex_exit(&mutex);

		np.setHSV(0, hue, 1, brightness);
		np.write();

		brightness+= delta;
		if((brightness <= 0) || (brightness >= 1))
			delta*= -1;

		vTaskDelay(pdMS_TO_TICKS(delay));
	}
}

static void usbTask( void *param ){
	uint32_t hue;
	vTaskDelay(pdMS_TO_TICKS(550));
	while(true){
		printf("NeoPixel hue [0-360): ");
		if(!readInt(&hue)){
			printf("\nInvalid input\n");
			continue;
		}
		mutex_enter_blocking(&mutex);
		svHue = hue;
		mutex_exit(&mutex);
		printf("NeoPixel hue set to %d\n", hue);
	}
}

static bool readInt(uint32_t* i){
	char ix = 0;
	char buffer[6];
	while(ix < 5){
		buffer[ix] = getchar();
		buffer[ix+1] = 0;
		putchar(buffer[ix]);
		if((buffer[ix] == '\n') || (buffer[ix] == '\r')){
			if(ix < 1) return false;
			buffer[ix] = 0;
			*i = atou(buffer, NULL);
			return true;
		}
		else if( (buffer[ix] < '0') || (buffer[ix] > '9') ){
			return false;
		}
		++ix;
	}
	return false;
}

static uint32_t atou(char* buffer, char** endptr){
	uint32_t num = 0;
	uint32_t pow = 1;
	char *end = buffer;
	while( (*end >= '0') && (*end <= '9') )
		++end;
	if(endptr != NULL) *endptr = end-1;

	for(--end; end >= buffer; --end){
		num+= (*end - '0') * pow;
		pow*= 10;
	}
	return num;
}
