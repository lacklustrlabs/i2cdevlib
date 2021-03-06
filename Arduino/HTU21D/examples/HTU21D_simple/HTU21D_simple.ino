// I2C device class (I2Cdev) demonstration Arduino sketch for HTU21D class
// Example of reading temperature and humidity from the HTU21D sensor
// 2016-03-24 by Eadf (https://github.com/eadf)
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2016-03-24 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2016 Eadf, Jeff Rowberg

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

#include "HTU21D.h"

#define SERIAL_OUTPUT Serial

I2CdevT<TwoWire, uint8_t> i2cdev(Wire);
HTU21D<TwoWire> htu21d(i2cdev);

void setup() {
  SERIAL_OUTPUT.begin(115200);
  i2cdev.begin();  // join I2C bus

  htu21d.initialize();
  SERIAL_OUTPUT.println(F("Testing device connections..."));
  SERIAL_OUTPUT.println(htu21d.testConnection() ? F("HTU21D connection successful") : F("HTU21D connection failed"));
}

void loop() {
  SERIAL_OUTPUT.print(F("Temperature: ")); SERIAL_OUTPUT.print(htu21d.getTemperature());
  SERIAL_OUTPUT.print(F("\t\tHumidity: ")); SERIAL_OUTPUT.println(htu21d.getHumidity());
  delay(400);
}