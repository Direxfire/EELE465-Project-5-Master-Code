#include <msp430.h>
/*
 * UART.c
 *
 *  Created on: Mar 29, 2023
 *      Author: Drew
 */


extern UART_Message_Length;
extern UART_Position_Counter;
extern UART_Message_Global[64];

void Setup_UART(void){
    // Take eUSCI_B1 out of software reset with UCSWRST = 0
    UCB1CTLW0 &= ~UCSWRST;

    // UART Communication to PC, Setup UART A1 (Tx)

    UCA1CTLW0 |= UCSWRST;

    UCA1CTLW0 |= UCSSEL__SMCLK; // Using SM clock

    UCA1BRW = 6;

    UCA1MCTLW |= 0x2081;

    P4SEL1 &= ~BIT3; // Use pin 4.7 for UART TX to Bluetooth
    P4SEL0 |= BIT3;


    P4SEL1 &= ~BIT2; // Use pin4.6 for UART RX on Bluetooth
    P4SEL0 |= BIT2;


    UCA1CTLW0 &= ~UCSWRST; // Take UART A1 out of SW Reset
}


void Send_UART_Message(void){
    UART_Position_Counter = 0;
    UART_Message_Length = 40;
    UCA1IE |= UCTXCPTIE;                                             //Enable the TX IRQ
    UCA1IFG &= ~UCTXCPTIFG;                                          //Clear the TX IFG
    UCA1TXBUF = UART_Message_Global[UART_Position_Counter];          //Put first value into the tx buffer

}
