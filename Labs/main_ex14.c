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

/* The tasks to be created. */
static void vIntegerGenerator( void *pvParameters );
static void vStringPrinter( void *pvParameters ); 

/*-----------------------------------------------------------*/
unsigned char index = 0;

void extint_detection_callback(void);

/* To-do:
	Declare two variables of type xQueueHandle.  
	One queue xIntegerQueue holds integer items; the other xStringQueueu holds the string items.
	Usage:
	One queue will be read from within an ISR, the other will be written to within an ISR. */

	xQueueHandle xIntegerQueue, xStringQueueu;



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

void configure_extint_channel(void)
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin           = BUTTON_0_EIC_PIN;
	config_extint_chan.gpio_pin_mux       = BUTTON_0_EIC_MUX;
	config_extint_chan.gpio_pin_pull      = EXTINT_PULL_UP;
	config_extint_chan.detection_criteria = EXTINT_DETECT_RISING; //EXTINT_DETECT_BOTH;
	extint_chan_set_config(BUTTON_0_EIC_LINE, &config_extint_chan);
}

void configure_extint_callbacks(void)
{
	extint_register_callback(extint_detection_callback, BUTTON_0_EIC_LINE,
	EXTINT_CALLBACK_TYPE_DETECT);
	
	extint_chan_enable_callback(BUTTON_0_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
}

/*-----------------------------------------------------------*/

int main( void )
{
	system_init();
	
	configure_console();
	
	printf("Experiment 14 starts\r\n");
	
	configure_extint_channel();
	
	configure_extint_callbacks();

	system_interrupt_enable_global();
	
    /* To-do: Call xQueueCreate() API functions twice to generate two queues. 
		The two parameters of the first xQueueCreate() call are 10 and sizeof(unsigned long) respectively,
		the return value will be assigned to the queue variable declared above which holds integer items.
		The two parameters of the second xQueueCreate() call are 10 and sizeof(char *) respectively,
		the return value will be assigned to the queue variable declared above which holds string items.
	
		Before a queue can be used it must first be created.  Create both queues
		used by this example.  One queue can hold variables of type unsigned long,
		the other queue can hold variables of type char*.  Both queues can hold a
		maximum of 10 items.  A real application should check the return values to
		ensure the queues have been successfully created. */
	






	/* Create the task that uses a queue to pass integers to the interrupt service
	routine.  The task is created at priority 1. */
	xTaskCreate( vIntegerGenerator, "IntGen", 240, NULL, 1, NULL );

	/* Create the task that prints out the strings sent to it from the interrupt
	service routine.  This task is created at the higher priority of 2. */
	xTaskCreate( vStringPrinter, "String", 240, NULL, 2, NULL );
	

	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	running.  If we do reach here then it is likely that there was insufficient
	heap available for the idle task to be created. */
	for( ;; );
}

static void vIntegerGenerator( void *pvParameters )
{
	portTickType xLastExecutionTime;
	unsigned portLONG ulValueToSend = 0;

	/* Initialize the variable used by the call to vTaskDelayUntil(). */
	xLastExecutionTime = xTaskGetTickCount();

	for( ;; )
	{
		/* This is a periodic task.  Block until it is time to run again.
		The task will execute every 200ms. */
		vTaskDelayUntil( &xLastExecutionTime, 200 / portTICK_RATE_MS );

		/* Send an incrementing number to the queue five times.  These will be
		read from the queue by the interrupt service routine.  A block time is
		not specified. */
		for(; index < 5; index++ )
		{
			/* To-do: Call xQueueSendToBack() API. 
				The first parameter is the integer queue variable you declared at the beginning.
				The second parameter is the reference to the variable ulValueToSend.
				The last parameter is 0.
			*/
			
			
			
						
			ulValueToSend++;
		}

        /* Generate the interrupt, printing a message both before hand and
        afterwards so the sequence of execution is evident from the output. */
		printf( "vIntegerGenerator task - Please Press SW0 button.\r\n" );
	}
}

static void vStringPrinter( void *pvParameters )
{
	char *pcString;

	for( ;; )
	{
		/* To-do: Call xQueueReceive() API and pass three parameters.
			The first parameter is the string queue variable you created at the beginning.
			The second one is the address pointed to the string variable pcString.
			The last one is portMAX_DELAY. */
		/* Block on the queue to wait for data to arrive. */




		/* Print out the string received. */
		printf( pcString );
	}
}

void extint_detection_callback(void)
{
	volatile bool pin_state = port_pin_get_input_level(BUTTON_0_PIN);
	port_pin_set_output_level(LED_0_PIN, pin_state);
	
	extint_chan_clear_detected(BUTTON_0_EIC_LINE);
	
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	static unsigned long ulReceivedNumber;

	/* The strings are declared static const to ensure they are not allocated to the
	interrupt service routine stack, and exist even when the interrupt service routine
	is not executing. */
	static const char *pcStrings[] =
	{
		"String 0\r\n",
		"String 1\r\n",
		"String 2\r\n",
		"String 3\r\n"
	};
	
	/* Check if EIC push button line interrupt line is pending. */
	printf("button pressed. \r\n");
	
	// reset the index
	index = 0;
		
		/* Loop until the queue is empty. */
	while( xQueueReceiveFromISR( xIntegerQueue, &ulReceivedNumber, &xHigherPriorityTaskWoken ) 
			!= errQUEUE_EMPTY )
	{
		/* Truncate the received value to the last two bits (values 0 to 3 inc.), then
		send the string    that corresponds to the truncated value to the other
		queue. */
		ulReceivedNumber &= 0x03;
			
		/*To-do: 
			If needed, Replace the first parameter "xStringQueue" of xQueueSendToBackFromISR() API with the 
			string Queue variable your created; otherwise, keep it unchanged.
			*/			
			xQueueSendToBackFromISR( xStringQueue, &pcStrings[ ulReceivedNumber ], &xHigherPriorityTaskWoken );
					
					
		}

		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	    printf( "Interrupt - send to Queue.\r\n" );
}

