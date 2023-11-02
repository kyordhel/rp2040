/* Kernel includes. */
#include <FreeRTOS.h>
#include <task.h>

/* Library includes. */
#include <stdio.h>
#include <pico/stdlib.h>


/**
 * The task that will blink the led.
 * @param  Parameter passed to the task. Not used.
 */
static void helloTask( void *param );


void main( void ){
	stdio_init_all();
	printf("Starting hello-world...\n");
	for(size_t i = 0; i < 4; ++i){

		xTaskCreate( helloTask,                 /* The function that implements the task. */
					 NULL,                      /* The text name assigned to the task - for debug only as it is not used by the kernel. */
					 configMINIMAL_STACK_SIZE,  /* The size of the stack to allocate to the task. */
					 (void*) i,                 /* The parameter passed to the task - the task number in this case. */
					 1,                         /* The priority assigned to the task. */
					 NULL );                    /* The task handle is not required, so NULL is passed. */
	}

	vTaskStartScheduler();
	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the Idle and/or
	timer tasks to be created.  See the memory management section on the
	FreeRTOS web site for more details on the FreeRTOS heap
	http://www.freertos.org/a00111.html. */
	for( ;; );
}

static void helloTask( void *param ){
	size_t tnum = (size_t)param;
	while(true){
		printf("Hello from task %lu\n", tnum);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
