/* Kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/* Library includes. */
#include <stdio.h>
#include <pico/stdlib.h>

#define PARTASKS 4

struct{
	size_t   job;
	size_t   start;
	size_t   end;
	uint32_t n;
	double   result;
}typedef work_t;


// Synchronization
SemaphoreHandle_t semb;
SemaphoreHandle_t semc;

/**
 * Main task: starts all computations and measure time.
 * @param  Parameter passed to the task. Not used.
 */
static void mainTask( void *param );

/**
 * Sequential Pi task: Approximates pi itself (wrapper function)
 * @param  The number of terms used to calculate pi (cast)
 */
static void seqPiTask(void *param);

/**
 * Parallel Pi task Approximates pi using N tasks (wrapper function)
 * @param  Parameter passed to the task. Not used.
 */
static void parPiTask(void *param);

/**
 * Approximates pi using n terms
 * @param  n Number of terms
 * @return   Calculated value of pi
 */
static double sequential_pi (uint32_t n);

/**
 * Approximates pi using n terms
 * @param  job Pointer to work_t structure
 */
static void parallel_pi (void* job);

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

void main( void ){
	stdio_init_all();
	printf("Starting hello-world...\n");
	for(size_t i = 0; i < 4; ++i){

		xTaskCreate( mainTask,                  /* The function that implements the task. */
					 NULL,                      /* The text name assigned to the task - for debug only as it is not used by the kernel. */
					 configMINIMAL_STACK_SIZE,  /* The size of the stack to allocate to the task. */
					 NULL,                      /* The parameter passed to the task - not used in this case. */
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



static void mainTask( void *param ){
	uint32_t n;
	TaskHandle_t seqPiTaskHandle;
	TaskHandle_t parPiTaskHandle;

	semb = xSemaphoreCreateBinary();
	while(true){
		printf("\fInteger between 1000 and 50k: ");
		if( !readInt(&n) ) continue;
		if(n < 1000) n = 1000;
		if(n > 65535) n = 65535;

		// Sequential
		printf("Calculating pi using 1 task and %lu terms...\n", n);
		xTaskCreate(seqPiTask, NULL, configMINIMAL_STACK_SIZE,
			(void*)n, 1, &seqPiTaskHandle);
		xSemaphoreTake(semb, portMAX_DELAY);

		//Cleanup
		vTaskDelete(seqPiTaskHandle);
		printf("\n");

		// Concurrent
		printf("Calculating pi using %d tasks and %lu terms...\n", PARTASKS, n);
		xTaskCreate(parPiTask, NULL, configMINIMAL_STACK_SIZE,
			(void*)n, 1, &parPiTaskHandle);
		xSemaphoreTake(semb, portMAX_DELAY);

		//Cleanup
		vTaskDelete(parPiTaskHandle);
	}
}



static void parPiTask(void *param){
	double pi;
	uint32_t n = (uint32_t)param;
	TickType_t start, end, diff;
	work_t jobs[PARTASKS];
	TaskHandle_t handle[PARTASKS];

	start = xTaskGetTickCount();

	semc = xSemaphoreCreateCounting(PARTASKS, 0);
	// Launch all tasks
	for(uint32_t i = 0; i < PARTASKS; ++i){
		jobs[i].job   = i + 1;
		jobs[i].start = i * n / PARTASKS;
		jobs[i].end   = (i+1) * n / PARTASKS;
		jobs[i].n     = n;
		xTaskCreate(parallel_pi, NULL, configMINIMAL_STACK_SIZE,
			&jobs[i], 1, &handle[i]);
	}
	// Wait for them to finish
	for(uint32_t i = 0; i < PARTASKS; ++i)
		xSemaphoreTake(semc, portMAX_DELAY);
	for(uint32_t i = 0; i < PARTASKS; ++i){
		vTaskDelete(handle[i]);
		pi+= jobs[i].result;
	}
	end = xTaskGetTickCount();

	diff = end - start;
	printf("Pi:      %0.6f\n", pi);
	printf("Tasks:   %d\n", PARTASKS);
	printf("Elapsed: %lu ticks (%0.0fms)\n", diff, diff * (1000.0/configTICK_RATE_HZ));

	vSemaphoreDelete(semc);
	xSemaphoreGive(semb);
	for(;;);
}



static void seqPiTask(void *param){
	double pi;
	TickType_t start, end, diff;

	start = xTaskGetTickCount();
	pi = sequential_pi( (uint32_t)param );
	end = xTaskGetTickCount();
	diff = end - start;
	printf("Pi:      %0.6f\n", pi);
	printf("Tasks:   1\n");
	printf("Elapsed: %lu ticks (%0.0fms)\n", diff, diff * (1000.0/configTICK_RATE_HZ));

	xSemaphoreGive(semb);
	for(;;);
}


static void parallel_pi(void *param){
	work_t* job = (work_t*)param;

	printf("Running job %lu on core %lu\n", job->job, get_core_num());

	double x, dx = 1.0/job->n, sum = 0;

	for (size_t i = job->start; i < job->end; ++i){
		x = dx * i;
		sum+= 4.0/(1.0 + x*x);
	}
	job->result = dx * sum;
	xSemaphoreGive(semc);
	for(;;);
}



static double sequential_pi (uint32_t n) {
	double x, sum = 0.0, dx = 1.0/n;

	for (size_t i = 0; i < n; ++i){
		x = dx * i;
		sum+= 4.0/(1.0 + x*x);
	}
	return dx * sum;
}



static bool readInt(uint32_t* i){
	char ix = 0;
	char buffer[10];
	while(ix < 10){
		buffer[ix] = getchar();
		buffer[ix+1] = 0;
		putchar(buffer[ix]);
		if( buffer[ix] == 0x08 ) // backspace
			buffer[ix--] = buffer[ix--] = 0;
		else if((buffer[ix] == '\n') || (buffer[ix] == '\r')){
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
