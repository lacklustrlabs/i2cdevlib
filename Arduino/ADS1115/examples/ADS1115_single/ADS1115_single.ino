// I2C device class (I2Cdev) demonstration Arduino sketch for ADS1115 class
// Example of reading two differential inputs of the ADS1115 and showing the value in mV
// 2016-03-22 by Eadf (https://github.com/eadf)
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2016-03-22 - initial release

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

I2Cdev i2cdev(Wire);
ADS1115<TwoWire> adc0(i2cdev);

// Wire ADS1115 ALERT/RDY pin to Arduino pin 2
const int alertReadyPin = 2;

void setup() {    
    SERIAL_OUTPUT.begin(115200); // initialize serial communication 
    i2cdev.begin(); // join I2C bus
    
    SERIAL_OUTPUT.println(F("Testing device connections..."));
    SERIAL_OUTPUT.println(adc0.testConnection() ? F("ADS1115 connection successful") : F("ADS1115 connection failed"));
    
    adc0.initialize(); // initialize ADS1115 16 bit A/D chip

    // We're going to do single shot sampling
    adc0.setMode(ADS1115_MODE_SINGLESHOT);
    
    // Slow things down so that we can see that the "poll for conversion" code works
    adc0.setRate(ADS1115_RATE_8);
      
    // Set the gain (PGA) +/- 6.144v
    // Note that any analog input must be higher than –0.3V and less than VDD +0.3
    adc0.setGain(ADS1115_PGA_6P144);
    // ALERT/RDY pin will indicate when conversion is ready
    
    pinMode(alertReadyPin,INPUT_PULLUP);
    adc0.setConversionReadyPinMode();

    // To get output from this method, you'll need to turn on the 
    //#define ADS1115_SERIAL_DEBUG // in the ADS1115.h file or right before you include it
#ifdef ADS1115_SERIAL_DEBUG
    adc0.showConfigRegister();
    SERIAL_OUTPUT.print(F("HighThreshold=")); SERIAL_OUTPUT.println(adc0.getHighThreshold(),BIN);
    SERIAL_OUTPUT.print(F("LowThreshold=")); SERIAL_OUTPUT.println(adc0.getLowThreshold(),BIN);
    SERIAL_OUTPUT.println();
#endif
}

/** Poll the assigned pin for conversion status 
 */
void pollAlertReadyPin() {
  for (uint32_t i = 0; i<100000; i++)
    if (!digitalRead(alertReadyPin)) return;
  SERIAL_OUTPUT.println(F("Failed to wait for AlertReadyPin, it's stuck high!"));
}

void loop() {
       
    // The below method sets the mux and gets a reading.
    adc0.setMultiplexer(ADS1115_MUX_P0_NG);
    adc0.triggerConversion();
    pollAlertReadyPin();
    SERIAL_OUTPUT.print(F("A0: ")); SERIAL_OUTPUT.print(adc0.getMilliVolts(false)); SERIAL_OUTPUT.print(F("mV\t"));
    
    adc0.setMultiplexer(ADS1115_MUX_P1_NG);
    adc0.triggerConversion();
    pollAlertReadyPin();
    SERIAL_OUTPUT.print(F("A1: ")); SERIAL_OUTPUT.print(adc0.getMilliVolts(false)); SERIAL_OUTPUT.print(F("mV\t"));
    
    adc0.setMultiplexer(ADS1115_MUX_P2_NG);
    adc0.triggerConversion();
    pollAlertReadyPin();
    SERIAL_OUTPUT.print(F("A2: ")); SERIAL_OUTPUT.print(adc0.getMilliVolts(false)); SERIAL_OUTPUT.print(F("mV\t"));
    
    adc0.setMultiplexer(ADS1115_MUX_P3_NG);
    // Do conversion polling via I2C on this last reading: 
    SERIAL_OUTPUT.print(F("A3: ")); SERIAL_OUTPUT.print(adc0.getMilliVolts(true)); SERIAL_OUTPUT.print(F("mV\t"));

    SERIAL_OUTPUT.print(F("Alert Pin is ")); 
    SERIAL_OUTPUT.println(digitalRead(alertReadyPin)?F("High"):F("Low"));
    delay(100);
}