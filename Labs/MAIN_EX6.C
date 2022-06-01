/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>

void vApplicationIdleHook()
{
	while(1);	
}

/** UART module for debug. */
static struct usart_module cdc_uart_module;

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	struct usart_config usart_conf;

	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	usart_conf.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	usart_conf.baudrate    = 115200;

	stdio_serial_init(&cdc_uart_module, EDBG_CDC_MODULE, &usart_conf);
	usart_enable(&cdc_uart_module);
}

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0x11ffff )

/* To-do:
	Declare the prototype of the task function vContinuousProcessingTask */
static void vContinuousProcessingTask( void *pvParameters );

/* To-do:
	Declare the prototype of the task function vPeriodicTask */
static void vPeriodicTask( void *pvParameters );

/* Define the strings that will be passed in as the task parameters.  These are
defined const and off the stack to ensure they remain valid when the tasks are
executing. */
const char *pcTextForTask1 = "Task 1 \r\n";
const char *pcTextForTask2 = "Task 2 \r\n";
const char *pcTextForPeriodicTask = "Periodic task \r\n";

/*-----------------------------------------------------------*/

int main( void )
{
	system_init();
	
	configure_console();
	
	printf("Experiment 6 starts\r\n");
	
	/* To-do:
		Create the first instance of the continuous task vContinuousProcessingTask
		at priority	1. Pass the string pcTexForTask1 as the 4th argument */
	xTaskCreate(vContinuousProcessingTask, "Continuous Task 1", 200, (void *)pcTextForTask1, 1, NULL);
	
	/* To-do:
		Also create the second instance of the continuous task vContinuousProcessingTask
		at priority	1. Pass the string pcTexForTask2 as the 4th argument */
	xTaskCreate(vContinuousProcessingTask, "Continuous Task 2", 200, (void *)pcTextForTask2, 1, NULL);

	/* To-do:
		Create one instance of the periodic task vPeriodicTask
		at priority	2. Pass the string pcTexForPeriodicTask as the 4th argument */	
	xTaskCreate(vPeriodicTask, "Periodic Task 1", 200, (void *)pcTextForPeriodicTask, 2, NULL);

	
	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	running.  If we do reach here then it is likely that there was insufficient
	heap available for the idle task to be created. */
	for( ;; );
}

static void vContinuousProcessingTask( void *pvParameters )
{
	/* To-do:
		Copy the function implementation of vTaskFunction() in Example 3 here. */
	char *pcTaskName;
	volatile unsigned long ul;

	/* The string to print out is passed in via the parameter.  Cast this to a
	character pointer. */
	pcTaskName = ( char * ) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		printf( pcTaskName );

		/* Delay for a period. */
		for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
		{
			/* This loop is just a very crude delay implementation.  There is
			nothing to do in here.  Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}
	}
}
/*-----------------------------------------------------------*/

static void vPeriodicTask( void *pvParameters )
{
	/* To-do:
		Copy the function implementation of vTaskFunction() in Example 5 here. */
	char *pcTaskName;
	
	/* To-do:
			Declare a variable named as xLastWakeTime with the type portTickType. */
	portTickType xLastWakeTime;
		
	/* The string to print out is passed in via the parameter.  Cast this to a
	character pointer. */
	pcTaskName = ( char * ) pvParameters;

	/* To-do: 
		Assign xLastWakeTime with the  value returned from the library
		function xTaskGetTickCount(). */
	xLastWakeTime = xTaskGetTickCount();
	

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		printf( pcTaskName );

		/* To-do:
			Call the library function vTaskDelayUntil() to generate periodic delay.
			It requires two parameters, 1th parameter is the pointer to the xLastWakeTime;
			2nd parameter is the relative delay time which could be (250 / portTICK_RATE_MS). 
			*/
		vTaskDelayUntil(&xLastWakeTime, (250 / portTICK_RATE_MS));
		
	}
	
}