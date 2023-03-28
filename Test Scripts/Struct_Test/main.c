#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <string.h>

//int Create_Temperature_array(int);
void Process_Temperature_Data(void);
float Convert_to_Celsius(float);

int Samples = 0;
int  New_Temp_Value;
int Sample_Number = 0;
struct Temperature {
    float Celsius_Float;
    char Upper_Bit[2];
    char Lower_Bit;
    char Kelvin[4];
    int Raw_Value;
};

struct Temperature Temperature_Array[5];

//Create a delay loop
void delay(int number_of_seconds){
    int milli_seconds = 1000 * number_of_seconds;
    clock_t start_time = clock();
    while(clock() < start_time + milli_seconds){}
}


int main() {
    char Mode;
    //Get input here for the "New_Temp_Array" then loop and do it again
    while(Mode != 'q'){
        printf("\n\rSelect mode:\n \r \t C: Clear Number of samples to collect \n \r \t N: Enter Number of samples to collect \n \r \t E: Enter Sample Value as Volts^-8 \n \r \t q: Quit \n\r\t\t\t");
        scanf("%c", &Mode);
        switch (Mode) {
            case 'N' | 'n':
                printf("\n\r");
                printf("Enter Number of Samples to collect: ");
                scanf("%d", &Samples);
                printf("\n\r");

                break;
            case 'E' | 'e':
                printf("\n\r");
                printf("Enter Sample Value in Milli-Volts: ");
                scanf("%d", &New_Temp_Value);
                printf("\n\r");
                printf("The immediate Input was: %d", New_Temp_Value);
                printf("\n\r###################################################################################################################");
                Process_Temperature_Data();
                break;
            case 'Q' | 'q':
                printf("\n\r");
                printf("Quitting....");
                delay(1);
                printf("\n\r");
                Sample_Number = 0;
                exit(0);
                 break;
            default:
                printf("Error! Invalid Input! \n\r");
                break;
        }

    }
    return 0;
}






void Process_Temperature_Data(void){
    //Now we have an array of Temperature structs which we can begin to populate??? And then feed these temperature structs into Gary's struct to hold the rolling average
    char buffer[20];
    Temperature_Array[Sample_Number].Raw_Value = New_Temp_Value;
    New_Temp_Value = 0; //Move the new raw value into the struct then clear the new raw value

    printf("\n\r");
    printf("The value recorded was: %d", Temperature_Array[Sample_Number].Raw_Value);
    //Now to convert from mV to degrees C

   Temperature_Array[Sample_Number].Celsius_Float =  Convert_to_Celsius(Temperature_Array[Sample_Number].Raw_Value*10e-8);
    printf("\n\r The processed Celsius Value was: %f",Temperature_Array[Sample_Number].Celsius_Float);
    int whole_num = (int) Temperature_Array[Sample_Number].Celsius_Float;
    int frac_num = (int) ((Temperature_Array[Sample_Number].Celsius_Float - whole_num) * 100); // multiply by 100 to get 2 decimal places

    //Convert int to char then move into struct
    //Using itoa
    itoa(whole_num, buffer,10);
    Temperature_Array[Sample_Number].Upper_Bit[0] = buffer[0];
    Temperature_Array[Sample_Number].Upper_Bit[1] = buffer[1];

    itoa(frac_num, buffer,10);
    Temperature_Array[Sample_Number].Lower_Bit = buffer[0];
    //Move everything into the struct

    printf("\n\r The upper byte is: %c", Temperature_Array[Sample_Number].Upper_Bit[0]);
    printf("\n\r The lower byte is: %c", Temperature_Array[Sample_Number].Lower_Bit);
    printf("\n\r The total value is %c%c.%c.", Temperature_Array[Sample_Number].Upper_Bit[0],Temperature_Array[Sample_Number].Upper_Bit[1], Temperature_Array[Sample_Number].Lower_Bit);

    //Convert to the kelvin equivalent
    float Kelvin_Value = Temperature_Array[Sample_Number].Celsius_Float + 273.15;

    whole_num = (int) Kelvin_Value;
    itoa(whole_num, buffer,10);
    printf("\n\rThe Kelvin value is %s.", buffer);

    snprintf(Temperature_Array[Sample_Number].Kelvin, 4, "%.1f", Temperature_Array[Sample_Number].Celsius_Float + 273.15);

    printf("\n\r Kelvin In Struct %s",Temperature_Array[Sample_Number].Kelvin);
    //End of the processing of the value.


    Sample_Number = Sample_Number + 1;
    if(Sample_Number > Samples){
        Sample_Number = 0;
    }
}

float Convert_to_Celsius (float V0){
    float Celsius = 0;
    printf("\n\r The value V0 passed into Convert_to_Celsius was: %f", V0);
    Celsius = -1481.6 + sqrt(2.1962e6 + ((1.8639 - V0) / (3.88e-6)));
    return(Celsius);
}

