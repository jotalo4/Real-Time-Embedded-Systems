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

/* To-do: 
	Declare a variable named as "xCountingSemaphore" of the type xSemaphoreHandle.  
	This is used to reference the semaphore that is used to synchronize a task with an interrupt. */

	xSemaphoreHandle xCountingSemaphore;


/* The tasks to be created. */
static void vHandlerTask( void *pvParameters );
static void vPeriodicTask( void *pvParameters );

void extint_detection_callback(void);

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
	
	printf("Experiment 13 starts\r\n");
	
	configure_extint_channel();
	
	configure_extint_callbacks();

	system_interrupt_enable_global();
	
	 /* To-do: Call xSemaphoreCreateCounting() API to create counting semaphore.
		The first parameter is 10. The second one is 0.
		The return value is assigned to the xCountingSemaphore variable you created above.
	
	Before a semaphore is used it must be explicitly created.  In this example
	a counting semaphore is created.  The semaphore is created to have a maximum
	count value of 10, and an initial count value of 0. */ 
	 
	 xSemaphoreCreateCounting(10,0);
	 
	 

   /*  To-do: use "if(condition)" expression to check the semaphore was created successfully.
    Note if success, the counting semaphore variable will not be NULL. */
   if(xCountingSemaphore!=NULL)
    {
        /* Create the 'handler' task.  This is the task that will be synchronized
        with the interrupt.  The handler task is created with a high priority to
        ensure it runs immediately after the interrupt exits.  In this case a
        priority of 3 is chosen. */
        xTaskCreate( vHandlerTask, "Handler", 240, NULL, 3, NULL );

        /* Create the task that will periodically generate a software interrupt.
        This is created with a priority below the handler task to ensure it will
        get preempted each time the handler task exits the Blocked state. */
        xTaskCreate( vPeriodicTask, "Periodic", 240, NULL, 1, NULL );

        /* Start the scheduler so the created tasks start executing. */
        vTaskStartScheduler();
    }

	/* If all is well we will never reach here as the scheduler will now be
	running.  If we do reach here then it is likely that there was insufficient
	heap available for the idle task to be created. */
	for( ;; );
}

static void vHandlerTask( void *pvParameters )
{
	for( ;; )
    {
       /*
		To-do: Call xSemaphoreTake() API. The first parameter is still the global semaphore	variable xCountingSemaphore, the second parameter is set as portMAX_DELAY.
		
		Use the semaphore to wait for the event.  The task blocks
        indefinitely meaning this function call will only return once the
        semaphore has been successfully obtained - so there is no need to check
        the returned value. */		
		
		xSemaphoreTake(xCountingSemaphore, portMAX_DELAY);
		
		puts( "Handler task - Semaphores taken.\r\n" );
        /* To get here the event must have occurred.  Process the event (in this
        case we just print out a message). */
        puts( "Handler task - Processing event.\r\n" );
    }
}

/*-----------------------------------------------------------*/
static void vPeriodicTask( void *pvParameters )
{
    /* As per most tasks, this task is implemented within an infinite loop. */
    for( ;; )
    {
        /* This task is just used to 'simulate' an interrupt.  This is done by
        periodically generating a software interrupt. */
        vTaskDelay( 500 / portTICK_RATE_MS );

        /* Generate the interrupt, printing a message both before hand and
        afterwards so the sequence of execution is evident from the output. */
		printf( "Periodic task - Please Press SW0 button to generate interrupt.\r\n" );
    }
}

void extint_detection_callback(void)
{
	volatile bool pin_state = port_pin_get_input_level(BUTTON_0_PIN);
	port_pin_set_output_level(LED_0_PIN, pin_state);
	
	extint_chan_clear_detected(BUTTON_0_EIC_LINE);
	
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Check if EIC push button line interrupt line is pending. */
	printf("button pressed. \r\n");
	
	printf( "Interrupt: - about to give an semaphore.\r\n" );
	/* To-do: Call the xSemaphoreGiveFromISR() API three times.
		The first parameter is the counting Semaphore you created above.
		The second parameter is the reference to the variable xHigherPriorityTaskWoken.
		
		'Give' the semaphore three times.  The first will unblock the handler
		task, the following 'gives' are to demonstrate that the semaphore latches
		the events to allow the handler task to process them in turn without any
		events getting lost.  This simulates multiple interrupts being taken by the
		processor, even though in this case the events are simulated within a single
		interrupt occurrence.*/
		 
		xSemaphoreGiveFromISR(xCountingSemaphore,&xHigherPriorityTaskWoken); 
		xSemaphoreGiveFromISR(xCountingSemaphore,&xHigherPriorityTaskWoken);
		xSemaphoreGiveFromISR(xCountingSemaphore,&xHigherPriorityTaskWoken);
		 

		/* Giving the semaphore may have unblocked a task - if it did and the
		unblocked task has a priority equal to or above the currently executing
		task then xHigherPriorityTaskWoken will have been set to pdTRUE and
		portEND_SWITCHING_ISR() will force a context switch to the newly unblocked
		higher priority task.

		NOTE: The syntax for forcing a context switch within an ISR varies between
		FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
		the Cortex M3 port layer for this purpose.  taskYIELD() must never be called
		from an ISR! */
		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	    printf( "Interrupt - Semaphore generated.\r\n" );
}

