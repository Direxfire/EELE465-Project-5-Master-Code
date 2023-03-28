#include <msp430.h>

/*
 * initalization.c
 *
 *  Created on: Mar 6, 2023
 *      Author: Drew
 */


/*
Functions declared in this file:

void Setup_Keypad_Ports(void);      //Setup the ports for the keypad. Using P1.0->P1.3 and P2.0->P2.3
void Setup_Interrupts(void);        //Setup the interrupt used for the keypad and any others not needed for communications
void Decode_Passcode(void);         //Basically a switch case that decodes the passcode inputs
int Decode_Keypad_Press(char);      //After unlocked takes the input and decodes it into the ASCII char values.


*/

void Setup_Keypad_Ports(void){
    //Setup P1.0->P1.3 as inputs

    P1DIR &= ~0xFF;      //Clear P1 direction = in
    P1REN |= 0xFF;       //Enable Pull up/down resistor
    P1OUT &= ~0xFF;      //Set Pull Down resistor
    P1IES &= ~0xFF;      //Set interrupt sensativity to L-to-H
    P1IFG &= ~0xFF;      //Clear any P1IFG on P1.0->P1.3

    //Setup P2.0->P2.3 as inputs

    P2DIR |= 0xFF;       //Set P2 direction =  out
    P2OUT |= 0xFF;       //Set P2 out on
    P2REN &= ~0xFF;      //Disable Resistor
    //No interrupts needed.

}

void Setup_Interrupts(void){

    //Enable P1 Interrupts on 0xFF
    P1IE |= 0xF;

    __enable_interrupt();
}

void Setup_I2C_Module(void){
    // put eUSCI_B1 into software reset
    UCB1CTLW0 |= UCSWRST;

    // configure eUSCI_B1
    UCB1CTLW0 |= UCSSEL_3; // choose BRCLK = SMCLK = 1MHz
    UCB1BRW = 10;          // divide BRCLK by 10 for SCL = 100kHz

    UCB1CTLW0 |= UCMODE_3; // put into I2C mode
    UCB1CTLW0 |= UCMST;    // Put into Master mode
    UCB1I2CSA = 0x0068;    // slave address = 0x68

    UCB1CTLW1 |= UCASTP_2;        // Auto stop when UCB1TBCNT reached
    UCB1TBCNT = 2; //# of bytes in Set_Time

    // I/O Ports
    // configure ports
    P4SEL1 & -~BIT7; // Want P4.7 = SCL
    P4SEL0 |= BIT7;

    P4SEL1 & -~BIT6; // Want P4.6 = SDA
    P4SEL0 |= BIT6;

    // Take eUSCI_B1 out of software reset with UCSWRST = 0
    UCB1CTLW0 &= ~UCSWRST;

    // enable interrupts
    UCB1IE |= UCTXIE0;
    UCB1IE |= UCRXIE0;

}
