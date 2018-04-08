// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

#define SYSTICK_FREQ 80000000		// 80MHz frequency

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);
void graderInit(void);
int main(void){ // Real Lab13
	// for the real board grader to work
	// you must connect PD3 to your DAC output
    graderInit();
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
// PortE used for piano keys, PortB used for DAC
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init();
  EnableInterrupts();  // enable after all initialization are done

  while(1){
// input from keys to select tone
		unsigned long newTone = Piano_In() ;
		if(newTone != 1){				// if a key is pressed, play a tone
			unsigned long counts = SYSTICK_FREQ/ newTone / 64;
			Sound_Tone(counts-1);	// Counts for systick are: Systick_Freq/NoteFreq/NumCycles
		}
		else										// Else, turn off the music
			Sound_Tone(0);

  }

}

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) {
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


void graderInit(void) {
    unsigned volatile long delay;
    SYSCTL_RCGCGPIO_R |= (SYSCTL_RCGCGPIO_R3 |
													SYSCTL_RCGCGPIO_R2 |
													SYSCTL_RCGCGPIO_R1 );	// Enable PORT D GPIO Clock
	delay = SYSCTL_RCGCGPIO_R;					// Kill some clocks to get PORT D up and running
	//GPIO_PORTD_LOCK_R	   =   0x01;			// Unlock Port D for writting
	//GPIO_PORTD_CR_R	       |=  0x08;			// Allow changes to PD.3
	GPIO_PORTD_DIR_R       &=  ~(0x08);  		// Turn PD3 to input
	//GPIO_PORTD_AMSEL_R     &=  0x80;			// Ensure the analog functionality is turned off
	//GPIO_PORTD_PCTL_R 	   &=  0x80;	       	// PD.3 to GPIO, not alternate function
	//GPIO_PORTD_DEN_R       |=  0x08;			// Enable Digital pins PE.0 - PE.3
}
