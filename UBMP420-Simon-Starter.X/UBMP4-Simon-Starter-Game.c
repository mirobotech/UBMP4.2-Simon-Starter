/*==============================================================================
 Project: UBMP4-Simon-Starter-Game
 Date:    June 25, 2022

 A Simon-style pattern matching game starter framework with complete game play.
 Just add startup, winning, and losing sound and lighting effects as well as
 a score display function to customize your own game!
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
#define simon 1                 // Traditional pattern matching game mode
                                // Create additional (optional) game modes here

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
    if(SW2 == pressed)
    {
        return(1);
    }
    else if(SW3 == pressed)
    {
        return(2);
    }
    else if(SW4 == pressed)
    {
        return(3);
    }
    else if(SW5 == pressed)
    {
        return(4);
    }
    else
    {
        return(0);
    }
}

/*==============================================================================
 LED function. Light one of the 4 LEDs. 0 = all LEDs off.
==============================================================================*/

void LED(unsigned char num)
{
    if(num == 1)
    {
        LED2 = 1;
    }
    else if(num == 2)
    {
        LED3 = 1;
    }
    else if(num == 3)
    {
        LED4 = 1;
    }
    else if(num == 4)
    {
        LED5 = 1;
    }
    else
    {
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        LED5 = 0;
    }
}

/*==============================================================================
 Music note functions. Plays note for requested cycles of time period.
 =============================================================================*/

void note_E5(unsigned int per)  // Plays note E5 for requested number of cycles
{
    for(per; per != 0; per--)
    {
        BEEPER = !BEEPER;
        __delay_us(758);
    }
}

void note_CS6(unsigned int per)  // Plays note C#6 for requested number of cycles
{
    for(per; per != 0; per--)
    {
        BEEPER = !BEEPER;
        __delay_us(451);
    }
}

void note_A5(unsigned int per)  // Plays note A5 for requested number of cycles
{
    for(per; per != 0; per--)
    {
        BEEPER = !BEEPER;
        __delay_us(568);
    }
}

void note_E6(unsigned int per)  // Plays note E6 for requested number of cycles
{
    for(per; per != 0; per--)
    {
        BEEPER = !BEEPER;
        __delay_us(379);
    }
}

/*==============================================================================
 Sound and game status functions. 
 =============================================================================*/

void sound(unsigned char num)   // Play selected note sound
{
	if(num == 1)
        note_E5(379);
	else if(num == 2)
		note_CS6(568);
	else if(num == 3)
		note_E6(758);
	else if(num == 4)
		note_A5(451);	
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

//    // Button, LED, and sound test code (comment this out later)
//    while(1)
//    {
//		button = get_button();  // Read button code
//		LED(button);            // Turn on button LED
//        sound(button);          // Play button sound
//		LED(0);                 // Turn off button LED
//
//        // Activate bootloader if SW1 is pressed.
//        if(SW1 == 0)
//        {
//            RESET();
//        }
//    }
    
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

            // Play the complete new pattern steps
            for(step = 0; step != maxStep; step++)
            {
                LED(pattern[step]);
                sound(pattern[step]);
                LED(0);
                __delay_ms(500);
            }

            // Let the user try to match the pattern
            step = 0;               // Reset the step count
            while(step != maxStep && mode == simon)
            {
                idle = 0;               // Reset idle timer for each user guess
                button = get_button();  // Is a button pressed?
                while(button == 0 && mode == simon)  // No, Wait for button press
                {
                    __delay_ms(50);
                    button = get_button();
                    idle++;         // Increment idle timer and check for shutdown
                    if(idle == timeOut)
                    {
                        mode = off;
                    }
                }
                LED(button);        // Show button and play sound for each guess
                sound(button);
                LED(0);

                if(button != pattern[step] && mode == simon)
                {
                    __delay_ms(500);
                    game_lose();    // Uh, oh. Button didn't match pattern
                    mode = off;
                }

                step++;             // Pattern matched! Go on to the next step
                idle = 0;           // Reset idle timer while waiting for key release
                while(get_button() != 0 && mode == simon)
                {
                    __delay_ms(50);
                    idle++;          // Increment idle timer and check for shutdown
                    if(idle == timeOut)
                    {
                        mode = off;
                    }
                }
            }
        }
    }
}

