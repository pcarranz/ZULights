
#include <msp430.h> 

/*
 * CPE 329 Final Project - Jeffrey Gerfen
 * Front-wheel LED bike light system.
 * Utilizes input from a reed sensor to 
 * control the behavior of 9 LEDs.
 * By: Patricia Carranza and Jose Armando Villa
 * December 2014
 */

volatile char currentLED = 0;
volatile unsigned int newTime = 10000;

const char leds[12][2] = {
		{BIT2, 0},
		{BIT3, 0},
		{BIT4, 0},
		{BIT5, 0},
		{0, BIT1},
		{0, BIT2},
		{0, BIT4},
		{0, BIT0},
		{0, BIT3},
		{BIT2, 0},
		{BIT3, 0}
    };

void nextLEDOn();


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    int i;
   // 4Mhz SMCLK
    if (CALBC1_1MHZ==0xFF) 	// If calibration constant erased
    {
    	while(1); 				// do not load, trap CPU!!
    }
    DCOCTL = 0; 				// Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;		// Set range
    DCOCTL = CALDCO_1MHZ; 		// Set DCO step + modulation

    // Set LEDs as output
    P1DIR |= BIT2 + BIT3 + BIT4 + BIT5;
    P2DIR |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4;

    P1IE |= BIT0;
    P1IFG &= ~BIT0;

    //Timer setup
   	//CCTL0 = CCIE;               //CCR0 interrupt enabled
   	//CCR0 = 400;
  	TACTL = TASSEL_1 + ID_3 + MC_2;    //SMCLK, div 8, cont. mode



    //_BIS_SR(LPM4_bits + GIE);
  	_enable_interrupts();
  	while (1) {
  		nextLEDOn();
  		for (i = 0; i < newTime; i++)
  			__delay_cycles(102);

  	}

}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1 (void) {
	newTime = TAR >> 2;
	currentLED = 1;
	TAR = 0;
	P1IFG &= ~BIT0;
}

/*
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {
	nextLEDOn();
	CCR0 += newTime;
}
*/

void nextLEDOn() {

	if (++currentLED == 12)
		currentLED = 3;

	_disable_interrupts();
	P1OUT |= leds[currentLED][0];
	P2OUT |= leds[currentLED][1];

	P1OUT &= ~leds[currentLED - 3][0];
	P2OUT &= ~leds[currentLED - 3][1];
	_enable_interrupts();

}
