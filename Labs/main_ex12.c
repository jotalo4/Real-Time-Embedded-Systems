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


/* To-do: Declare a global variable "binarySem" with the type xSemaphoreHandle. 
This is used to reference the semaphore that is used to synchronize a task with an interrupt. */

xSemaphoreHandle binarySem;


/* The tasks to be created. */
static void vHandlerTask( void *pvParameters );
static void vPeriodicTask( void *pvParameters );

/* The service routine for the interrupt.  This is the interrupt that the
task will be synchronized with. */
void externalInterruptHandler( void );

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
	config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
	extint_chan_set_config(BUTTON_0_EIC_LINE, &config_extint_chan);
}

void extint_detection_callback(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Check if EIC push button line interrupt line is pending. */
	printf("button pressed. \r\n");
	
	printf( "Interrupt: - about to give an semaphore.\r\n" );
	/* 
		To-do: call the xSemaphoreGiveFromISR() API function.
		The first parameter is the global binary semaphore you created above,
		The second parameter is the address pointed to the variable xHigherPriorityTaskWoken.
			
		'Give' the semaphore to unblock the task. */
		 
		xSemaphoreGiveFromISR(binarySem,xHigherPriorityTaskWoken); 
		 

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
	
	printf("Experiment 12 starts\r\n");
	
	configure_extint_channel();
	
	configure_extint_callbacks();

	system_interrupt_enable_global();
	
	/*
		To-do: Call the vSemaphoreCreateBinary() API to create a binary semaphore.
		The input parameter is the variable you declared above. 
	
		Before a semaphore is used it must be explicitly created.  In this example
		a binary semaphore is created. */

	vSemaphoreCreateBinary(binarySem);



    /*  To-do: use "if(condition)" expression to check the semaphore was created successfully.
    Note if success, the binary semaphore variable will not be NULL. */
    if(binarySem!=NULL)
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
    /* To-do: Call xSemaphoreTake() API. The first parameter is the binary semaphore
    variable you created, the second parameter is set to 0.
	
    Take the semaphore once to start with so the semaphore is empty before the
    infinite loop is entered.  The semaphore was created before the scheduler
    was started so before this task ran for the first time.*/

	xSemaphoreTake(binarySem,0);

    for( ;; )
    {
       /*
		To-do: Call xSemaphoreTake() API again. The first parameter is still the global binary semaphore
		variable you created above, the second parameter is set as portMAX_DELAY.
		
		Use the semaphore to wait for the event.  The task blocks
        indefinitely meaning this function call will only return once the
        semaphore has been successfully obtained - so there is no need to check
        the returned value. */
	xSemaphoreTake(binarySem, portMAX_DELAY);


		
		printf( "Handler task - Semaphore taken.\r\n" );
        /* To get here the event must have occurred.  Process the event (in this
        case we just print out a message). */
        printf( "Handler task - Processing event.\r\n" );
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
		printf( "Periodic task - Please Press PB0 button.\r\n" );
    }
}


/**
 * \brief Interrupt handler for EIC interrupt.
 */
#ifdef test
void externalInterruptHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Check if EIC push button line interrupt line is pending. */
	//if (eic_line_interrupt_is_pending(EIC, GPIO_PUSH_BUTTON_EIC_LINE)) {
	//	eic_line_clear_interrupt(EIC, GPIO_PUSH_BUTTON_EIC_LINE);
		printf("button pressed. \r\n");
		
		printf( "Interrupt: - about to give an semaphore.\r\n" );
		/* 
		To-do: call the xSemaphoreGiveFromISR() API function.
		The first parameter is the global binary semaphore you created above,
		The second parameter is the address pointed to the variable xHigherPriorityTaskWoken.
			
		'Give' the semaphore to unblock the task. */
		 
		 xSemaphoreGiveFromISR(binarySem,xHigherPriorityTaskWoken);
		 

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
}
#endif