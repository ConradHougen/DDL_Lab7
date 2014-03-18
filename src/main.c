/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC11xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

#include "driver_config.h"
#include "target_config.h"

#include "uart.h"
#include "timer32.h"
#include "gpio.h"
#include "adc.h"

extern volatile uint32_t UARTCount;
extern volatile uint8_t UARTBuffer[BUFSIZE];

extern double duty;
extern int freq;
extern int adc_freq;
extern double curr_volt;

int LED_ENABLE = 1;
int ADC_ENABLE = 0;

enum menu_t {
	ARM_MENU = 0,

	ADC_CONTROL_MENU,
	ADC_REPORT_FREQ_MENU,

	LED_CONTROL_MENU,
	LED_DUTY_MENU,
	LED_FREQ_MENU
} menu;
static char * arm_menu_msg = "Options: 1.Control LED, 2.Control ADC\n";
static char * adc_control_menu_msg = "Options: 1.ADC Reporting ON, 2.ADC Reporting OFF, 3.Set Reporting Freq, 4.Return\n";
static char * adc_reporting_menu_msg = "Options: 1.Slow, 2.Medium, 3.Fast, 4.Very Fast, 5.Return\n";
static char * control_menu_msg = "Options: 1.Blink ON, 2.Blink OFF, 3.Set Frequency, 4.Set Duty Cycle 5.Return\n";
static char * duty_menu_msg = "Options: 1. 10%, 2. 25%, 3. 50%, 4. 75%, 5. 90%, 6.Return\n";
static char * freq_menu_msg = "Options: 1.Slow, 2.Medium, 3.Fast, 4.Very Fast, 5.Return\n";


int main(void) {
	menu = ARM_MENU;
	int arm_menu_size= sizeof("Options: 1.Control LED, 2.Control ADC\n");
	int adc_control_menu_size = sizeof("Options: 1.ADC Reporting ON, 2.ADC Reporting OFF, 3.Set Reporting Freq, 4.Return\n");
	int adc_reporting_menu_size = sizeof("Options: 1.Slow, 2.Medium, 3.Fast, 4.Very Fast, 5.Return\n");
	int control_size = sizeof("Options: 1.Blink ON, 2.Blink OFF, 3.Set Frequency, 4.Set Duty Cycle\n");
	int duty_size = sizeof("Options: 1. 10%, 2. 25%, 3. 50%, 4. 75%, 5. 90%, 6.Return\n");
	int freq_size = sizeof("Options: 1.Slow, 2.Medium, 3.Fast, 4.Very Fast, 5.Return\n");
	/* Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS */
	/* You may also want to use the Cortex SysTick timer to do this */
	init_timer32(0, TIME_INTERVAL);
	/* Enable timer 0. Our interrupt handler will begin incrementing
	 * the TimeTick global each time timer 0 matches and resets.
	 */
	enable_timer32(0);

	/* Initialize ADC  */
	ADCInit( ADC_CLK );

	/* Initialize GPIO (sets up clock) */
	GPIOInit();
	/* Set LED port pin to output */
	GPIOSetDir( LED_PORT, LED_BIT, 1 );

	/* NVIC is installed inside UARTInit file. */
	UARTInit(UART_BAUD);


	while(1)
	{
		switch(menu)
		{
			case ARM_MENU:
			{
				// send control menu instructions
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTSend((uint8_t *) arm_menu_msg, arm_menu_size);
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;
				while(UARTCount == 0);
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTCount = 0;
				if(UARTBuffer[0] == 49)
				{
					menu = LED_CONTROL_MENU;
				}
				else if(UARTBuffer[0] == 50)
				{
					menu= ADC_CONTROL_MENU;
				}
				else
				{
					LPC_UART->IER = IER_THRE | IER_RLS;			/* Disable RBR */
					UARTSend( (uint8_t *)"invalid input\n", sizeof("invalid input\n") );
					UARTCount = 0;
					LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
				}
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;
				break;
			}

			case ADC_CONTROL_MENU:
			{
				// send control menu instructions
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTSend((uint8_t *) adc_control_menu_msg, adc_control_menu_size);
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;
				while(UARTCount == 0);
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTCount = 0;
				if(UARTBuffer[0] == 49)
				{
					ADC_ENABLE = 1;
				}
				else if(UARTBuffer[0] == 50)
				{
					ADC_ENABLE = 0;
				}
				else if(UARTBuffer[0] == 51)
				{
					menu = ADC_REPORT_FREQ_MENU;
				}
				else if(UARTBuffer[0] == 52)
				{
					menu = ARM_MENU;
				}
				else
				{
					LPC_UART->IER = IER_THRE | IER_RLS;			/* Disable RBR */
					UARTSend( (uint8_t *)"invalid input\n", sizeof("invalid input\n") );
					UARTCount = 0;
					LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
				}
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;
				break;
			}

			case ADC_REPORT_FREQ_MENU:
			{
				// send control menu instructions
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTSend((uint8_t *) adc_reporting_menu_msg, adc_reporting_menu_size);
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;
				while(UARTCount == 0);
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTCount = 0;
				if(UARTBuffer[0] == 49)
				{
					adc_freq = 1;
				}
				else if(UARTBuffer[0] == 50)
				{
					adc_freq = 2;
				}
				else if(UARTBuffer[0] == 51)
				{
					adc_freq = 4;
				}
				else if(UARTBuffer[0] == 52)
				{
					adc_freq = 8;
				}
				else if(UARTBuffer[0] == 53)
				{
					menu = ADC_CONTROL_MENU;
				}
				else
				{
					LPC_UART->IER = IER_THRE | IER_RLS;			/* Disable RBR */
					UARTSend( (uint8_t *)"invalid input\n", sizeof("invalid input\n") );
					UARTCount = 0;
					LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
				}
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;

				break;
			}
			case LED_CONTROL_MENU:
			{
				// send control menu instructions
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTSend((uint8_t *) control_menu_msg, control_size);
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;
				while(UARTCount == 0);
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTCount = 0;
				if(UARTBuffer[0] == 49)
				{
					LED_ENABLE = 1;
				}
				else if(UARTBuffer[0] == 50)
				{
					LED_ENABLE = 0;
				}
				else if(UARTBuffer[0] == 51)
				{
					menu = LED_FREQ_MENU;
				}
				else if(UARTBuffer[0] == 52)
				{
					menu = LED_DUTY_MENU;
				}
				else if(UARTBuffer[0] == 53)
				{
					menu = ARM_MENU;
				}
				else
				{
					LPC_UART->IER = IER_THRE | IER_RLS;			/* Disable RBR */
				    UARTSend( (uint8_t *)"invalid input\n", sizeof("invalid input\n") );
				    UARTCount = 0;
				    LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
				}
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;
				break;
			}
			case LED_DUTY_MENU:
			{
				// send duty menu instructions
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTSend((uint8_t *) duty_menu_msg, duty_size);
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;
				while(UARTCount == 0);
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTCount = 0;
				if(UARTBuffer[0] == 49)
				{
					duty = 0.10;
				}
				else if(UARTBuffer[0] == 50)
				{
					duty = 0.25;
				}
				else if(UARTBuffer[0] == 51)
				{
					duty = 0.50;
				}
				else if(UARTBuffer[0] == 52)
				{
					duty = 0.75;
				}
				else if(UARTBuffer[0] == 53)
				{
					duty = 0.90;
				}
				else if(UARTBuffer[0] == 54)
				{
					menu = LED_CONTROL_MENU;
				}
				else
				{
					LPC_UART->IER = IER_THRE | IER_RLS;			/* Disable RBR */
					UARTSend( (uint8_t *)"invalid input\n", sizeof("invalid input\n") );
					UARTCount = 0;
					LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
				}
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;
				break;
			}
			case LED_FREQ_MENU:
			{
				// send control menu instructions
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTSend((uint8_t *) freq_menu_msg, freq_size);
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;
				while(UARTCount == 0);
				LPC_UART->IER = IER_THRE | IER_RLS;
				UARTCount = 0;
				if(UARTBuffer[0] == 49)
				{
					freq = 1;
				}
				else if(UARTBuffer[0] == 50)
				{
					freq = 2;
				}
				else if(UARTBuffer[0] == 51)
				{
					freq = 4;
				}
				else if(UARTBuffer[0] == 52)
				{
					freq = 8;
				}
				else if(UARTBuffer[0] == 53)
				{
					menu = LED_CONTROL_MENU;
				}
				else
				{
					LPC_UART->IER = IER_THRE | IER_RLS;			/* Disable RBR */
					UARTSend( (uint8_t *)"invalid input\n", sizeof("invalid input\n") );
					UARTCount = 0;
					LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
				}
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;
				break;
			}
			default:
				break;
		}

	}



	return 0 ;
}
