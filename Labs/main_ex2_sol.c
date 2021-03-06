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
#define mainDELAY_LOOP_COUNT		( 0x11ffff )//( 0xffffff )

/* To-do: 
	DECLARE the prototype of a task function "vTaskFunction". This function is
	defined after the "main" function.
	*/
static void vTaskFunction( void *pvParameters );

/* To-do: 
	DECLARE a global constant string variable "pcTextForTask1" that will be passed in 
	as the task parameter, and initialize it as "Task 1 \r\n". */
const char *pcTextForTask1 = "Task 1 \r\n";

/* To-do: 
	DECLARE another global constant string variable "pcTextForTask2" that will also be passed in 
	as the task parameter, and initialize it as "Task 2 \r\n". */
const char *pcTextForTask2 = "Task 2 \r\n";

/*-----------------------------------------------------------*/

int main( void )
{
	system_init();
		
	configure_console();
	
	printf("Experiment 2 starts\r\n");
	
	/* To-do:
		Create the first instance of the task "vTaskFunction" by modifying the 1th parameter to "vTaskFunction" and passing 
		"pcTextForTask1" to the task instance in the 4th parameter, other parameters are same as lab1 */
		xTaskCreate( vTaskFunction, "Task 1", 200, (void *)pcTextForTask1, 1, NULL );

	
	/* To-do:
		Create the second task instance in the same way. but passing 
		a different value as the 4th parameter "pcTextForTask2".  */
		xTaskCreate( vTaskFunction, "Task 2", 200, (void *)pcTextForTask2, 1, NULL );
	
	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	running.  If we do reach here then it is likely that there was insufficient
	heap available for the idle task to be created. */
	for( ;; );
}

/*-----------------------------------------------------------*/
static void vTaskFunction( void *pvParameters )
{
	/* To-do:
		Declare a string variable "pcTaskName" WITHOUT initialization. */
	char *pcTaskName;
	
	volatile unsigned long ul;
	
	// The string to print out is passed in via the parameter.  
	/* To-do:
		Cast the input parameter "pvParameters" to character pointer (i.e., char *) 
		and assign it to the variable "pcTaskName". */
	pcTaskName = (char *)pvParameters;
		
	/* As per most tasks, this task is implemented in an infinite loop
		which is exactly SAME as the loop body of vTask1 in Example 1.*/
	for( ;; )
	{
		/* To-do:
			Call printf() function to display the name of this task at the terminal. */
		printf(pcTaskName);		

		/* Delay for a period. */
		for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
		{
			/* This loop is just a very crude delay implementation.  There is
			nothing to do in here.  Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}
	}
}