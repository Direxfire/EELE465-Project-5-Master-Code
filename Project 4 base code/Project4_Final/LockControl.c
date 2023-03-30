#include<msp430.h>
/*
This file is to create the functions that correspond to the locked vs. unlocked status of the system
*/


//Time Out Function
//This function is called when the wrong passcode is entered. This may be legacy code at this point as the master has the ability to write strings out now
extern void Send_I2C_Message(int, char*);
extern char Decode_Input(int);


//Function prototypes

void Time_Out(void);
int Passcode_Check(void);

extern char Locked_Code;
extern char* Locked_Code_ptr;
extern char Unlocked_ASCII;
extern char* Unlocked_ASCII_ptr;


extern int Passcode_Inputs[5];       //Three digit passcode, the last value holds the locked state
extern int Input_Counter;
extern int Temp_In[2];
extern int Status;
extern char Input_Arr[3];
extern int New_Input;
extern int Unlocked_Input;

char Passocde[3] = {'1','2','3'};

int Locked_Status(void){
    signed int i;
    Locked_Code_ptr = "#";
    //Send_I2C_Message(0x68, Locked_Code_ptr);
    Send_I2C_Message(0x48, Locked_Code_ptr);
    Send_I2C_Message(0x58, Locked_Code_ptr);
    Passcode_Inputs[4] = 0;
    unsigned int Passcode_Inc = 0;
    //Below is the waiting for user input to unlock the device
    while(Passcode_Inputs[4] == 0){
        //Wait for inputs on P1
        P1IE |= 0xF;
        while(Passcode_Inputs[3] == 0){
            //Do nothing
            //Should really be put into low-power mode but that's alot of work

        }
        P1IE &= ~0xF;
        for(i = 0; i < 30000; i++){
           //Short delay to wait for finger to be removed
        }
        P6OUT &= ~BIT6;
        P1IE |= 0xF;
        Passcode_Inputs[3] = 0;
        Locked_Code = Decode_Input(Passcode_Inputs[Passcode_Inc]);
        Input_Arr[Passcode_Inc] = Locked_Code;
        Send_I2C_Message(0x048, Locked_Code_ptr);
        Passcode_Inc++;

    }

    //End waiting to unlock device. 

    //Begin check if password is correct
    Status = Passcode_Check();
    if (Status == 1)
    {
    Time_Out();
    // Send I2C String here

    int j;
    for (j = 0; j < 30000; j++)
    {
    }
    for (j = 0; j < 30000; j++)
    {
    }

    for (j = 0; j < 30000; j++)
    {
    }
    for (j = 0; j < 30000; j++)
    {
    }

    Passcode_Inc = 0;

    for (j = 0; j < 6; j++)
    {
        Passcode_Inputs[j] = 0;
    }
    }
    else if (Status == 0)
    {
        return(0);
    }
    return(1);
}

int Unlocked_Status(void)
{
        unsigned int i;
        P1IE |= 0xF;
        // Device unlocked. Send "\a" which is the unlocked/lock status change symbol to both slaves
        Unlocked_ASCII_ptr = "\a";
        Send_I2C_Message(0x058, Unlocked_ASCII_ptr);
        Send_I2C_Message(0x048, Unlocked_ASCII_ptr);
        while (Status == 0)
        {
        if (New_Input == 1)
        {
            Unlocked_ASCII = Decode_Input(Unlocked_Input);
            for (i = 0; i < 55000; i++)
            {
            }
            P6OUT &= ~BIT6;
            P6OUT &= ~BIT5;
            P1IE |= 0xF;
            New_Input = 0;
            // Next write the value out the I2C
            // Testing writing out to 0x058 address the Unlocked_ASCII Character
            Send_I2C_Message(0x048, Unlocked_ASCII_ptr);
            Send_I2C_Message(0x058, Unlocked_ASCII_ptr);
            if (*Unlocked_ASCII_ptr == '#')
            {
                Status = 1;
                for (i = 0; i < 6; i++)
                {
                    Passcode_Inputs[i] = 0;
                }
            }
        }
        }
        return(0);
}

int Passcode_Check(void)
{
    int i;
    for (i = 0; i <= 2; i++)
    {
        if (Passocde[i] != Input_Arr[i])
        {
            return(1);
        }
        else{}
    }
    for(i = 0; i < 25000; i++){}
    return(0);
}
void Time_Out(void){
    char Locked_Out_Message[] = "Error! Wrong Password!";
    char* Locked_Out_Message_ptr = Locked_Out_Message;  //Create the pointer to the locked out message for passing into the Send_I2C_Message function located in I2C_Communication.c
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
    Send_I2C_Message(0x58, Locked_Out_Message_ptr);
}

