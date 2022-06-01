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

/*To-do:
	Define a macro named as "mainSENDER_1" with value 1
	It is used as a unique ID of the sender task 1. */

//unsigned char mainSENDER_1=1;
#define  mainSENDER_1  1
#define  mainSENDER_2  2

/*To-do:
	Define another macro named as "mainSENDER_2" with value 2
	It is used as a unique ID of the sender task 2. */
//unsigned char mainSENDER_2=2;

/* The tasks to be created.  Two instances are created of the sender task while
only a single instance is created of the receiver task. */
static void vSenderTask( void *pvParameters );
static void vReceiverTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* Declare a variable of type xQueueHandle.  This is used to store the queue
that is accessed by all three tasks. */
xQueueHandle xQueue;

/* To-do:
	Declare a struct type "xData" with two elements that will be passed on the queue. 
	The first element "ucValue" is of unsigned char type that holds the data sent to the queue.
	The second element "ucSource" is of unsigned char type that holds the ID of the sender.
	Refer to the lab sheet. */

typedef struct{
	unsigned char ucValue;
	unsigned char ucSource;
	}xData;


/* To-do: 
	Define a constant array named as xStructsToSend of the type "xData" with two elements that will be passed on the queue. 
	Refer to the lab sheet. */
static const xData xStructsToSend[2]=
{
	{100, mainSENDER_1},
	{200, mainSENDER_2}
};

/*-----------------------------------------------------------*/

int main( void )
{
	system_init();
			
	configure_console();
		
	puts("Experiment 11 starts\r\n");

    /* The queue is created to hold a maximum of 3 structs of type xData. */
/* To-do: 
	Call the xQueueCreate() API to create a queue that holds a maximum of 3 xData  
	type data. And assign the created queue to to xQueue. */

	xQueue= xQueueCreate(3,sizeof(xData));

	if( xQueue != NULL )
	{
		/* Create two instances of the task that will write to the queue.  The
		parameter is used to pass the struct that the task should write to the 
		queue, so one task will continuously send xStructsToSend[ 0 ] to the queue
		while the other task will continuously send xStructsToSend[ 1 ].  
		Both tasks are created at priority 2 which is above the priority of the receiver. */
		xTaskCreate( vSenderTask, "Sender1", 240, ( void * ) &( xStructsToSend[ 0 ] ), 2, NULL );
		xTaskCreate( vSenderTask, "Sender2", 240, ( void * ) &( xStructsToSend[ 1 ] ), 2, NULL );

		/* Create the task that will read from the queue.  The task is created with
		priority 1, so below the priority of the sender tasks. */
		xTaskCreate( vReceiverTask, "Receiver", 240, NULL, 1, NULL );

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
	portBASE_TYPE xStatus;
	const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/*To-do:
			Call the xQueueSendToBack() API function to send data to the queue by passing 
			three parameters as described below; and assign the returned value to xStatus.
			
		The 1st parameter is the queue to which data is being sent.  The 
		queue xQueue was created before the scheduler was started, so before this task
		started to execute.

		The 2nd parameter is the address of the struct being sent.  The
		address is passed in as the task parameter "pvParameters". 

		The 3rd parameter is the Block time xTicksToWait" that the task should be kept
		in the Blocked state to wait for space to become available on the queue
		should the queue already be full.  A block time is specified as the queue
		will become full.  Items will only be removed from the queue when both
		sending tasks are in the Blocked state.. */
	xStatus=xQueueSendToBack(xQueue, pvParameters,xTicksToWait);
		

		if( xStatus != pdPASS )
		{
			/* We could not write to the queue because it was full - this must
			be an error as the receiving task should make space in the queue
			as soon as both sending tasks are in the Blocked state. */
			puts( "Could not send to the queue.\r\n" );
		}

		/* Allow the other sender task to execute. */
		taskYIELD();
	}
}
/*-----------------------------------------------------------*/

static void vReceiverTask( void *pvParameters )
{
	/*To-do: 
		Declare a variable "xReceivedStruct" with the type of "xData" struct. 
		It will hold the values received from the queue. */

	xData xReceivedStruct;
		
	portBASE_TYPE xStatus;

	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
		/* As this task only runs when the sending tasks are in the Blocked state, 
		and the sending tasks only block when the queue is full, this task should
		always find the queue to be full.  3 is the queue length. */
		/* To-do:
			Check whether xQueue is full (equal to 3) by calling the uxQueueMessagesWaiting() with the input
			parameter xQueue. 
			If the return value of this function is not equal to 3, print a message "Queue should have been full!\r\n". */
	if (uxQueueMessagesWaiting(xQueue)!=3 )
	{
		printf("Queue should have been full!\r\n");
	}
		
		
		/*To-do:
			Call the xQueueReceive() API function to send data to the queue by passing 
			three parameters as described below; and assign the returned value to xStatus.		
		 The first parameter is the queue from which data is to be received.  The
		queue is created before the scheduler is started, and therefore before this
		task runs for the first time.

		The second parameter is the buffer into which the received data will be
		placed.  In this case the buffer is simply the address of a variable 
		-- &xReceivedStruct. This variable has the required size to hold the received struct. 

		The last parameter is the block time - the maximum amount of time that the
		task should remain in the Blocked state to wait for data to be available 
		should the queue already be empty.  A block time is set to 0 as this
		task will only run when the queue is full so data will always be available. */

	xStatus= xQueueReceive(xQueue, &xReceivedStruct,0);

		if(xStatus == pdPASS) 
		{
			/* Data was successfully received from the queue, print out the received
			value and the source of the value. */
			if( xReceivedStruct.ucSource == mainSENDER_1 )
			{
				printf( "From Sender 1 = %d \r\n", xReceivedStruct.ucValue );
			}
			else
			{
				printf( "From Sender 2 = %d \r\n", xReceivedStruct.ucValue );
			}
		}
		else
		{
			/* We did not receive anything from the queue.  This must be an error 
			as this task should only run when the queue is full. */
			puts( "Could not receive from the queue.\r\n" );
		}
	}
}
