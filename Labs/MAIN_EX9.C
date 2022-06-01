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
	//while(1);	
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


/* The two task functions. */
static void vTask1( void *pvParameters );
static void vTask2( void *pvParameters );

/* To-do:
	Create a global variable xTask2Handle with the type xTaskHandle.
	It is used to hold the handle of Task2. */
xTaskHandle xTask2Handle;

/*-----------------------------------------------------------*/

int main( void )
{
	system_init();
	
	configure_console();
	
	printf("Experiment 9 starts\r\n");
		
	/* To-do: 
		Create the first task instance of vTask1 at priority 1.  The 
		task parameter (4th parameter)	is not used and is set to NULL.  
		The task handle (last parameter) is also not used and set to NULL. */

	xTaskCreate(vTask1,"Task 1",240,NULL,1,NULL);
	
	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();	
	
	for( ;; );
}
/*-----------------------------------------------------------*/

static void vTask1( void *pvParameters )
{
	const portTickType xDelay100ms = 100 / portTICK_RATE_MS;

	for( ;; )
	{
		/* Print out the name of this task. */
		printf( "Task1 is running.\r\n" );

		/* To-do:
			Create task 2 at a higher priority (i.e., 2).  
			Again the task parameter is not used, so is set to NULL. BUT we want to 
			obtain a handle to the task, so pass in the address of the xTask2Handle variable 
			as the last parameter of xTaskCreate() API function. */

	xTaskCreate(vTask2,"Task 2",240,NULL,2,&xTask2Handle);
					
		/* Task2 has/had the higher priority, so for Task1 to reach here Task2
		must have already executed and deleted itself.  Delay for 100ms. */
		vTaskDelay( xDelay100ms );
	}
}

/*-----------------------------------------------------------*/

static void vTask2( void *pvParameters )
{
	/* Task2 does nothing but delete itself. */
	printf( "Task2 is deleting itself\r\n" );
	
	/*To-do: 
		Call the vTaskDelete() function either using a NULL parameter 
		or the handle of xTask2 vTask2Handle.  */
	vTaskDelete(xTask2Handle);
	
}
