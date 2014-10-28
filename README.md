tmp36_temperature_sensor
============
http://elegantcircuits.com/2014/10/27/using-the-tmp36-temperature-sensor-and-atmega328p-to-measure-temperature-and-output-to-a-display/

A digital thermometer utilizing a TMP36 temperature sensor and an SSD1306 OLED module

Runs on Atmega328P AVR Microcontroller at 8MHz (CLKDIV 8 fuse disabled for quicker refresh)

CLK = 8MHZ
avrdude -p atmega328p -P /dev/ttyUSB0 -c avrisp -b 19200 -B 1 -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m 

Enable CKDIV8 fuse
avrdude -p atmega328p -P /dev/ttyUSB0 -c avrisp -b 19200 -B 1 -U lfuse:w:0x62:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m 
