/*==============================================================================
 Project: UBMP4-Simon-Starter
 Date:    June 25, 2022

 A Simon-style pattern matching game starter framework. Add button, LED, sound,
 winning, losing and score display functions to create your own game!
==============================================================================*/

#include    "xc.h"              // Microchip XC8 compiler include file
#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include    "UBMP420.h"         // Include UBMP4.2 constant and function definitions

// TODO Set linker ROM ranges to 'default,-0-7FF' under "Memory model" pull-down.
// TODO Set linker code offset to '800' under "Additional options" pull-down.

// Program constants
#define pressed 0               // Switch pressed logic state
#define timeOut 2400            // Idle time-out duration (50ms intervals - 1200/min)
#define patternLength 42        // Total number of pattern steps to match

//Game modes
#define off 0                   // Game off
#define simon 1               // Regular pattern matching game mode
                                // Add additional (optional) game modes here

unsigned char mode = off;       // Start in off mode

// Set up program variables
__persistent unsigned char randomSeed;  // Random seed will be updated from TMR0
unsigned char button;           // Button ID (0 = none, 1 = SW1, etc.)
unsigned char pattern[patternLength];	// Pattern sequence memory
unsigned char step = 0;         // Current pattern step/index counter
unsigned char maxStep = 0;      // Highest pattern step/score achieved by player
unsigned int idle = 0;          // Idle time-out counter

// Declare random number functions and variables
extern int rand();              // MPLAB random number functions
extern void srand(unsigned int);
unsigned int newSeed;           // New random number generator seed values

/*==============================================================================
 Interrupt function. Processes IOC interrupts and wakes game from nap/sleep.
 =============================================================================*/

void __interrupt() wake(void)
{
    di();                       // Disable interrupts
	if(IOCIF == 1 && IOCIE == 1)	// When an IOC interrupt occurs...
	{
        IOCAF = 0;              // Clear IOC port A interrupt flag
        IOCBF = 0;              // Clear IOC port B interrupt flag
        IOCIF = 0;				// Clear IOC interrupt flag
	}
	else
    {
        IOCAN = 0b00001000;     // Other interrupt? Only allow IOC on pushbuttons
        IOCBN = 0b11110000;
		INTCON = 0b00001000;
    }
}

// Turn off LEDs, set pin change interrupt, enable interrupts, and sleep
void nap(void)
{
    LED1 = 1;                   // Turn power LED off
    LATC = 0;
    IOCAF = 0;                  // Clear S1 IOC interrupt flag
    IOCBF = 0;                  // Clear S2-S5 IOC interrupt flags
    IOCIF = 0;                  // Clear global IOC interrupt flag
    ei();                       // Enable interrupts
    SLEEP();
}
    
/*==============================================================================
 get_button function. Return the code of a pressed button. 0 = no button.
 =============================================================================*/

unsigned char get_button(void)      // Return code of pressed button or 0 if
{                                   // no buttons are pressed
    // Create the code for your button function
}

/*==============================================================================
 LED function. Light one of the 4 LEDs. 0 = all LEDs off.
==============================================================================*/

void LED(unsigned char num)
{
    // Create the code for your LED function
}

/*==============================================================================
 Music note functions. Plays note for requested cycles of time period.
 =============================================================================*/

void note_1(unsigned int per)  // Play note 1 for the requested period
{
    // Create code for each note
}

void note_2(unsigned int per)  // Play note 2 for the requested period
{
    // Create code for each note
}

void note_3(unsigned int per)  // Play note 3 for the requested period
{
    // Create code for each note
}

void note_4(unsigned int per)  // Play note 4 for the requested period
{
    // Create code for each note
}


/*==============================================================================
 Sound and game status functions. 
 =============================================================================*/

void sound(unsigned char num)   // Play selected note sound
{
	if(num == 1)
        note_1(300);
	else if(num == 2)
		note_2(300);
	else if(num == 3)
		note_3(300);
	else if(num == 4)
		note_4(300);	
}

void game_start(void)           // Game start-up function
{
    // Add code to create your own game starting sounds and light patterns
}

void game_win(void)             // Game win function
{
    // Add code to create your own game wining sound and light pattern
}

void game_lose(void)             // Pattern fail. Game lose function
{
    // Add code to create your own game losing sounds and light patterns
}

/*==============================================================================
	Main program loop. The main() function is executed on power-up and reset.
==============================================================================*/

int main(void)
{
    OSC_config();               // Configure internal oscillator for 48 MHz
    UBMP4_config();             // Configure on-board UBMP4 I/O devices
    
    LED1 = 0;                   // Power up beep
    note_E6(50);
    __delay_ms(100);

    // Button, LED, and sound test (comment this out after testing your functions)
    while(1)
    {
		button = get_button();  // Read button code
		LED(button);            // Turn on button LED
        sound(button);          // Play button sound
		LED(0);                 // Turn off button LED

        // Activate bootloader if SW1 is pressed.
        if(SW1 == 0)
        {
            RESET();
        }
    }
    
    while(1)
    {
        while(mode == off)
        {
            LED(0);                 // Turn all game LEDs off
            nap();                  // Sleep and wait for a button press

            LED1 = 0;               // Turn power LED on and start game
            game_start();
            while(SW2 == pressed || SW3 == pressed || SW4 == pressed || SW5 == pressed);

            // Initialize the random number generator
            randomSeed = TMR0;      // Generate random number from timer
            newSeed = (newSeed << 8 ) | randomSeed; // Mix previous seed with random sample
            srand(newSeed);         // Seed random number generator

            maxStep = 0;            // Clear step count, start simon game mode
            mode = simon;
        }

        while(mode == simon)        // Loop until win, lose, or shutdown
        {
            // Delay for each turn before picking the next random pattern step
            __delay_ms(1000);
            pattern[maxStep] = (rand() & 0b00000011) + 1;
            maxStep++;              // Increase step count, check for win
            if(maxStep == (patternLength + 1))
            {
                game_win();
                mode = off;
            }

            // Play the complete new pattern sequence

            
            // Let the user try to match the pattern

        }
    }
}

