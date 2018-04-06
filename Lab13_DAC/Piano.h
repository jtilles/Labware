// Piano.h
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano, Jonathan Valvano
// December 29, 2014

#define KEY0 	0x01
#define KEY1 	0x02
#define KEY2	0x04
#define KEY3	0x08	

// Freqencies (in hertz) of the notes abstracted
typedef enum{
	C = 523,
	D = 587,
	E = 659,
	G = 784
} notes;

// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void); 
  

// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void);
