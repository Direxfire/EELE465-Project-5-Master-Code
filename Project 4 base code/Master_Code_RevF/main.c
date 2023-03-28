#include <msp430.h> 
#include <string.h>
#include <stdlib.h>
/*
Revision E of Project 4 Master Code
Author: Drew Currie
Goal of this file:
    This is the hopefully final revision of the master code before starting the complete system integration in 03/07/23
    This file will be re-organized and optomized to allow for better debugging. The main goal is to slow down the program
    which while that sounds counter-intuative there is not enough delay in the input -> passcode systems which means
    double presses are often recorded in the input passcodes.

    Building from scratch up, to eliminate redundant programming and seek to reuse as much as possible.
Group: Drew Currie, Gary Jiang, Johnathan Norell
Written: 03/06/23   Last Revision: 03/06/23 -DC
*/


/*
Creating initaliztion functions. Starting with the Keypad code then going into the I2C control module

*/

void Setup_Keypad_Ports(void);      //Setup the ports for the keypad. Using P1.0->P1.3 and P2.0->P2.3
void Setup_Interrupts(void);        //Setup the interrupt used for the keypad and any others not needed for communications
void Decode_Passcode(void);         //Basically a switch case that decodes the passcode inputs
char Decode_Keypad_Press(int);      //After unlocked takes the input and decodes it into the ASCII char values.
int Passcode_Check(void);
void Time_Out(void);
char Locked_Code;

//I2C Functions
void Setup_I2C_Module(void);
void Send_I2C_Message(int, char);
void Send_I2C_String(int, char[]);
char Lock_Out_Message[] = {"Wrong Passcode!"};
char Unlocked_Message[] = {"Device Unlocked."};
char I2C_transmit_Message[32];
char I2C_Message[2] = {'\a', '0'};
int I2C_Message_Counter = 0;
int Passcode_Inputs[5] = {0,0,0,0,0};       //Three digit passcode, the last value holds the locked state
int Input_Counter = 0;
int Temp_In[2] = {0,0};
int Status = 1;
int Char_or_String;
int Input_Arr[3] = {0,0,0};
int Unlocked_Input;
char Unlocked_ASCII;
int New_Input = 0;

unsigned int Passcode_Inc = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    Setup_Keypad_Ports();
    Setup_Interrupts();
    Setup_I2C_Module();
    // Temporarily Using P6.6 as output to test when there is an input of any kind on the keypad
    P6DIR |= BIT6;
    P6DIR |= BIT5;
    P6OUT &= ~BIT6;
    P6OUT &= ~BIT5;
    PM5CTL0 &= ~LOCKLPM5; // Enable GPIO
    unsigned int i;
    while (1)
    {
        Locked_Code = '#';
        Send_I2C_Message(0x048, Locked_Code);
        Send_I2C_Message(0x058, Locked_Code);
        Passcode_Inputs[4] = 0;
        P1IE |= 0xF;
        Passcode_Inc = 0;

        while (Passcode_Inputs[4] == 0)
        {
            // Wait for input here:
            while (Passcode_Inputs[3] == 0)
            {
            }
            // Polling loop for P1 input (Not quite the right terminology but waits for the PIFG to trigger)
            for (i = 0; i < 40000; i++)
            {
            }
            P6OUT &= ~BIT6;
            P1IE |= 0xF;
            Passcode_Inputs[3] = 0;
            Locked_Code = Decode_Keypad_Press(Passcode_Inputs[Passcode_Inc]);
            Send_I2C_Message(0x048, Locked_Code);
            Passcode_Inc++;

        }
        P1IE &= ~0xF;
        P6OUT |= BIT5;
        // Call truth table for passcode
        Decode_Passcode();
        // Now to do passcode check
        Status = Passcode_Check();
        if (Status == 1)
        {
            Locked_Code = 'E';
            Send_I2C_Message(0x48,Locked_Code);
            Send_I2C_Message(0x058, Locked_Code);
            Time_Out();
            //Send I2C String here

            int j;
            for(j = 0; j < 30000; j ++){}
            for(j = 0; j < 30000; j ++){}

            for(j = 0; j < 30000; j ++){}
            for(j = 0; j < 30000; j ++){}

            Passcode_Inc = 0;

            for(j = 0; j < 6; j++){
                Passcode_Inputs[j] = 0;

            }
        }
        else if (Status == 0)
        {
            P1IE |= 0xF;
            Unlocked_ASCII = '\a';
            Send_I2C_Message(0x058, Unlocked_ASCII);
            Send_I2C_Message(0x048, Unlocked_ASCII);
            while (Status == 0)
            {
                // Unlocked so loop forever getting the button press as a char input
                if (New_Input == 1)
                {
                    Unlocked_ASCII = Decode_Keypad_Press(Unlocked_Input);
                    for (i = 0; i < 55000; i++)
                    {
                    }
                    P6OUT &= ~BIT6;
                    P6OUT &= ~BIT5;
                    P1IE |= 0xF;
                    New_Input = 0;
                    // Next write the value out the I2C
                    //Testing writing out to 0x058 address the Unlocked_ASCII Character
                    Send_I2C_Message(0x048, Unlocked_ASCII);
                    Send_I2C_Message(0x058, Unlocked_ASCII);

                    if(Unlocked_ASCII == '#'){
                        Status = 1;
                        for(i = 0; i < 6; i++){
                            Passcode_Inputs[i] = 0;
                        }
                        }

                    }
            }
        }
    }
    return 0;
}

//Truth table for the three digit passcode
void Decode_Passcode(void)
{

    unsigned int i;
    for (i = 0; i <= 2/*Size of the passcode*/; i++)
    {
        switch (Passcode_Inputs[i])
        {
        case 0x84:
            Input_Arr[i] = 0x0;
            break;

        case 0x18:
            Input_Arr[i] = 0x1;
            break;

        case 0x14:
            Input_Arr[i] = 0x2;
            break;

        case 0x12:
            Input_Arr[i] = 0x3;
            break;

        case 0x28:
            Input_Arr[i] = 0x4;
            break;

        case 0x24:
            Input_Arr[i] = 0x5;
            break;

        case 0x22:
            Input_Arr[i] = 0x6;
            break;

        case 0x48:
            Input_Arr[i] = 0x7;
            break;

        case 0x44:
            Input_Arr[i] = 0x8;
            break;

        case 0x42:
            Input_Arr[i] = 0x9;
            break;

        case 0x11:
            Input_Arr[i] = 0xA;
            break;

        case 0x21:
            Input_Arr[i] = 0xB;
            break;

        case 0x41:
            Input_Arr[i] = 0xC;
            break;

        case 0x81:
            Input_Arr[i] = 0xD;
            break;

        case 0x88:
            Input_Arr[i] = 0x2A;
            break;

        case 0x82:
            Input_Arr[i] = 0x23;
            break;
        default:
            Input_Arr[i] = 0xF;
            break;
        }
    }
}

//Change the status to determine if unlocked.
int Passcode_Check(){
    //Start with a delay
    int i;
    for(i = 0; i < 25000; i++){}
    int Passcode[3] = {1,2,3};
    //Now check the passcode
    for(i = 0; i <= 2; i++){
        if(Passcode[i] != Input_Arr[i]){
            return(1);
        }
        else{}
    }
    //end with a delay
    for(i = 0; i < 25000; i++){}
    return(0);
}


//Time Out Function
void Time_Out(void){
    P6OUT |= BIT6;
    P6OUT |= BIT5;
    int i;
    for(i = 0; i < 25000; i++){}
    P6OUT &= ~BIT6;
    P6OUT &= ~BIT5;
    for(i = 0; i < 25000; i++){}
    P6OUT |= BIT6;
    P6OUT |= BIT5;
    for(i = 0; i < 25000; i++){}
    P6OUT &= ~BIT6;
    P6OUT &= ~BIT5;
    //Send_I2C_Message(0x58, 'F') Since F is not an option on the keypad, can use it as the time out char to flash all the LEDs on the light bar to indicate the error. 
}
//Decode the keypad presses for the input after unlock
char Decode_Keypad_Press(int Input){
    switch (Input)
        {
        case 0x84:
            return('0');
            break;

        case 0x18:
            return('1');
            break;

        case 0x14:
            return('2');
            break;

        case 0x12:
            return('3');
            break;

        case 0x28:
            return('4');
            break;

        case 0x24:
            return('5');
            break;

        case 0x22:
            return('6');
            break;

        case 0x48:
            return('7');
            break;

        case 0x44:
            return('8');
            break;

        case 0x42:
            return('9');
            break;

        case 0x11:
            return('A');
            break;

        case 0x21:
            return('B');
            break;

        case 0x41:
            return('C');
            break;

        case 0x81:
            return('D');
            break;

        case 0x88:
            return('*');
            break;

        case 0x82:
            return('#');
            break;
        default:
            return('F');
            break;
        }
}

void Send_I2C_Message(int Address, char Message){
    I2C_Message[1] = Message;
    I2C_Message_Counter = 0;
    Char_or_String = 0;
    int i;

    //Get the current time stamp from the I2C
    UCB1CTLW0 |= UCTR; // Tx mode
    UCB1CTLW0 |= UCTXSTT; // generate star condition
    UCB1I2CSA = Address;    // slave address = 0x68
    UCB1TBCNT = 1;  //# of bytes in Set_Time

    while((UCB1IFG & UCSTPIFG) == 0); //wait for stop flag
    for(i = 0; i < 1000; i++){}

}
//Send the entire string up to 32 char long
void Send_I2C_String(int Address, char str[]){
    I2C_Message_Counter = 0;
    Char_or_String = 1;
    int i;

    //Get the current time stamp from the I2C
    UCB1CTLW0 |= UCTR;          // Tx mode
    UCB1CTLW0 |= UCTXSTT;       // generate star condition
    UCB1I2CSA = Address;        // slave address = 0x68
    UCB1TBCNT = sizeof(str);    //# of bytes in Set_Time

    while((UCB1IFG & UCSTPIFG) == 0); //wait for stop flag
    I2C_Message_Counter = 0;
    for(i = 0; i < 1000; i++){}


}


/*
                Interrupts

*/
#pragma vector = PORT1_VECTOR
__interrupt void ISR_Port1_Inputs(void)
{
    switch(Status){

    case 1:

    Temp_In[0] = P1IN;
    P6OUT |= BIT6;
    // Switch to P1 as output, and P2 as input

    // Set P1.0->P1.3 as Outputs
    P1DIR |= 0xFF;
    P1REN |= 0xFF;
    P1OUT |= 0xFF;
    // Set P2.0->P2.3 as Outputs
    P2DIR &= ~0xFF;
    P2REN |= 0xFF;
    P2OUT &= ~0xFF;

    P2IN &= ~0xFF;
    P1IFG &= ~0xFF; // Clear P1 IFG

    Temp_In[1] = P2IN;
    // Rotate the upper nibble
    Temp_In[0] = Temp_In[0] << 4;

    Passcode_Inputs[Input_Counter] = Temp_In[0] | Temp_In[1];
    Input_Counter++;
    if(Input_Counter == 3){
        Input_Counter = 0;
        Passcode_Inputs[4] = 1;
    }
    Passcode_Inputs[3] = 1;
    // Switch to P1 as output, and P2 as input

    // Set P1.0->P1.3 as inputs
    P1DIR &= ~0xFF;
    P1REN |= 0xFF;
    P1OUT &= ~0xFF;
    // Set P2.0->P2.3 as Outputs
    P2DIR |= 0xFF;
    P2REN &= 0xFF;
    P2OUT |= 0xFF;

    // Enable P1.0->P1.3 Interrupts
    P1IN &= ~0xFF;
    P1IFG &= ~0xFF;
    P2IFG &= ~0xFF; // Clear P2 IFG

    P1IE &= ~0xFF;
    break;

    case 0:     //This is used for when we want to get just a single input

    Temp_In[0] = P1IN;
    P6OUT |= BIT6;
    // Switch to P1 as output, and P2 as input

    // Set P1.0->P1.3 as Outputs
    P1DIR |= 0xFF;
    P1REN |= 0xFF;
    P1OUT |= 0xFF;
    // Set P2.0->P2.3 as Outputs
    P2DIR &= ~0xFF;
    P2REN |= 0xFF;
    P2OUT &= ~0xFF;

    P2IN &= ~0xFF;
    P1IFG &= ~0xFF; // Clear P1 IFG

    Temp_In[1] = P2IN;
    // Rotate the upper nibble
    Temp_In[0] = Temp_In[0] << 4;

    Unlocked_Input = Temp_In[0] | Temp_In[1];

    Passcode_Inputs[3] = 1;
    // Switch to P1 as output, and P2 as input

    // Set P1.0->P1.3 as inputs
    P1DIR &= ~0xFF;
    P1REN |= 0xFF;
    P1OUT &= ~0xFF;
    // Set P2.0->P2.3 as Outputs
    P2DIR |= 0xFF;
    P2REN &= 0xFF;
    P2OUT |= 0xFF;

    // Enable P1.0->P1.3 Interrupts
    P1IN &= ~0xFF;
    P1IFG &= ~0xFF;
    P2IFG &= ~0xFF; // Clear P2 IFG

    P1IE &= ~0xFF;
    New_Input = 1;
    break;

    default:
    }

}

#pragma vector = EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void)
{
    // Send LED code here
    switch(Char_or_String){
    case 0: 
        if (Status == 1)
            UCB1TXBUF = Locked_Code;
        else
            UCB1TXBUF = Unlocked_ASCII;
    break;

    case 1:
        UCB1TXBUF =  I2C_transmit_Message[I2C_Message_Counter];
        I2C_Message_Counter++;
    break;
    }
}   

