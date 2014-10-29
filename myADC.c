/*
$Id:$
// myADC.c

My ADC library!

Atmega328P 8-bit MCU

*/

#include <avr/io.h>
#include <avr/delay.h>
#include <stdlib.h>

void setup_adc(){
    ADCSRA |= ((1<<ADPS1)|(1<<ADPS0));              // setups up ADC clock prescalar to 16
    ADMUX |= (1<<REFS0);                            // set ref voltage to AVCC
    //ADMUX |= (1<<ADLAR);                          // left align results in ADC registers (10 bits across 2 regs)

    //ADCSRB &= ~(1<<ADTS2);                        // These three cleared should enable free-running mode
    //ADCSRB &= ~(1<<ADTS1);
    //ADCSRB &= ~(1<<ADTS0); 

    //ADCSRA |= (1<<ADATE);                         // ADC Auto Trigger Enable 
    ADCSRA |= (1<<ADEN);                            // ADC Enable

    ADCSRA |= (1<<ADSC);                            // start sampling

}
uint16_t sample_adc_channel(uint8_t channel){
// Returns 10 bit reading (right justified)
    uint16_t adcVal;
    ADMUX &= ~((1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0));    // Clear ADC Mux Bits
    if(channel == 1){
        ADMUX |= (1<<MUX0);                             // setup ADC Channel 1
        ADCSRA |= (1 << ADSC);                          // Start a new conversion, 
        while(ADCSRA & _BV(ADSC));                      // Wait until conversion is complete and ADSC is cleared
        adcVal = ADC;                                   // 10 bit reading
    }
    else if(channel == 2){
        ADMUX |= (1<<MUX1);                             // setup ADC Channel 2
        ADCSRA |= (1 << ADSC);                          // Start a new conversion, 
        while(ADCSRA & _BV(ADSC));                      // Wait until conversion is complete and ADSC is cleared
        adcVal = ADC;                                   // 10 bit reading
    }
    else if(channel == 3){
        ADMUX |= ((1<<MUX1)|(1<<MUX0));                 // setup ADC Channel 3
        ADCSRA |= (1 << ADSC);                          // Start a new conversion, 
        while(ADCSRA & _BV(ADSC));                      // Wait until conversion is complete and ADSC is cleared
        adcVal = ADC;                                   // 10 bit reading
    }
    return adcVal;
}
