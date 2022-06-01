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
void vPrintStringAndNumber( const char *pcString, unsigned long ulValue )
{
	/* Print the string, suspending the scheduler as method of mutual
	exclusion. */
	vTaskSuspendAll();
	{
		printf( "%s %u\r\n", pcString, ulValue );
	}
	xTaskResumeAll();
}

/* A variable that is incremented by the idle task hook function. */
/*To-do:
	Declare a global variable "ulIdleCycleCount" with the unsigned long type.
	And initialize it with the value 0UL. */

unsigned long ulIdleCycleCount=0UL;

void vApplicationIdleHook()
{
	/* This hook function does nothing but increment a counter. */
	/*To-do:
		Only perform the function of incrementing the variable ulIdleCycleCount by 1. */
	ulIdleCycleCount++;
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

/* The task function. */
static void vTaskFunction( void *pvParameters );


/* Define the strings that will be passed in as the task parameters.  These are
defined const and off the stack to ensure they remain valid when the tasks are
executing. */
const char *pcTextForTask1 = "Task 1 count= ";
const char *pcTextForTask2 = "Task 2 count= ";


/*-----------------------------------------------------------*/

int main( void )
{
	system_init();
	
	configure_console();
	
	printf("Experiment 7 starts\r\n");
	
	/* Create the first task at priority 1... */
	xTaskCreate( vTaskFunction, "Task 1", 240, (void*)pcTextForTask1, 1, NULL );

	/* ... and the second task at priority 2.  The priority is the second to
	last parameter. */
	xTaskCreate( vTaskFunction, "Task 2", 240, (void*)pcTextForTask2, 2, NULL );	
	
	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	running.  If we do reach here then it is likely that there was insufficient
	heap available for the idle task to be created. */
	for( ;; );
}

static void vTaskFunction( void *pvParameters )
{
	char *pcTaskName;

	/* The string to print out is passed in via the parameter.  Cast this to a
	character pointer. */
	pcTaskName = ( char * ) pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/*To-do:
			Call the vPrintStringAndNumber() library function, and pass it with
		  two parameters: the name of this task AND the number of ulIdleCycleCount. */
		vPrintStringAndNumber(pcTaskName,ulIdleCycleCount);
		

		/* Delay for a period.  This time we use a call to vTaskDelay() which
		puts the task into the Blocked state until the delay period has expired.
		The delay period is specified in 'ticks'. */
		vTaskDelay( 250 / portTICK_RATE_MS );
	}
}
