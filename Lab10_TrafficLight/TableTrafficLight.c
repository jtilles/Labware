// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define BIT0 	0x01
#define BIT1 	0x02
#define BIT2	0x04
#define BIT3	0x08
#define BIT4	0x10
#define BIT5	0x20
#define BIT6	0x40
#define BIT7	0x80

#define CRYSTAL 0x15
#define NUM_STATES 12

// ***** Traffic Signal Outputs
#define WESTGRN_SOUTHRED 	0x0C
#define WESTYL_SOUTHRED		0x14
#define WESTRED_SOUTHGRN 	0x21
#define WESTRED_SOUTHYL		0x22
#define WESTRED_SOUTHRED	0x24

// ***** Walk Signal Outputs
#define WALK 							0x08
#define DONT_WALK 				0x02
#define BLINK							0x00

#define SHORT_WAIT				250
#define MEDIUM_WAIT				1000
#define LONG_WAIT					3000
//#define DONT_WALK_WAIT		2000
// ***** 2. Global Declarations Section *****


typedef struct state{
	uint8_t output[2];			// Needs two bytes, one for port B and one for F
	uint16_t delay;
	uint8_t nextState[8];
} state;



state FSM[NUM_STATES]={
	{{WESTGRN_SOUTHRED, DONT_WALK}, 	LONG_WAIT, 			{0,0,1,1,1,1,1,1}},
	{{WESTYL_SOUTHRED, DONT_WALK}, 		MEDIUM_WAIT, 		{2,0,2,2,4,4,4,4}},
	{{WESTRED_SOUTHGRN, DONT_WALK}, 	LONG_WAIT, 			{2,3,2,3,3,3,3,3}},
	{{WESTRED_SOUTHYL, DONT_WALK}, 		MEDIUM_WAIT, 		{0,0,2,0,4,0,4,0}},
	{{WESTRED_SOUTHRED, WALK}, 				LONG_WAIT, 			{4,5,5,5,4,5,5,5}},
	{{WESTRED_SOUTHRED, DONT_WALK}, 	SHORT_WAIT, 		{6,6,6,6,6,6,6,6}},
	{{WESTRED_SOUTHRED, BLINK}, 			SHORT_WAIT, 		{7,7,7,7,7,7,7,7}},
	{{WESTRED_SOUTHRED, DONT_WALK}, 	SHORT_WAIT, 		{8,8,8,8,8,8,8,8}},
	{{WESTRED_SOUTHRED, BLINK}, 			SHORT_WAIT, 		{9,9,9,9,9,9,9,9}},
	{{WESTRED_SOUTHRED, DONT_WALK}, 	SHORT_WAIT, 		{10,10,10,10,10,10,10,10}},
	{{WESTRED_SOUTHRED, BLINK}, 			SHORT_WAIT, 		{11,11,11,11,11,11,11,11}},
	{{WESTRED_SOUTHRED, DONT_WALK},		2*MEDIUM_WAIT,	{2,0,2,0,4,0,2,2}}
};

state FLAG;

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); 			// Disable interrupts
void EnableInterrupts(void);  			// Enable interrupts
void initHardware(void);						// Sets up the ports to interact with the hardware
void initSysTick(void);							// Set up the systick timer
void delayMilliSec(uint16_t delay);	// Delay set number of milliseconds
void hardwareCheck(void);								// Light up all the LEDs for verification

// ***** 3. Subroutines Section *****

int main(void){
  uint8_t currentState = 0;
	uint32_t next;
	TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz


  EnableInterrupts();
	initHardware();
	hardwareCheck();

  while(1){
		FLAG = FSM[currentState];
		GPIO_PORTF_DATA_R = FSM[currentState].output[1];		// Output the walk signal
		GPIO_PORTB_DATA_R = FSM[currentState].output[0];		// Output the traffic signals
		delayMilliSec(FSM[currentState].delay);							// Wait prescribed amount of time
		next = 	GPIO_PORTE_DATA_R 													// Read Inputs
						& (BIT0 | BIT1 | BIT2);
		currentState = FSM[currentState].nextState[next];		// get the next state
  }
}

void initHardware(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= (BIT1 | BIT4 | BIT5);     // 1) activate clock for Ports B,E, and F
  delay = SYSCTL_RCGC2_R;           					// allow time for clock to start

	GPIO_PORTB_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port B
	GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port E
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F

	GPIO_PORTB_CR_R = 0xFF;           // allow changes to PB7-0
  GPIO_PORTE_CR_R = 0x1F;           // allow changes to PE7-0
	GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF7-0

	GPIO_PORTB_AMSEL_R = 0x00;        // 3) disable analog on PB
	GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog on PE
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF

	GPIO_PORTB_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PB7-0
	GPIO_PORTE_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PE7-0
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF7-0

  GPIO_PORTB_DIR_R = 0xFF;          								// 5) PB0-5 outputs
	GPIO_PORTE_DIR_R &= ~(BIT0 | BIT1 | BIT2);        // 5) PE0-2 Inputs
	GPIO_PORTF_DIR_R = BIT1 | BIT3;         					// 5) PF3,1 outputs

  GPIO_PORTB_AFSEL_R = 0x00;        // 6) disable alt funct on PB7-0
	GPIO_PORTE_AFSEL_R = 0x00;        // 6) disable alt funct on PE7-0
	GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0

  //GPIO_PORTE_PDR_R = 0x07;          // enable pull-downs on PE0-2

	GPIO_PORTB_DEN_R = 0xFF;          // 7) enable digital I/O on PB5-0
	GPIO_PORTE_DEN_R = 0x07;          // 7) enable digital I/O on PE2-0
  GPIO_PORTF_DEN_R = 0x0F;          // 7) enable digital I/O on PF3,1

	initSysTick();
}

// Initialize the systick timer for precise timing applications
void initSysTick(void){

	SYSCTL_RCC2_R |= 	SYSCTL_RCC2_USERCC2 + 			// Use RCC2
										SYSCTL_RCC2_BYPASS2; 				// Bypass PLL
	SYSCTL_RCC_R 	= 	(SYSCTL_RCC_R & ~(0x1F<<6))	// Clear XTAL
										|	(CRYSTAL << 6);						// Set Crystal Freq in XTAL
	SYSCTL_RCC2_R &=	~(0x07<<4 | 0x01<<13);			// Sets oscillator source to Main Oscillator
	SYSCTL_RCC2_R |=	0x40000000;									// Divide PLL from 400MHz
	SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~ 0x1FC00000)  // clear system clock divider
                  + (4<<22);      							// configure for 80 MHz clock
	while((SYSCTL_RIS_R&0x00000040)==0){};  			// wait for PLLRIS bit, indicating PLL is locked
	SYSCTL_RCC2_R &= ~0x00000800;									// Enable PLL by clearing BYPASS

	NVIC_ST_CTRL_R = 0;		                   			// disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x00FFFFFF;        				// maximum reload value
  NVIC_ST_CURRENT_R = 0;                				// any write to current clears it
  NVIC_ST_CTRL_R = 	NVIC_ST_CTRL_ENABLE |				// enable SysTick with system clock (80 MHz)
										NVIC_ST_CTRL_CLK_SRC;
	//uint32_t ctrlReg = NVIC_ST_CTRL_R;
}

/* Precise delays using systick timer
// Has a known number of system clock cycles neccesary to equal 1ms (timeConst)
//	counts that number of cycles the number of times specified by the input var "delay"
*/
void delayMilliSec(uint16_t delay){
	uint32_t timeConst = 80000;			// Number of ticks of 80 MHz clock to equal 1ms
	while(delay){
		NVIC_ST_RELOAD_R = timeConst-1;		// Set value to count down from
		NVIC_ST_CURRENT_R = 0;          // any write to current clears it
		while((NVIC_ST_CTRL_R & 0x00010000)==0){}	// Wait for count flag to be cleared
		delay--;
		}
}

void hardwareCheck(void){
	uint8_t i;
	for (i=0; i<NUM_STATES; i++){
		GPIO_PORTF_DATA_R = FSM[i].output[1];		// Output the walk signal
		GPIO_PORTB_DATA_R = FSM[i].output[0];		// Output the traffic signals
		delayMilliSec(500);
	}
}
