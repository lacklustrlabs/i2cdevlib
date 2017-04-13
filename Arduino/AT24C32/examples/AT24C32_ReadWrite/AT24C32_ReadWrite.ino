// I2C device class (I2Cdev) demonstration Arduino sketch for AT24C32 class
// 11/12/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
//     2011-11-12 - initial release

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

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"

// I2Cdev and AT24C32 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "AT24C32.h"

// default I2C address is 0x50
I2CdevT<TwoWire,uint16_t> i2cdev(Wire);
AT24C32<TwoWire> at24c32(i2cdev);



void test_bytes(uint16_t regAddr) {
    uint8_t data[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','x','y','z','A','B','C','D','E'};
    Serial.print("\nTesting AT24C32::readBytes and AT24C32::writeBytes @0x");
    Serial.println(regAddr, HEX);
    
    Serial.print("Writing: ");
    for (int i=0; i<sizeof(data); i++) {
      Serial.print((char)data[i]);
    }
    
    at24c32.writeBytes(regAddr, sizeof(data), data);

    for (int i=0; i<sizeof(data); i++) {
      data[i] = 0;
    }
   
    at24c32.readBytes(regAddr, sizeof(data), data);
    Serial.print("\nVerify:  ");
    for (int i=0; i<sizeof(data); i++) {
      Serial.print((char)data[i]);
    }
    Serial.println();  
}

void test_byte() {
    uint16_t regAddr = 0b111111111001;
    Serial.print("\nTesting AT24C32::readByte and AT24C32::writeByte @0x");
    Serial.println(regAddr, HEX);
    
    char data = 'X';
    Serial.print("Writing: ");
    Serial.println(data);

    at24c32.writeByte(regAddr, data);

    Serial.print("Verify:  ");
    data = at24c32.readByte(regAddr);
    Serial.println(data);
}

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    i2cdev.begin();

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    at24c32.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(at24c32.testConnection() ? "AT24C32 connection successful" : "AT24C32 connection failed");

    test_byte();
    for (uint16_t i=0;i<2;i++){
      // When writing 30 bytes at once the low 4 bits of the address must be cleared.
      test_bytes(i<<4);   
    }
}

void loop() {
    delay(1000);
}
