#include <msp430.h> 
#include <string.h>
#include <math.h>
#include<stdio.h>
#include <stdlib.h>


#define LM19_PIN    BIT0
#define third_second_timer_period 33333


#define Test_LED BIT6

void Setup_I2C_Module(void);
void Send_I2C_Message(char*, int);
void Setup_Keypad_Ports(void);
char Decode_Input(int);

//Global Variables needed for I2C communication
int I2C_Message_Counter = 0; //Index value to count the position in the message being written out I2C
char I2C_Message_Global[32]; 		 //Create an empty "string" to hold the final message as it is being sent out I2C

//UART Function Prototypes
void Setup_UART(void);
void Send_UART_Message(void);

/*
//UART test variables below
char message[] = "Hello World!\r\n";
int position;
//End UART test variables.
*/

//UART Global Variables
char UART_Message_Global[64];
char *UART_Message_ptr = UART_Message_Global;
unsigned int UART_Position_Counter;
int UART_Message_Length;
char Temp_Raw_ASCII[5];
char *Temp_Raw_ASCII_ptr = Temp_Raw_ASCII;
//Analog to digital function prototypes

void Setup_TimerB0_A2D(int);
void Setup_A2D();

/*This section contains the test variables used to verify the functionality of the system



char Test_Char[] = "A";
char* Test_Char_ptr = Test_Char;
int Test_Slave_Address = 0x048;

*/





/*
This section is the struct system for storing all the temperature values.
*/
void Process_Temperature_Data(int);
float Convert_to_Celsius (float);
struct Temperature {
    float Celsius_Float;
    char Upper_Bit[2];
    char Lower_Bit;
    char Kelvin[4];
    int Raw_Value;
};

struct Temperature Temperature_Array[5];

char Temperature_Write_Out[16];
char *Temperature_Write_Out_ptr = Temperature_Write_Out;
int Samples = 0;
int  New_Temp_Value;
int Sample_Number = 0;
float Rolling_Average;
char Rolling_Average_ASCII[4];
char *Rolling_Average_ASCII_ptr = Rolling_Average_ASCII;
float Real_Analog_Value;
/*

 Gary's A2D variables
 These will need changed a bit to make them work with the multi-file structure probably. 
*/


int sum = 0;            // sum of the last three samples
int valid_samples = 0;  // number of valid samples
int window_size = 0;      // size of the averaging window
int window_index = 0;     // index of the current sample in the window
int window_full = 0;      // flag indicating whether the window is full
int samples[10];          // array to store the last n samples
int Fresh_Data = 0;

int Raw_Temp = 0;

//End A2D variables.
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

	//Setup A2D and Timers
	Setup_A2D();
	Setup_TimerB0_A2D(third_second_timer_period);


	//Setup UART
	Setup_UART();
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
	//Testing to make sure the program reaches the main loop setting the P6.6 LED on
	P6DIR |= Test_LED;
	P6OUT |= Test_LED;
	//Putting timerB0 into UP mode for testing...
	TB0CTL |= MC__UP;                      //Put timer into UP mode for testing. 
	int j;
	while (1)
	{
		/* These functions are the locked and unlocked system. These are no longer needed but maybe we can do something cool with them??
		*
		int Unlocked = Locked_Status();
		Unlocked = Unlocked_Status();
		*/
		if(Fresh_Data == 1){
            Fresh_Data = 0;
			Process_Temperature_Data(Raw_Temp);
            snprintf(UART_Message_Global, 100, "Current temperature raw data is: %d. \n\r", Raw_Temp);

		       Send_UART_Message();
		       for(j = 0; j < 10000; j++){}		}

	}
	return 0;
}



/* reverse:  reverse string s in place */
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* itoa:  convert n to characters in s */
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}


void Process_Temperature_Data(int New_Temp_Value){
    //Now we have an array of Temperature structs which we can begin to populate??? And then feed these temperature structs into Gary's struct to hold the rolling average
    char buffer[20];
    Temperature_Array[Sample_Number].Raw_Value = New_Temp_Value;
    New_Temp_Value = 0; //Move the new raw value into the struct then clear the new raw value
    //Now to convert from mV to degrees C
      Real_Analog_Value = Temperature_Array[Sample_Number].Raw_Value*830e-6;
    Temperature_Array[Sample_Number].Celsius_Float =  Convert_to_Celsius(Real_Analog_Value);
    int whole_num = (int) Temperature_Array[Sample_Number].Celsius_Float;
    int frac_num = (int) ((Temperature_Array[Sample_Number].Celsius_Float - whole_num) * 100); // multiply by 100 to get 2 decimal places

    //Convert int to char then move into struct
    //Using itoa
    itoa(whole_num, buffer);
    Temperature_Array[Sample_Number].Upper_Bit[0] = buffer[0];
    Temperature_Array[Sample_Number].Upper_Bit[1] = buffer[1];

    itoa(frac_num, buffer);
    Temperature_Array[Sample_Number].Lower_Bit = buffer[0];
    //Move everything into the struct

    //Convert to the kelvin equivalent
    float Kelvin_Value = Temperature_Array[Sample_Number].Celsius_Float + 273.15;

    //whole_num = (int) Kelvin_Value;
   // itoa(whole_num, buffer);
    itoa((int)Kelvin_Value, Temperature_Array[Sample_Number].Kelvin) ;
    //snprintf(Temperature_Array[Sample_Number].Kelvin, 4, "%d", Kelvin_Value);
    //End of the processing of the value.


    Sample_Number = Sample_Number + 1;
    if(Sample_Number > Samples){
        Sample_Number = 0;
        //TODO add average here.... DONE??
        int i;
        Rolling_Average = 0;
        for (i = 0; i <= Samples; i++){
        Rolling_Average = Temperature_Array[i].Celsius_Float + Rolling_Average;
        }
        Rolling_Average = Rolling_Average / Samples;
        int whole_num = (int) Rolling_Average;
        int frac_num = (int) ((Rolling_Average - whole_num) * 100); // multiply by 100 to get 2 decimal places
        itoa(whole_num, buffer);
        Rolling_Average_ASCII[0] = buffer[0];
        Rolling_Average_ASCII[1] = buffer[1];
        itoa(frac_num, buffer);
        Rolling_Average_ASCII[2] = buffer[0];

        //TODO create snprintf like below with average.
        snprintf(Temperature_Write_Out, 100, "%c%c.%c\n%c%c%c\n%c%c.%c\n", Temperature_Array[Sample_Number].Upper_Bit[0], Temperature_Array[Sample_Number].Upper_Bit[1], Temperature_Array[Sample_Number].Lower_Bit, Temperature_Array[Sample_Number].Kelvin[0], Temperature_Array[Sample_Number].Kelvin[1], Temperature_Array[Sample_Number].Kelvin[2], Rolling_Average_ASCII[0], Rolling_Average_ASCII[1], Rolling_Average_ASCII[2]);

    }
    //Create string of values if samples has not reached the threshold without the average
    snprintf(Temperature_Write_Out, 100, "%c%c.%c\n%c%c%c\n\n", Temperature_Array[Sample_Number].Upper_Bit[0], Temperature_Array[Sample_Number].Upper_Bit[1], Temperature_Array[Sample_Number].Lower_Bit, Temperature_Array[Sample_Number].Kelvin[0], Temperature_Array[Sample_Number].Kelvin[1], Temperature_Array[Sample_Number].Kelvin[2]);
    Send_I2C_Message(0x048, Temperature_Write_Out_ptr);
}

float Convert_to_Celsius (float V0){
    float Celsius = 0;
    Celsius = -1481.6 + sqrt(2.1962e6 + ((1.8639 - V0) / (3.88e-6)));
    return(Celsius);
}



#pragma vector = EUSCI_A1_VECTOR
__interrupt void ISR_EUSCI_A1(void){
    if(UART_Position_Counter == 40){
        UCA1IE &= ~UCTXCPTIE;
    }
    else{
        UART_Position_Counter++;
        UCA1TXBUF = UART_Message_Global[UART_Position_Counter];

    }
    UCA1IFG &= ~UCTXCPTIFG;
}


/*
Interrupt Service Routines

*/

//I2C Transmit intterupt vector
//Used to send data out the master



//Keypad interrupt vector
#pragma vector = PORT1_VECTOR
__interrupt void ISR_Keypad_Pressed(void)
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


//Analog to Digital interrupt triggered by Timer A1
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Sample_Timer(void) {


    float voltage = 0;
    float temperature = 0;
    float average = 0;


	//Clear timer interrupt flag
	TB0CCTL0 &= ~CCIFG;	

int window_index = 0;     // index of the current sample in the window
int window_full = 0;      // flag indicating whether the window is full
int samples[10];          // array to store the last n samples

    // read LM19 sensor and convert to temperature
    ADCCTL0 |= ADCENC | ADCSC;
    while (ADCCTL1 & ADCBUSY);
    Raw_Temp = ADCMEM0;
    Fresh_Data = 1;

/* This doesn't work...See temperature.c for updated code
//Below is the conversion of the values brought in
    voltage = (float)Raw_Temp; //* 3.3 / 1024.0;
    temperature = -1481.96+ sqrt((2.1962*pow(10,6)+(1.8639-(voltage)/(3.8*pow(10,-6))))); // Calculate temperature

	
	 // update circular buffer and averaging calculation
    samples[window_index] = temperature;
    if (valid_samples < window_size) {
        sum += temperature;
        valid_samples++;
    } else {
        sum = sum + temperature - samples[(window_index + 1) % window_size];
    }
    window_index = (window_index + 1) % window_size;
    window_full = (window_full || (window_index == 0));


    // calculate and display moving average temperature
    if (valid_samples == window_size) {
        average = (float)sum / window_size;
        //printf("Average temperature: %.1f C\n", average);
    }
	*/
//End of the math section.
}

