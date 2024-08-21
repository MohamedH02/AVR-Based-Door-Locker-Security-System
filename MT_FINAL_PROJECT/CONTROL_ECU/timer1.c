 /******************************************************************************
 *
 * Module: Timer1
 *
 * File Name: Timer1.c
 *
 * Description: Source file for Timer1
 *
 * Author: Mohamed Hisham
 *
 *******************************************************************************/

#include "timer1.h"
#include <avr/io.h> /* To use ICU/Timer1 Registers */
#include <avr/interrupt.h> /* For ICU ISR */


/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after overflow*/
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after reaching the compare value*/
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description
 	 * Function to initialize the Timer driver
* Inputs: pointer to the configuration structure with type Timer1_ConfigType.
* Return: None
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr){

	TCCR1A = (1<<FOC1A);
	TCCR1B = (Config_Ptr->mode<<WGM12);
	TCCR1B |= (Config_Ptr->prescaler);
	TCNT1 = Config_Ptr->initial_value;

	if(Config_Ptr->mode)
	{
		OCR1A = Config_Ptr->compare_value;
		TIMSK = (1<<OCIE1A);
	}
	else
	{
		TIMSK = (1<<TOIE1);
	}
}

/*
 * Description
 	 * Function to disable the Timer1.
* Inputs: None
* Return: None
 */
void Timer1_deInit(void){
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0;
	TIMSK = 0;
}

/*
 * Description
 	 *Function to set the Call Back function address.
* Inputs: pointer to Call Back function.
* Return: None
 */
void Timer1_setCallBack(void(*a_ptr)(void)){

	/* Save the address of the Call back function in a global variable */
		g_callBackPtr = a_ptr;
}
