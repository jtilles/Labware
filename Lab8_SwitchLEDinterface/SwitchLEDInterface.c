// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
//#include <stdint.h>

// ***** 2. Global Declarations Section *****
#define BIT0	 	0x01
#define BIT1		0x02

#define SWITCH	BIT0
#define LED			BIT1


// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void init();									// Intialize ports
void setLED();								// Turn on LED
void clearLED();							// Turn off LED
//void delay(uint16_t milliSeconds); //  Delay for set number of milliseconds
void delay(short milliSeconds); //  Delay for set number of milliseconds
// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  
	
  EnableInterrupts();           // enable interrupts for the grader
	init();
  while(1){
		setLED();	// turn led on
		while (!(GPIO_PORTE_DATA_R & SWITCH)){			// While button is not pressed, do nothing
			continue;
		}
		while (GPIO_PORTE_DATA_R & SWITCH){
			delay(10);																// set LED to blink at 5Hz
			GPIO_PORTE_DATA_R ^= LED;									// toggle the LED's state
		}
  }
  
}


void init(){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;						// Port E clock
	volatile unsigned long delay = SYSCTL_RCGC2_R;	// delay?
	GPIO_PORTE_LOCK_R = 0x4C4F434B;
	GPIO_PORTE_CR_R |= (SWITCH | LED);							// Enable writting to bits 0 and 1
	GPIO_PORTE_AMSEL_R &=  ~(LED | SWITCH);					// Turn of ADC capabilities
	GPIO_PORTE_AFSEL_R &=  ~(LED | SWITCH);					// Disable alternate functions
	GPIO_PORTE_PCTL_R = 0x000000;										// Configure for GPIO pins and no other functionality
	GPIO_PORTE_DIR_R |=	LED;												// Make LED an output
	GPIO_PORTE_DEN_R |= (LED | SWITCH);							// Enable GPIOs for the LED and the SWITCH
}

void setLED(){
	GPIO_PORTE_DATA_R |= LED;
	return;
}

void clearLED(){
	GPIO_PORTE_DATA_R &= ~LED;
	return;
}


//void delay(uint16_t milliSeconds){
void delay(short milliSeconds){
	 unsigned long i;
  while(milliSeconds > 0){
    i = 13333;  // this number means 1ms
    while(i > 0){
      i --;
    }
    milliSeconds --; // decrements every 1 ms
  }
}
