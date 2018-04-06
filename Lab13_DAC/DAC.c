// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"

#define PORTA 1
#define PORTB 2
#define PORTC 4
#define PORTD 8
#define PORTE 16
#define PORTF 32
#define OUTPUT 1


// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
// Setup B0-B3 as outputs
void DAC_Init(void){
	unsigned volatile long delay;
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;			// Enable PORT B GPIO Clock
	delay = SYSCTL_RCGCGPIO_R;							// Kill some clocks to get PORT B up and running
	GPIO_PORTB_LOCK_R		= 	0x01;						// Unlock Port B for writting
	GPIO_PORTB_CR_R			|= 	0x0F;						// Allow changes to PB.0-PB.3
	GPIO_PORTB_DIR_R 		|= 	0x0F;  					// Turn PB.0 - PB.3 to outputs
	GPIO_PORTB_AMSEL_R 	&= 	0xF0;						// Ensure the analog functionality is turned off
	GPIO_PORTB_DATA_R 	&= 	0xF0;						// Intialize Port B Pins as off
	GPIO_PORTB_PCTL_R 	&= 	0xF0;						// Set the PB.0 - PB.3 to GPIO, not alternate function
	GPIO_PORTB_DEN_R 		|= 	0x0F;						// Enable Digital pins PB.0 - PB.3
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
	//if(data > 0x0F) return;				// Only output data if it is 4 bits
  GPIO_PORTB_DATA_R = data;		// Set DAC output to data
}
