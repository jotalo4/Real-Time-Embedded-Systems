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
#include "queue.h"

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


/* Two instances are created from the sender task in main(). */
/* To-do:
	Declare the prototype of the task function "vSenderTask".*/
static void vSenderTask( void *pvParameters );



/* A single instance is created from the receiver task in main(). */
/* To-do:
	Declare the prototype of the task function "vReceiverTask".*/
static void vReceiverTask( void *pvParameters );



/*-----------------------------------------------------------*/

/* This variable xQueue is used to store the queue data structure that is 
	accessed by all three tasks. */
/* To-do: 
	Declare a Queue named as "xQueue" with the type of xQueueHandle. */
xQueueHandle xQueue;

/* To-do:
	Declare two global constant variables data1 and data2 with long type;
	and initialize data1 with the constant 100, and data2 with the constant 200 */
const long data1=100;
const long data2=200; 

/*-----------------------------------------------------------*/

int main( void )
{
	system_init();
		
	configure_console();
		
	printf("Experiment 10 starts\r\n");

    /* To-do: 
		Call the xQueueCreate() API to create a queue that holds a maximum of 5 long 
		type values. And assign the created queue to to xQueue. */
   xQueue = xQueueCreate(5, sizeof(long));

	if( xQueue != NULL )
	{
		/* To-do: 
			Create one task instance of vSenderTask at priority 1. The 4th parameter is used 
			to pass the address of variable "data1". This task should continuously write
			this variable to the queue. */ 

	xTaskCreate(vSenderTask,"Sender1", 200, (void*)data1,1,NULL );
	
		/* To-do:	
			Create another task instance of vSenderTask at priority 1. The 4th parameter is used 
			to pass the address of variable "data2". This task should continuously write this
			variable to the queue. */
    xTaskCreate(vSenderTask,"Sender2", 200, (void*)data2,1,NULL );

		/* To-do: 
			Create one task instance of vReceiverTask at priority 2. The 4th parameter is NULL.
			This task will read data from the queue.  */
      xTaskCreate( vReceiverTask, "Receiver",200,NULL,2,NULL);

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}
	else
	{
		/* The queue could not be created. */
	}
	
	for( ;; );
}
/*-----------------------------------------------------------*/

static void vSenderTask( void *pvParameters )
{
	/*To-do:
		Declare a long type pointer variable named as lValueToSend. */	
	long lValueToSend; 
	
	/*To-do:
		Declare a variable named as xStatus with the type of portBASE_TYPE. It holds
		the return value of the function xQueueSendToBack() */
	portBASE_TYPE xStatus;

	/* Two instances are created of this task so the value that is sent to the
	queue is passed in via the task parameter rather than be hard coded.  This way
	each instance can use a different value.  Cast the parameter to the required
	type. */
	/*To-do:
		Assign the value of input parameter "pvParamters" to the local variable
		"lValueToSend", and cast the parameter to the (long *) type.*/
	lValueToSend=(long *) pvParameters;

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/*To-do:
			Call the xQueueSendToBack() API function to send data to the queue by passing 
			three parameters as described above; and assign the returned value to xStatus.

		The first parameter is the queue to which data is being sent.  The 
		queue "xQueue" was created before the scheduler was started.

		The second parameter is the address of the data to be sent which is equal
		to lValueToSend.

		The third parameter is the Block time – the time the task should be kept
		in the Blocked state to wait for space to become available on the queue
		should the queue already be full.  In this case we specify a block
		time as 0 because there should always be space in the queue. */
	 xStatus = xQueueSendToBack(xQueue,&lValueToSend,0);

		 if( uxQueueMessagesWaiting( xQueue ) != 0 ) {
			 puts ( "Queue should have been empty!\r\n" );
		 }


		/* To-do:
			Call the API function taskYIELD() to allow the other sender task to execute. */
		taskYIELD();
		
	}
}
/*-----------------------------------------------------------*/

static void vReceiverTask( void *pvParameters )
{
	/* Declare the variable with the long type that will hold the values received 
		from the queue. */
	long lReceivedValue;
	
	/*	Declare a variable named as xStatus with the type of portBASE_TYPE. It holds
		the return value of the function xQueueReceive() */	
	portBASE_TYPE xStatus;	
	
	const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
		/* As this task unblocks immediately that data is written to the queue this
		call should always find the queue empty. */
		if( uxQueueMessagesWaiting( xQueue ) != 0 )
		{
			printf( "Queue should have been empty!\r\n" );
		}

		/*To-do:
			Call the xQueueReceive() API function to send data to the queue by passing 
			three parameters as described above; and assign the returned value to xStatus.

		The first parameter is the queue from which data is to be received.  The
		queue "xQueue" is created before the scheduler is started.

		The second parameter is the buffer into which the received data will be
		placed.  In this case the buffer is simply the address of the variable 
		"&lReceivedValue" that has the required size to hold the received data. 

		the last parameter is the block time "xTicksToWait" – the maximum amount of time that the
		task should remain in the Blocked state to wait for data to be available should
		the queue already be empty. */

xStatus = xQueueReceive ( xQueue, &lReceivedValue, xTicksToWait );

		/*To-do: check whether the value of xStatus is equal to pdPASS. If yes, print out the following string. */	
		if(xStatus == pdPASS)
		{
			/* Data was successfully received from the queue, print out the current value of lReceivedValue. */
			printf( "Received = %ld\r\n", lReceivedValue );
		}
		else
		{
			/* We did not receive anything from the queue even after waiting for 100ms.
			This must be an error as the sending tasks are free running and will be
			continuously writing to the queue. */
			printf( "Could not receive from the queue.\r\n" );
		}
	}
}
