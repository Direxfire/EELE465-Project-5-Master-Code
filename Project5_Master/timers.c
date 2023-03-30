#include<msp430.h>

//Setting up timers to trigger the .333 second analog read in 


void Setup_TimerB0_A2D(int Timer_Period){
    /*
    // configure TimerA for interrupts
    TA0CTL |= TASSEL_2 | ID_3 | MC_1 | TACLR;
    TA0CCR0 = TIMER_PERIOD - 1;	// .333 seconds
    TA0CCTL0 |= CCIE;	// Enables Timer Interupt
    */

   //Using timer B0 to trigger a .333ms interrupt to poll for the current temp.

   TB0CTL |= TBCLR;                         //Clear timers and dividers
   TB0CTL |= TBSSEL__SMCLK;                 //Source is SMCLOCK (1Mhz)
   TB0CTL |= MC__STOP;                      //Put timer into stop mode, change after sample size collected
   TB0CCR0 = Timer_Period;                  //Set CCR0 to the value put into this function

   TB0CCTL0 |= CCIE;                        //Enable CCR0 interrupt
   TB0CCTL0 &= ~CCIFG;                      //Clear interrupt flag

}


