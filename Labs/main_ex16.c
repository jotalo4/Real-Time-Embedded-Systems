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

/* The gatekeeper task itself. */
static void prvStdioGatekeeperTask( void *pvParameters );

/* Define the strings that the tasks and interrupt will print out via the gatekeeper. */
static char *pcStringsToPrint[] =
{
	"Task 1 **********************************\r\n",
	"Task 2 ----------------------------------\r\n",
	"Message printed from tick hook interrupt #\r\n"
};

/*-----------------------------------------------------------*/

/* To-do: Declare a variable xPrintQueue of the type xQueueHandle.
This is used to send messages from the print tasks to the gatekeeper task. */


xQueueHandle xPrintQueue;





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
	
	printf("Experiment 16 starts\r\n");

	//------------------------------------------------------
    /* To-do: Call the xQueueCreate() API function to create a queue.
	The first parameter is set to 5 which specifies the maximum number of items.
	The second parameter is the size of char*.
	The return value is assigned to the newly created variable xPrintQueue.*/
   xPrintQueue = xQueueCreate(5,sizeof(char*));
	/*Before a queue is used it must be explicitly created.  The queue is created
	to hold a maximum of 5 character pointers. */

	
	
	/* The tasks are going to use a pseudo random delay, seed the random number
	generator. */
	srand( 567 );


	/* Check the queue was created successfully. */
	if( xPrintQueue != NULL )
	{
		/* Create two instances of the tasks that send messages to the gatekeeper.
		The	index to the string they attempt to write is passed in as the task
		parameter (4th parameter to xTaskCreate()).  The tasks are created at
		different priorities so some pre-emption will occur. */
		xTaskCreate( prvPrintTask, "Print1", 240, ( void * ) 0, 1, NULL );
		xTaskCreate( prvPrintTask, "Print2", 240, ( void * ) 1, 2, NULL );

		/* Create the gatekeeper task.  This is the only task that is permitted
		to access standard out. */
		xTaskCreate( prvStdioGatekeeperTask, "Gatekeeper", 240, NULL, 0, NULL );

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}

	//------------------------------------------------------

	

	/* If all is well we will never reach here as the scheduler will now be
	running.  If we do reach here then it is likely that there was insufficient
	heap available for the idle task to be created. */
	for( ;; );
}

/*-----------------------------------------------------------*/

static void prvStdioGatekeeperTask( void *pvParameters )
{
	char *pcMessageToPrint;
	static char cBuffer [mainMAX_MSG_LEN];
	
	/* This is the only task that is allowed to write to the terminal output.
	Any other task wanting to write to the output does not access the terminal
	directly, but instead sends the output to this task.  As only one task
	writes to standard out there are no mutual exclusion or serialization issues
	to consider within this task itself. */
	for( ;; )
	{
		/* To-do: Call xQueueReceive() API. 
			The first parameter is the queue variable xPrintQueue you declared at the beginning.
			The second parameter is the reference to the variable pcMessageToPrint.
			The last parameter is portMAX_DELAY. */		
		/* Wait for a message to arrive. */

		xQueueReceive(xPrintQueue, &pcMessageToPrint, portMAX_DELAY);




		/* There is no need to check the return	value as the task will block
		indefinitely and only run again when a message has arrived.  When the
		next line is executed there will be a message to be output. */
		sprintf(cBuffer, "%s", pcMessageToPrint);
		printf( cBuffer );

		/* Now simply go back to wait for the next message. */
	}
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	static int iCount = 0;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Print out a message every 200 ticks.  The message is not written out
	directly, but sent to the gatekeeper task. */
	iCount++;
	if( iCount >= 200 )
	{
		/* To-do: Call xQueueSendToFrontFromISR() API. 
			The first parameter is the queue variable xPrintQueue you declared at the beginning.
			The second parameter is the reference to the last element of 
						the array variable pcStringsToPrint.
			The last parameter is the reference to the variable xHigherPriorityTaskWoken. */		
		
		xQueueSendToFrontFromISR(xPrintQueue, &pcStringsToPrint, &xHigherPriorityTaskWoken);
		/* In this case the last parameter (xHigherPriorityTaskWoken) is not
		actually used but must still be supplied. */
		





		/* Reset the count ready to print out the string again in 200 ticks time. */
		iCount = 0;
	}
}

static void prvPrintTask( void *pvParameters )
{
	int iIndexToString;

	/* Two instances of this task are created so the string the task will send
	to prvNewPrintString() is passed in the task parameter.  Cast this to the
	required type. */
	iIndexToString = ( int ) pvParameters;

	for( ;; )
	{
		/* Print out the string, not directly but by passing the string to the
		gatekeeper task on the queue.  The queue is created before the scheduler is
		started so will already exist by the time this task executes.  A block time
		is not specified as there should always be space in the queue. */
		xQueueSendToBack( xPrintQueue, &( pcStringsToPrint[ iIndexToString ] ), 0 );



		/* Wait a pseudo random time.  Note that rand() is not necessarily
		re-entrant, but in this case it does not really matter as the code does
		not care what value is returned.  In a more secure application a version
		of rand() that is known to be re-entrant should be used - or calls to
		rand() should be protected using a critical section. */
		vTaskDelay( ( rand() & 0x1FF ) );
	}
}
