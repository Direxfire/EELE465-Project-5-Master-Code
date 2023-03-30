#include <msp430.h>

/*
 * AnalogToDigital.c
 *
 *  Created on: Mar 29, 2023
 *      Author: Drew
 */

#define LM19_PIN    BIT0
#define third_second_timer_period 33333


void Setup_A2D(void){
    /*
     // configure ADC
	ADCCTL0 &= ~ADCENC;                                     // Disable ADC
    ADCCTL0 |= ADCSHT_2 | ADCON;                            // Sampling time and ADC on
    ADCCTL1 |= ADCSHP | ADCSSEL_2 | ADCDIV_7 | ADCSHP;      // Signal source
    ADCCTL2 |= ADCRES;                                      //sets the ADC resolution to 10 bits.
	ADCIE |= ADCIE0;                                        // Enable interrupt
    //ADCMCTL0 |= LM19_PIN | ADCINCH_0;                       // Vref+ and input channel
    ADCMCTL0 |= LM19_PIN | ADCINCH_8;                       //Vref+ and input channel
	ADCCTL0 |= ADCENC;                                      // Enable ADC
    */

   ADCCTL0 &= ~ADCSHT;                                     //Clear ADCSHT from def. of ADCHST =01
   ADCCTL0 |= ADCSHT_2;                                    //Conversion cycles = 16 <- Might be a bit over kill but eh
   ADCCTL0 |= ADCON;                                        //Turn ADC ON <-  This is fine as we won't start it until the timer triggers
   
   ADCCTL1 |= ADCSSEL_2;                                    //ADC Clock Source = SMCLK
   ADCCTL1 |= ADCSHP;                                       //Sample Signal Source = Sampling timer

   ADCCTL2 &= ~ADCRES;                                      //Clear ADCRES from def. of ADCRES = 01
   ADCCTL2 |= ADCRES_2;                                     //Resolution = 8 bit (+/- .1 c this is plenty)

   ADCMCTL0 |= ADCINCH_8;                                   //Set ADC input channel to 8, P5.0
}


