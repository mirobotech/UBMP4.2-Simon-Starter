/*==============================================================================
 Project: UBMP4-Simon-Game
 Date:    June 28, 2022

 A Simon-style pattern matching game.
 * implements interrupts to wake game from low power mode
 * low power shutdown feature
 
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

// Program variables
unsigned char mode = off;       // Current game mode. Start game in off mode
__persistent unsigned char randomSeed;  // Random seed will be updated from TMR0
unsigned char button;           // Button ID (0 = none, 1 = SW1, etc.)
unsigned char pattern[patternLength];	// Pattern memory
unsigned char step = 0;         // Current pattern step (index) counter
unsigned char maxStep = 0;      // Highest pattern step (score) achieved by player
unsigned int idle = 0;          // Idle time-out counter for low-power shut-down

// Declare random number functions and variables
extern int rand();              // MPLAB random number functions
extern void srand(unsigned int);
unsigned int newSeed;           // New random number generator seed values

// Interrupt function. Processes IOC interrupts and wakes game from nap/sleep.

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
    
// get_button function. Return the code of a pressed button. 0 = no button.

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

// LED function. Light one of the 4 LEDs. 0 = all LEDs off.

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

// Music note functions. Plays note for requested cycles of time period.

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

void note_low(unsigned int per) // Plays low note
{
    for(per; per != 0; per--)
    {
        BEEPER = !BEEPER;
        __delay_us(1136);
    }
}

void note_lower(unsigned int per)   // Plays lower note
{
    for(per; per != 0; per--)
    {
        BEEPER = !BEEPER;
        __delay_us(2272);
    }    
}

// Sound and game status functions. 

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
    LED(1);
    note_E5(200);
    LED(4);
    note_A5(200);
    LED(2);
    note_CS6(200);
    LED(3);
    note_E6(200);
    LED(0);
}

void game_win(void)             // Game win function
{
    LED(1);
    LED(2);
    LED(3);
    LED(4);
    note_E6(500);
    __delay_ms(200);
    note_E6(800);
    LED(0);
}

void game_lose(void)             // Pattern fail. Game lose function
{
    note_low(500);               // Sounds, lights?
    __delay_ms(500);
    note_lower(400);
    __delay_ms(500);
    
    maxStep = maxStep -1;       // User step is 1 below maxStep
    while(maxStep >= 10)        // Show score - tens digits
    {
        LED(1);
        __delay_ms(500);
        LED(0);
        __delay_ms(500);
        maxStep = maxStep - 10;
    }
    while(maxStep >= 5)         // Show score - fives digit
    {
        LED(2);
        __delay_ms(500);
        LED(0);
        __delay_ms(500);
        maxStep = maxStep - 5;
    }
    while(maxStep > 0)          // Show score - ones digits
    {
        LED(3);
        __delay_ms(500);
        LED(0);
        __delay_ms(500);
        maxStep = maxStep - 1;
    }
}

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

            // Add code to switch to different game modes here
            
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

            // Play all of the steps in the new pattern
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

