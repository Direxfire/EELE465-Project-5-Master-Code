#include <msp430.h> 
#include <string.h>
void Setup_I2C_Module(void);
void Send_I2C_Message(char*, int);

void Setup_Keypad_Ports(void);
char Decode_Input(int);

//Global Variables needed for I2C communication
int I2C_Message_Counter = 0; //Index value to count the position in the message being written out I2C
char I2C_Message_Global[32]; 		 //Create an empty "string" to hold the final message as it is being sent out I2C
/*This section contains the test variables used to verify the functionality of the system



char Test_Char[] = "A";
char* Test_Char_ptr = Test_Char;
int Test_Slave_Address = 0x048;

*/



//Keypad/Locked code globals
int Passcode_Inputs[5] = {0,0,0,0,0};       //Three digit passcode, the last value holds the locked state
int Input_Counter = 0;
int Temp_In[2] = {0,0};
int Status = 1;
char Input_Arr[3] = {0,0,0};
int New_Input = 0;


//Keypad functions prototypes



//Program Flow function prototypes

void Time_Out(void);
int Locked_Status(void);
int Unlocked_Status(void);
//Program flow globals
char Locked_Code[1];
char *Locked_Code_ptr = Locked_Code;
char Unlocked_ASCII[1];
char *Unlocked_ASCII_ptr = Unlocked_ASCII;

int Unlocked_Input;


/* 
Final revision for the Project 4 code
The goal of this was to create a completely stable version with the I2C bug fixed
Additionally, all functionality will be put into function to allow for a modular design

Group: Drew Currie, Gary Jiang, Johnathan Norell
Written 03/27/23 Last Revision 03/27/23 by Drew.

*/
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
	// Using some simple IO to debug
	P6DIR |= BIT6;
	P6OUT &= ~BIT6;

	// Need to create a simple I2C transmission protocol

	// Call I2C Setup function
	Setup_I2C_Module();
	Setup_Keypad_Ports();

	// After setup don't forget to enable GPIO....
	PM5CTL0 &= ~LOCKLPM5;
	__enable_interrupt();

	/*/
	//Now to create a loop that tests sending a single character out using the Send_I2C_Message
	Send_I2C_Message(Test_Char_ptr, Test_Slave_Address);
	while(1){
		//Set LED to indicate that it finished
		P6OUT ^= BIT6;
		int i;
		for(i = 10000; i > 0; i--){}
	}
	*/
	// Below is an ifinate loop of going from locked -> unlocked -> locked -> ... based on user inputs.
	// The system can sit in either the Locked_Status or the Unlocked_Status infinately.
	while (1)
	{
		int Unlocked = Locked_Status();
		Unlocked = Unlocked_Status();
	}
	return 0;
}

//Send I2C Message Function --> Only works if it's in the main file .... thanks Code Composer for being a shit compiler....




/*
Interrupt Service Routines

*/

#pragma vector = EUSCI_B1_VECTOR
__interrupt void EUSCI_B1_I2C_ISR(void)
{
	//Send I2C Message
	UCB1TXBUF = I2C_Message_Global[I2C_Message_Counter];		//Send the next byte in the I2C_Message_Global string
	I2C_Message_Counter++;										//Increase the message position counter

}

#pragma vector = PORT1_VECTOR
__interrupt void ISR_Port1_Inputs(void)
{
	switch (Status)
	{

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
		if (Input_Counter == 3)
		{
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

	case 0: // This is used for when we want to get just a single input

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
