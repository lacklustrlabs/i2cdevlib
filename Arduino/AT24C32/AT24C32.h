// I2Cdev library collection - AT24C32 I2C device class header file
// Based on Maxim AT24C32 datasheet, 2008
// 11/13/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
//     2011-11-13 - initial release

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

#ifndef _AT24C32_H_
#define _AT24C32_H_

#include "I2Cdev.h"

#define AT24C32_ADDRESS_0            0x50
#define AT24C32_ADDRESS_1            0x51
#define AT24C32_ADDRESS_2            0x52
#define AT24C32_ADDRESS_3            0x53
#define AT24C32_ADDRESS_4            0x54
#define AT24C32_ADDRESS_5            0x55
#define AT24C32_ADDRESS_6            0x56
#define AT24C32_ADDRESS_7            0x57

#define AT24C32_DEFAULT_ADDRESS      AT24C32_ADDRESS_0


template <typename WIRE>
class AT24C32 {
  AT24C32() = delete;
  AT24C32( const AT24C32& other ) = delete; // non construction-copyable
  AT24C32 & operator=( const AT24C32& ) = delete; // non copyable

public:
  /** Specific address constructor.
   * @param address I2C address
   * @see AT24C32_DEFAULT_ADDRESS
   * @see AT24C32_ADDRESS
   */
  AT24C32(I2CdevT<WIRE, uint16_t>& i2cdev, uint8_t devAddress=AT24C32_DEFAULT_ADDRESS):
    _i2cdev(i2cdev), _devAddr(devAddress) {
  }

  /** Power on and prepare for general usage.
   * This method reads the current 12/24-hour mode setting into the locally stored
   * variable so that it isn't accidentally changed by the setHour*() methods.
   */
  void initialize() {  
  }

  /** Verify the I2C connection.
   * Make sure the device is connected and responds as expected.
   * @return True if connection is valid, false otherwise
   */
  bool testConnection(){
    // read the last byte in the eeprom
    bool rv = true;
    uint16_t address = 0b111111111111;
    uint8_t before = readByte(address);
    
    // do some xor on the previous data and save it
    uint8_t replace = before ^ B10101010;
    writeByte(address,replace, true);

    // verify that the new data is saved correctly
    uint8_t verify = readByte(address);

    // write back the original
    writeByte(address,before, true);
    return replace == verify;
  }
  
  /** Write a byte to the eeprom
   *
   * @param regAddr 12 bit eeprom address
   * @param data the byte to write
   * @param useBuiltinDelay if true: run delay(1)
   * @return True if the write succeded
   */
  bool writeByte(uint16_t regAddr, uint8_t data, bool useBuiltinDelay=true) {
    bool rv = _i2cdev.writeByte(_devAddr, regAddr, data);
    if (useBuiltinDelay)
      delay(1);
    return rv;
  }
  
  /** Read a byte from the eeprom
   *
   * @param regAddr 12 bit eeprom address
   * @return the data
   */
  uint8_t readByte(uint16_t regAddr) {
    uint8_t buffer;
    _i2cdev.readByte(_devAddr, regAddr, &buffer);
    return buffer;
  }
  
  /** Write a sequence of bytes to the eeprom
   *
   * @param regAddr First register address to write to (note: 12 bits)
   *                Clear the last four bits of the address.
   * @param length Number of bytes to write (max 30)
   * @param data Buffer to copy new data from
   * @return Status of operation (true = success)
   */
  bool writeBytes(uint16_t regAddr, uint8_t length, uint8_t *data, bool useBuiltinDelay=true) {
    bool rv = _i2cdev.writeBytes(_devAddr, regAddr, length, data);
    if (useBuiltinDelay)
      delay(1);
    return rv;
  }

  /** Read a sequence of bytes from the eeprom
   *
   * @param regAddr First register address to write to (note: 12 bits).
   * @param length the number of bytes to read (max 30)
   * @param data Buffer to copy new data to
   * @return Number of bytes read (-1 indicates failure)
   */
  int8_t readBytes(uint16_t regAddr, uint8_t length, uint8_t *data) {
    return _i2cdev.readBytes(_devAddr, regAddr, length, data);
  }


private:
  I2CdevT<WIRE, uint16_t>& _i2cdev;
  uint8_t _devAddr;
};

#endif /* _AT24C32_H_ */
