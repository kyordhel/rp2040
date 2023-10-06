/* Kernel includes. */
#include <FreeRTOS.h>
#include <task.h>

/* Library includes. */
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/mutex.h>
#include <hardware/gpio.h>

#include"neopixel.h"

/**
 * The delay variable that is shared by the two threads
*/
volatile uint32_t svDelay = 500;

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
	uint8_t pins[] = {PICO_DEFAULT_LED_PIN, 16, 17};
	for (uint8_t i = 0; i < 3; ++i){
		gpio_init(pins[i]);
		gpio_set_dir(pins[i], GPIO_OUT);
		gpio_put(pins[i], 0);
	}
}


static void blinkTask( void *param ){
	int value = 0;
	uint32_t blinkDelay;
	gpio_put(16, 1);
	vTaskDelay(pdMS_TO_TICKS(1000));
	printf("Starting blink.\n");

	vTaskDelay(pdMS_TO_TICKS(1));

	NeoPixel np = NeoPixel(17, 1);

	while(true){
		gpio_put(PICO_DEFAULT_LED_PIN, value);
		value = !value;
		mutex_enter_blocking(&mutex);
		blinkDelay = svDelay;
		mutex_exit(&mutex);
		vTaskDelay(pdMS_TO_TICKS(blinkDelay));

		np.setColor(0, 64, 0, 0);
		np.write();
	}
}

static void usbTask( void *param ){
	uint32_t delay;
	vTaskDelay(pdMS_TO_TICKS(1100));
	while(true){
		printf("Delay [ms]: ");
		if(!readInt(&delay)){
			printf("\nInvalid input\n");
			continue;
		}
		mutex_enter_blocking(&mutex);
		svDelay = delay;
		mutex_exit(&mutex);
		printf("Delay set to %d\n", delay);
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
