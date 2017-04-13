// I2C device class (I2Cdev) demonstration Arduino sketch for ADS1115 class
// Example of reading two differential inputs of the ADS1115 and showing the value in mV
// 06 May 2013 by Frederick Farzanegan (frederick1@farzanegan.org)
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2013-05-13 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/
#define SERIAL_OUTPUT Serial
#define ADS1115_SERIAL_DEBUG
#define I2CDEV_DEBUG_OUTPUT SERIAL_OUTPUT

#include "ADS1115.h"
#include <Wire.h>

I2Cdev i2cdev(Wire);
ADS1115<TwoWire> adc0(i2cdev, ADS1115_DEFAULT_ADDRESS);

void setup() {
    SERIAL_OUTPUT.begin(115200); // initialize serial communication 
    i2cdev.begin(); // join I2C bus                
    SERIAL_OUTPUT.println(F("Initializing I2C devices...")); 
    adc0.initialize(); // initialize ADS1115 16 bit A/D chip
    
    SERIAL_OUTPUT.println(F("Testing device connections..."));
    SERIAL_OUTPUT.println(adc0.testConnection() ? F("ADS1115 connection successful") : F("ADS1115 connection failed"));
      
    // To get output from this method, you'll need to turn on the 
    //#define ADS1115_SERIAL_DEBUG // in the ADS1115.h file or right before you include it
    adc0.showConfigRegister();

    SERIAL_OUTPUT.println(F("Don't leave the input pins floating\n."));
    // We're going to do continuous sampling
    adc0.setMode(ADS1115_MODE_CONTINUOUS);
}

void loop() {

    // Sensor is on P0/N1 (pins 4/5)
    // Set the gain (PGA) +/- 1.024v
    adc0.setGain(ADS1115_PGA_1P024);

    // Get the number of counts of the accumulator   
    // The below method sets the mux and gets a reading.
    int sensorOneCounts=adc0.getConversionP0N1();  // counts up to 16-bits  

    // To turn the counts into a voltage, we can use
    SERIAL_OUTPUT.print(F("Sensor 1 (A0-A1):"));
    SERIAL_OUTPUT.print(sensorOneCounts*adc0.getMvPerCount());
    SERIAL_OUTPUT.println(F(" milli volt (max ± 1.024mV with current settings)"));
          
    // 2nd sensor is on P2/N3 (pins 6/7)
    // Set the gain (PGA) +/- 0.256v
    adc0.setGain(ADS1115_PGA_0P256);

    // Manually set the MUX  // could have used the getConversionP* above
    adc0.setMultiplexer(ADS1115_MUX_P2_N3); 
    
    SERIAL_OUTPUT.print(F("Sensor 2 (A2-A3):"));
    SERIAL_OUTPUT.print(adc0.getMilliVolts());  // Convenience method to calculate voltage
    SERIAL_OUTPUT.println(F(" milli volt (max ± 1.024mV with current settings)"));
    SERIAL_OUTPUT.println();
    
    delay(800);
}