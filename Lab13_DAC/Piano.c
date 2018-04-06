// Piano.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano
// December 29, 2014

// Port E bits 3-0 have 4 piano keys

#include "Piano.h"
#include "..//tm4c123gh6pm.h"


// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void){ 
  // Intialize Port E.0-E.3 as inputs
		unsigned volatile long delay;
		SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;			// Enable PORT E GPIO Clock
		delay = SYSCTL_RCGCGPIO_R;							// Kill some clocks to get PORT E up and running
		GPIO_PORTE_LOCK_R		= 	0x01;						// Unlock Port E for writting
		GPIO_PORTE_CR_R			|= 	0x0F;						// Allow changes to PE.0-PE.3
		GPIO_PORTE_DIR_R 		&= 	~(0x0F);  			// Turn PE.0 - PE.3 to inputs
		GPIO_PORTE_PDR_R		|=	0x0F;						// Set weak pull downs for PE.0 - PE.3
		GPIO_PORTE_AMSEL_R 	&= 	0xF0;						// Ensure the analog functionality is turned off
		GPIO_PORTE_PCTL_R 	&= 	0xF0;						// Set the PE.0 - PE.3 to GPIO, not alternate function
		GPIO_PORTE_DEN_R 		|= 	0x0F;						// Enable Digital pins PE.0 - PE.3
}
// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void){
  unsigned long input = GPIO_PORTE_DATA_R & 0x0F;		// Read input (switches) w/ mask
	
	// Based on the input, output the correct note
	switch(input){
		case KEY0:
			return C;
		
		case KEY1:
			return D;
		
		case KEY2:
			return E;
		
		case KEY3:
			return G;
		
		default: 
			return 1;
	}	
  
}
