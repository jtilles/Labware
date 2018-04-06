// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"

#define SYSTICK_FREQ 1000000

unsigned char sineWave[16] = {8, 10, 13, 14, 15, 14, 13, 10, 8, 5, 2, 1, 0, 1, 2, 5};

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
  // Initialize Systick interrupts
	NVIC_ST_CTRL_R &= ~(NVIC_ST_CTRL_ENABLE);  	// Disable Systick during setup
	NVIC_ST_RELOAD_R = 0xFFFFFF;								// Intialize reload value to largest possible value
	NVIC_ST_CURRENT_R = 0;											// Intialize current systick count to 0
	NVIC_SYS_PRI3_R &= ~(NVIC_SYS_PRI3_TICK_M);	// Set systick interrupt on highest priority (0)
	NVIC_ST_CTRL_R |= 
							( NVIC_ST_CTRL_ENABLE |					// Enable systick
								NVIC_ST_CTRL_INTEN	|					// Enable systick to generate interupts
								NVIC_ST_CTRL_CLK_SRC);				// Use system clock for Systick source
	
	DAC_Init(); 		// Sets up pins for DAC
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
// this routine sets the RELOAD and starts SysTick
	NVIC_ST_RELOAD_R = period;
//	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE; 			// Enable systick

}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
//	NVIC_ST_CTRL_R &= ~(NVIC_ST_CTRL_ENABLE);		// Turn off systick
	DAC_Out(0);
	NVIC_ST_RELOAD_R = 0;
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
		//DAC_Out(GPIO_PORTB_DATA_R ^ 0x08);
		static char counter;
		DAC_Out(sineWave[counter]);
		counter= ((counter + 1) & 0x0F); 	// Resets counter after overflow (hypothetically)
}
