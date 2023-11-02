/* Kernel includes. */
#include <FreeRTOS.h>
#include <task.h>

/* Library includes. */
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>


/**
 * Core0 main function
 */
void main( void );

/**
 * Core1 main function
 */
void core1_main( void );

/**
 * The Hello-World task to be run on Core0
 */
void helloTask( void* param );


void main( void ){
	stdio_init_all();
	// for(uint8_t i = 0; i < 3; ++i) sleep_ms( 1000 );

	printf("\fStarting multicore hello-world...\n");
	xTaskCreate( helloTask,                 /* The function that implements the task. */
				 NULL,                      /* The text name assigned to the task - for debug only as it is not used by the kernel. */
				 configMINIMAL_STACK_SIZE,  /* The size of the stack to allocate to the task. */
				 NULL,                      /* The parameter passed to the task - not used in this case. */
				 1,                         /* The priority assigned to the task. */
				 NULL );                    /* The task handle is not required, so NULL is passed. */

	multicore_launch_core1(core1_main);
	vTaskStartScheduler();
	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the Idle and/or
	timer tasks to be created.  See the memory management section on the
	FreeRTOS web site for more details on the FreeRTOS heap
	http://www.freertos.org/a00111.html. */
	for( ;; );
}

void helloTask( void* param ){
	while(true){
		printf("Core %d: Hello World!\n", get_core_num());
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void core1_main( void ){
	while(true){
		printf("Core %d: Hello World!\n", get_core_num());
		sleep_ms( 1000 );
	}
}

