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
#include "semphr.h"
#include "task.h"

/* Dimensions the buffer into which messages destined for stdout are placed. */
#define mainMAX_MSG_LEN	( 80 )

/* The task to be created.  Two instances of this task are created. */
static void prvPrintTask( void *pvParameters );

/* The function that uses a mutex to control access to standard out. */
static void prvNewPrintString( const portCHAR *pcString );


/* To-do: Declare a variable named as xMutex with the type xSemaphoreHandle.
Declare a variable of type xSemaphoreHandle.  This is used to reference the
mutex type semaphore that is used to ensure mutual exclusive access to stdout. */

 xSemaphoreHandle xMutex;



void vApplicationIdleHook(void)
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


/*-----------------------------------------------------------*/

int main( void )
{
	system_init();
	
	configure_console();
	
	printf("Experiment 15 starts\r\n");

	//------------------------------------------------------
    /* To-do: Call the xSemaphoreCreateMutex() API function to create a mutex.
		The return value is assigned to the newly created variable xMutex.
	Before a semaphore is used it must be explicitly created.  In this example
	a mutex type semaphore is created. */

	xMutex=xSemaphoreCreateMutex();

	/* The tasks are going to use a pseudo random delay, seed the random number
	generator. */
	srand( 567 );

	/* To-do: Check if xMutex is created successfully by comparing it with NULL. 
		If it is not NULL, we created two tasks and start the scheduler. 
		
	Only create the tasks if the semaphore was created successfully. */
	if(xMutex != NULL)
	{
		/* Create two instances of the tasks that attempt to write stdout.  The
		string they attempt to write is passed in as the task parameter.  The tasks
		are created at different priorities so some pre-emption will occur. */
		xTaskCreate( prvPrintTask, "Print1", 240, "Task 1 ******************************************\r\n", 1, NULL );
		xTaskCreate( prvPrintTask, "Print2", 240, "Task 2 ------------------------------------------\r\n", 2, NULL );

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}		
	//------------------------------------------------------
	

	/* If all is well we will never reach here as the scheduler will now be
	running.  If we do reach here then it is likely that there was insufficient
	heap available for the idle task to be created. */
	for( ;; );
}

//------------------------------------------------------------
static void prvNewPrintString( const portCHAR *pcString )
{
	static char cBuffer[ mainMAX_MSG_LEN ];

	/* To-do: Call the xSemaphoreTake() API function to take the mutex.
		Two parameters are needed. The first one is the mutex variable you created 
		previously; The second is the block time specified as portMAX_DELAY.
	
	The semaphore is created before the scheduler is started so already
	exists by the time this task executes.

	Attempt to take the semaphore, blocking indefinitely if the mutex is not
	available immediately.  The call to xSemaphoreTake() will only return when
	the semaphore has been successfully obtained so there is no need to check the
	return value.  If any other delay period was used then the code must check
	that xSemaphoreTake() returns pdTRUE before accessing the resource (in this
	case standard out. */
	xSemaphoreTake(xMutex,portMAX_DELAY);
	

	{
		/* The following line will only execute once the semaphore has been
		successfully obtained - so standard out can be accessed freely. */
		sprintf( cBuffer, "%s", pcString );
		printf( cBuffer );
	}
		
	/* To-do: Call the xSemaphoreGive() API function to take the mutex.
		One parameter is needed. It is the mutex variable you created previously.			
	*/
	xSemaphoreGive(xMutex);
	
	
}

static void prvPrintTask( void *pvParameters )
{
	char *pcStringToPrint;

	/* Two instances of this task are created so the string the task will send
	to prvNewPrintString() is passed in the task parameter.  Cast this to the
	required type. */
	pcStringToPrint = ( char * ) pvParameters;

	for( ;; )
	{

	/* To-do: Call the prvNewPrintString() function by passing the variable 
		pcStringToPrint as the parameter.
	Print out the string using the newly defined function. */
	prvNewPrintString(pcStringToPrint);



		/* Wait a pseudo random time.  Note that rand() is not necessarily
		re-entrant, but in this case it does not really matter as the code does
		not care what value is returned.  In a more secure application a version
		of rand() that is known to be re-entrant should be used - or calls to
		rand() should be protected using a critical section. */
		vTaskDelay( ( rand() & 0x1FF ) );
	}
}
