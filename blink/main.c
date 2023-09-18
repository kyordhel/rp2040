/* Kernel includes. */
#include <FreeRTOS.h>
#include <task.h>

/* Library includes. */
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>


/**
 * Configures the hardware
 */
static void setup();

/**
 * The task that will blink the led.
 * @param  Parameter passed to the task. Not used.
 */
static void blinkTask( void *param );


void main( void ){
	setup();
	printf("Starting blink.\n");

	xTaskCreate( blinkTask,                 /* The function that implements the task. */
				 "blink",                   /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				 configMINIMAL_STACK_SIZE,  /* The size of the stack to allocate to the task. */
				 NULL,                      /* The parameter passed to the task - not used in this case. */
				 1,                         /* The priority assigned to the task. */
				 NULL );                    /* The task handle is not required, so NULL is passed. */

	vTaskStartScheduler();
	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the Idle and/or
	timer tasks to be created.  See the memory management section on the
	FreeRTOS web site for more details on the FreeRTOS heap
	http://www.freertos.org/a00111.html. */
	for( ;; );
}

static void setup(){
	stdio_init_all();
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN, 0);
}


static void blinkTask( void *param ){
	int value = 0;

	while(true){
		gpio_put(PICO_DEFAULT_LED_PIN, value);
		printf("Led: %d\n", value);
		value = !value;
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

}
