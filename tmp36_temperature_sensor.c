/***************************************************************************
Title:    TMP36 Temperature Sensor
Author:   Elegantcircuits.com
File:     $Id: tmp36_temperature_sensor 
Software: AVR-GCC 3.3
Hardware: Atmega328P AVR 

Description:
    This example shows how to drive OLED graphic display with the readings taken from an accelerometer

HW Description:

SSD1306 to Atmega I2C Connections
VCC -> 3.3V
SCL -> (Pin 28)
SDA -> (Pin 27)

TMP36 to Atmega ADC Connections
VCC -> 3.3V
VOUT-> (Pin 24)
GND -> GND

*****************************************************************************/
#include <avr/io.h>
#include <avr/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "i2cmaster.h"
#include "myADC.h"                      // my ADC library
#include "myOLED.h"
#include "myFont.h"

#define NUM_TASKS 8
#define DEVSSD1306  0x78                // device address of SSD1306 OLED, uses 8-bit address (0x3c)!!! 

static void init_devices(void);
static void timer0_init(void);
static void init_io(void);


char task_bits = 0;  /* lsb is hi priority task */
volatile char tick_flag = 0;    /* if non-zero, a tick has elapsed */
unsigned int task_timers[NUM_TASKS]={0,0,0,0,0,0,0,0};                  /* init the timers to 0 on startup */
static const char bit_mask[] PROGMEM={1,2,4,8,16,32,64,128};            /* value -> bit mask xlate table */

int main(void)
{
    static unsigned char ret;
    static char valueIn[4];

    init_io();
    init_devices();
    i2c_init();                                 // Init I2C interface

    ret = i2c_start(DEVSSD1306+I2C_WRITE);      // set device address and write mode

    if ( ret ) {
        /* failed to issue start condition, possibly no device found */
        i2c_stop();
        PORTB= (1<<PB1);                        // Turn on LED connected to PB1 as I2C alarm 
    }
    else {
        /* issuing start condition ok, device accessible */
        setup_oled_i2c();
        setup_adc();
    }

    for(;;){                                                // Event Loop
        if(tick_flag){
            tick_flag = 0;
            sleep_disable();

            clearBuffer(buffer);

            lcd_draw_string(0, 0, "ADXL335 Readings: " , buffer);

            // get accelerometer values
            int x_reading = sample_adc_channel(1);
            itoa(x_reading, valueIn, 10);
            lcd_draw_string(0,2, "X: " , buffer);
            lcd_draw_string(13, 2, valueIn, buffer);

            int y_reading = sample_adc_channel(2);
            itoa(y_reading, valueIn, 10);
            lcd_draw_string(0,3, "Y: " , buffer);
            lcd_draw_string(13, 3, valueIn, buffer);


            int z_reading = sample_adc_channel(3);
            itoa(z_reading, valueIn, 10);
            lcd_draw_string(0,4, "Z: " , buffer);
            lcd_draw_string(13, 4, valueIn, buffer);

            //PORTB=0x00;
            drawBuffer(0, 0, buffer);
            // PORTB= (1<<PB1);                            // activate LED to show sampling */

            //if(z_reading < 500){
            //    PORTB ^= _BV(PB1);
            //}

            set_sleep_mode(SLEEP_MODE_IDLE);
            sleep_enable();
            sleep_mode();
        }
    }
}


//void drawPixel(int16_t x, int16_t y){//, int16_t color){
////void drawPixel(int16_t x, int16_t y, int16_t color){
//    buffer[x+(y/8)*128] |= (1 << (y & 7));
//}
//
//void clearBuffer(uint8_t *buff){
//    memset(buff, 0, ((128 * 64)/8));
//}
//
//void drawBuffer(uint8_t column_address, uint8_t page_address, uint8_t *buff){
//    i2c_start(DEVSSD1306+I2C_WRITE); 
//    i2c_write(0x21);        // column address
//    i2c_write(0);           // column start address (0 = reset)
//    i2c_write(127);         // column end addres (127 = reset)
//
//    i2c_write(0x22);        // page address
//    i2c_write(0);           // page start address (0 = reset);
//    i2c_write(7);           // page end address
//
//    uint8_t twbrbackup = TWBR;
//    TWBR = 12;
//
//    for (uint16_t i=0; i<((128*64)/8); i++){
//        unsigned char ret = i2c_start(DEVSSD1306+I2C_WRITE);   // set device address and write mode
//        if ( ret ) {
//            /* failed to issue start condition, possibly no device found */
//            i2c_stop();
//            PORTB=0xff;                            // activate all 8 LED to show error */
//        }
//        else {
//            i2c_write(0x40);                        // set display RAM display start line register
//            for (uint8_t x=0; x<16; x++){
//                i2c_write(buff[i]);
//                i++;
//            }
//            i--;
//            i2c_stop();
//            TWBR = twbrbackup;
//        }
//    }
//}
//
//void lcd_draw_char(unsigned char column, unsigned char page, unsigned char letter, uint8_t *buff){
//    uint8_t ascii_offset = 32;
//    for(uint8_t i=0; i<5; i++){
//        buff[i+((page*128)+column)] = Ascii_1[letter-ascii_offset][i];
//    }
//}
//void lcd_draw_string(uint8_t column, uint8_t page, char *string, uint8_t *buff){
//    uint8_t i = 0;
//    while(string[0] != 0){
//        //column += 6; // 6 pixels wide
//        //if (column + 6 >= 128) {
//        //  column = 0;    // ran out of this line
//        //  page++;
//        //}
//        //if (page >= 8)
//        //  return;        // ran out of space :(
//        lcd_draw_char(column+(5*i), page, (string[0]), buff);
//        string++;
//        i++;
//    }
//}


//call this routine to initialize all peripherals
void init_devices(void){
    //stop errant interrupts until set up
    cli(); //disable all interrupts

    timer0_init();
    
    MCUCR = 0x00;
    EICRA = 0x00; //extended ext ints
    EIMSK = 0x00;
    
    TIMSK0 = 0x02; //timer 0 interrupt sources
    
    PRR = 0x00; //power controller
    sei(); //re-enable interrupts
    //all peripherals are now initialized
}
//TIMER0 initialize - prescale:1024
// WGM: CTC
// desired value: 10mSec
// actual value: 10.048mSec (-0.5%)
void timer0_init(void){
    TCCR0B = 0x00; //stop
    TCNT0 = 0x00; //set count
    TCCR0A = 0x02; //CTC mode
    OCR0A = 0x9C; 
    TCCR0B = 0x05; //start timer
}

void init_io(){
    DDRB  = 0xff;                              // use all pins on port B for output 
    PORTB = 0x00;                              // (LED's low & off)

    DDRB &= ~(1<<PB0);                          // Set PB0 as input
    PORTB |= (1<<PB0);                          // Pullup resistor on PB0
}

ISR(TIMER0_COMPA_vect){
 //TIMER0 has overflowed
    tick_flag = 1;
}

