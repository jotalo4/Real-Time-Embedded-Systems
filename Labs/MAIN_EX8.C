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

/* The task functions. */
static void vTask1( void *pvParameters );
static void vTask2( void *pvParameters );

/* Used to hold the handle of Task2. */
/*To-do:
	Declare two global variables xTask1Handle and xTask2Handle with the type
	xTaskHandle to hold the handle of vTask1 and vTask2 respectively.*/
 xTaskHandle xTask1Handle;
 xTaskHandle xTask2Handle;

/*-----------------------------------------------------------*/

int main( void )
{
	system_init();
	
	configure_console();
	
	printf("Experiment 8 starts\r\n");
	
	/* To-do:
		Create the task instance Task1 of vTask1 at priority 2.  
		The last parameter is set as &xTask1Hanle to obtain a handle to the 
		created task1 */

	xTaskCreate(vTask1,"Task1",200,NULL,2,&xTask1Handle);

	/* To-do:
		Create the task instance Task2 of vTask2 at priority 1 - which is lower 
		than the priority of Task1. The last parameter is set as &xTask2Hanle to 
		obtain a handle to the created task2 */
	xTaskCreate(vTask2,"Task2",200,NULL,1,&xTask2Handle);

	
	
	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	running.  If we do reach here then it is likely that there was insufficient
	heap available for the idle task to be created. */
	for( ;; );
}

/*-----------------------------------------------------------*/
static void vTask1( void *pvParameters )
{
	volatile unsigned long ul;

	/*To-do:
		Declare a local variable uxPriority with the type unsigned portBASE_TYPE */
	unsigned portBASE_TYPE uxPriority;
	
	/* This task will always run before Task2 as it has the higher priority.
	Neither Task1 nor Task2 ever block so both will always be in either the
	Running or the Ready state.
	Query the priority at which this task is running - passing in NULL means
	"return our own priority". */
	/*To-do:
		Assign the variable uxPriority with the priority of Task 1 itself, 
		which is returned from the function uxTaskPriorityGet(). 
		The parameter passed to this function is NULL. */
	uxPriority=uxTaskPriorityGet(NULL);
	
	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		printf( "Task 1 is running-------Raise the priority of Task 2----\r\n\r\n" );

		printf( "\r\n" );
		
		/* Delay for a period. */
		for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
		{
			/* This loop is just a very crude delay implementation.  There is
			nothing to do in here.  Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}

		/* Setting the Task2 priority above the Task1 priority will cause
		Task2 to immediately start running (as then Task2 will have the higher 
		priority among two created tasks). */
		/*To-do:
			Call the function vTaskPrioritySet() with two paramters. The first one 
			is the handle of task 2; the second one is uxPriority + 1. */
	vTaskPrioritySet(xTask2Handle,(uxPriority+1));


		/* Task1 will only run when it has a priority higher than Task2.
		Therefore, for this task to reach this point Task2 must already have
		executed and set its priority back down to lower than Task1. */
		
	}
}
/*-----------------------------------------------------------*/

static void vTask2( void *pvParameters )
{
	unsigned portBASE_TYPE uxPriority;
	volatile unsigned long ul;
	/*To-do:
		Assign uxPriority with the priority of Task 1, which is returned from 
		uxTaskPriorityGet(). The parameter passed to this function is the
		handle to the created task1. */
	uxPriority =uxTaskPriorityGet(&xTask1Handle);
	
	/* As per most tasks, this task is implemented in an infinite loop. */
	for( ;; )
	{
		/* Print out the name of this task. */
		printf( "Task 2 is running-------Lower the priority of Task 2----\r\n\r\n" );

		printf( "\r\n" );

		/* Delay for a period. */
		for( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
		{
			/* This loop is just a very crude delay implementation.  There is
			nothing to do in here.  Later exercises will replace this crude
			loop with a proper delay/sleep function. */
		}
		/* Set our priority back down to its original value.  Passing in NULL
		as the task handle means "change our own priority".  Setting the
		priority below that of Task1 will cause Task1 to immediately start
		running again. */
		/*To-do:
			Call the function vTaskPrioritySet() with two parameters. The first one 
			is the handle of task 2 itself which is NULL; the second one could be 
			uxPriority - 1. */		
	vTaskPrioritySet(NULL,(uxPriority-1));


	}
}	