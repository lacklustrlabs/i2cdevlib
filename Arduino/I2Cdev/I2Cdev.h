// I2Cdev library collection - Main I2C device class header file
// Abstracts bit and byte I2C R/W functions into a convenient class
// 2013-06-05 by Jeff Rowberg <jeff@rowberg.net>
//
// Changelog:
//      2015-10-30 - simondlevy : support i2c_t3 for Teensy3.1
//      2013-05-06 - add Francesco Ferrara's Fastwire v0.24 implementation with small modifications
//      2013-05-05 - fix issue with writing bit values to words (Sasquatch/Farzanegan)
//      2012-06-09 - fix major issue with reading > 32 bytes at a time with Arduino Wire
//                 - add compiler warnings when using outdated or IDE or limited I2Cdev implementation
//      2011-11-01 - fix write*Bits mask calculation (thanks sasquatch @ Arduino forums)
//      2011-10-03 - added automatic Arduino version detection for ease of use
//      2011-10-02 - added Gene Knight's NBWire TwoWire class implementation with small modifications
//      2011-08-31 - added support for Arduino 1.0 Wire library (methods are different from 0.x)
//      2011-08-03 - added optional timeout parameter to read* methods to easily change from default
//      2011-08-02 - added support for 16-bit registers
//                 - fixed incorrect Doxygen comments on some methods
//                 - added timeout value for read operations (thanks mem @ Arduino forums)
//      2011-07-30 - changed read/write function structures to return success or byte counts
//                 - made all methods static for multi-device memory savings
//      2011-07-28 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2013 Jeff Rowberg

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

#ifndef _I2CDEV_H_
#define _I2CDEV_H_

#ifndef I2CDEV_DEBUG_OUTPUT
#define I2CDEV_DEBUG_OUTPUT Serial
#endif

// -----------------------------------------------------------------------------
// Arduino-style "Serial.print" debug constant (uncomment to enable)
// -----------------------------------------------------------------------------
//#define I2CDEV_SERIAL_DEBUG

#ifdef ARDUINO
#if ARDUINO < 100
#include "WProgram.h"
#else
#include "Arduino.h"
#endif
#endif

#include <Wire.h>

// 1000ms default read _timeout (modify with "I2Cdev::_timeout = [ms];")
#ifndef I2CDEV_DEFAULT_READ_TIMEOUT
#define I2CDEV_DEFAULT_READ_TIMEOUT     1000
#endif

template<typename WIRE, typename RegAddr>
class I2CdevT {
public:
  I2CdevT(WIRE& wire, uint16_t readTimeout = I2CDEV_DEFAULT_READ_TIMEOUT) :
      _wire(wire), _timeout(readTimeout) {
  }
  I2CdevT() = delete;
  I2CdevT(const I2CdevT<WIRE, RegAddr>& other) = delete; // non construction-copyable
  I2CdevT& operator=(const I2CdevT<WIRE, RegAddr>&) = delete; // non copyable

  void begin();

  int8_t readBit(uint8_t devAddr, RegAddr regAddr, uint8_t bitNum, uint8_t *data);
  int8_t readBitW(uint8_t devAddr, RegAddr regAddr, uint8_t bitNum, uint16_t *data);
  int8_t readBits(uint8_t devAddr, RegAddr regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
  int8_t readBitsW(uint8_t devAddr, RegAddr regAddr, uint8_t bitStart, uint8_t length, uint16_t *data);
  int8_t readByte(uint8_t devAddr, RegAddr regAddr, uint8_t *data);
  int8_t readWord(uint8_t devAddr, RegAddr regAddr, uint16_t *data);
  int8_t readBytes(uint8_t devAddr, RegAddr regAddr, uint8_t length, uint8_t *data);
  int8_t readWords(uint8_t devAddr, RegAddr regAddr, uint8_t length, uint16_t *data);

  bool writeBit(uint8_t devAddr, RegAddr regAddr, uint8_t bitNum, uint8_t data);
  bool writeBitW(uint8_t devAddr, RegAddr regAddr, uint8_t bitNum, uint16_t data);
  bool writeBits(uint8_t devAddr, RegAddr regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
  bool writeBitsW(uint8_t devAddr, RegAddr regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
  bool writeByte(uint8_t devAddr, RegAddr regAddr, uint8_t data);
  bool writeWord(uint8_t devAddr, RegAddr regAddr, uint16_t data);
  bool writeBytes(uint8_t devAddr, RegAddr regAddr, uint8_t length, uint8_t *data);
  bool writeWords(uint8_t devAddr, RegAddr regAddr, uint8_t length, uint16_t *data);

protected:
  WIRE& _wire;
  uint16_t _timeout;
};

template<typename WIRE, typename RegAddr>
void I2CdevT<WIRE, RegAddr>::begin() {
  _wire.begin();
}

/** Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param _timeout Optional read _timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
template<typename WIRE, typename RegAddr>
int8_t I2CdevT<WIRE, RegAddr>::readBit(uint8_t devAddr, RegAddr regAddr, uint8_t bitNum, uint8_t *data) {
  uint8_t b;
  uint8_t count = readByte(devAddr, regAddr, &b);
  *data = b & (1 << bitNum);
  return count;
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param _timeout Optional read _timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
template<typename WIRE, typename RegAddr>
int8_t I2CdevT<WIRE, RegAddr>::readBits(uint8_t devAddr, RegAddr regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
  // 01101001 read byte
  // 76543210 bit numbers
  //    xxx   args: bitStart=4, length=3
  //    010   masked
  //   -> 010 shifted
  uint8_t count, b;
  if ((count = readByte(devAddr, regAddr, &b)) != 0) {
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    b &= mask;
    b >>= (bitStart - length + 1);
    *data = b;
  }
  return count;
}

/** Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @param _timeout Optional read _timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
template<typename WIRE, typename RegAddr>
int8_t I2CdevT<WIRE, RegAddr>::readByte(uint8_t devAddr, RegAddr regAddr, uint8_t *data) {
  return readBytes(devAddr, regAddr, 1, data);
}

/** Read single word from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for word value read from device
 * @param _timeout Optional read _timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
//template<>
//int8_t I2CdevT<TwoWire, uint8_t>::readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data) {
template<typename WIRE, typename RegAddr>
int8_t I2CdevT<WIRE, RegAddr>::readWord(uint8_t devAddr, RegAddr regAddr, uint16_t *data) {
  return readWords(devAddr, regAddr, 1, data);
}

/** Read a single bit from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-15)
 * @param data Container for single bit value
 * @param _timeout Optional read _timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
template<typename WIRE, typename RegAddr>
int8_t I2CdevT<WIRE, RegAddr>::readBitW(uint8_t devAddr, RegAddr regAddr, uint8_t bitNum, uint16_t *data) {
  uint16_t b;
  uint8_t count = readWord(devAddr, regAddr, &b);
  *data = b & (1 << bitNum);
  return count;
}

/** Read multiple bits from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-15)
 * @param length Number of bits to read (not more than 16)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param _timeout Optional read _timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (1 = success, 0 = failure, -1 = _timeout)
 */
template<typename WIRE, typename RegAddr>
int8_t I2CdevT<WIRE, RegAddr>::readBitsW(uint8_t devAddr, RegAddr regAddr, uint8_t bitStart, uint8_t length, uint16_t *data) {
  // 1101011001101001 read byte
  // fedcba9876543210 bit numbers
  //    xxx           args: bitStart=12, length=3
  //    010           masked
  //           -> 010 shifted
  uint8_t count;
  uint16_t w;
  if ((count = readWord(devAddr, regAddr, &w)) != 0) {
    uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    w &= mask;
    w >>= (bitStart - length + 1);
    *data = w;
  }
  return count;
}

/** write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
template<typename WIRE, typename RegAddr>
bool I2CdevT<WIRE, RegAddr>::writeBit(uint8_t devAddr, RegAddr regAddr, uint8_t bitNum, uint8_t data) {
  uint8_t b;
  readByte(devAddr, regAddr, &b);
  b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
  return writeByte(devAddr, regAddr, b);
}

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
template<typename WIRE, typename RegAddr>
bool I2CdevT<WIRE, RegAddr>::writeBits(uint8_t devAddr, RegAddr regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
  //      010 value to write
  // 76543210 bit numbers
  //    xxx   args: bitStart=4, length=3
  // 00011100 mask byte
  // 10101111 original value (sample)
  // 10100011 original & ~mask
  // 10101011 masked | value
  uint8_t b;
  if (readByte(devAddr, regAddr, &b) != 0) {
    uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    b &= ~(mask); // zero all important bits in existing byte
    b |= data; // combine data with existing byte
    return writeByte(devAddr, regAddr, b);
  } else {
    return false;
  }
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
template<typename WIRE, typename RegAddr>
bool I2CdevT<WIRE, RegAddr>::writeByte(uint8_t devAddr, RegAddr regAddr, uint8_t data) {
  return writeBytes(devAddr, regAddr, 1, &data);
}

/** Write single word to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New word value to write
 * @return Status of operation (true = success)
 */
template<typename WIRE, typename RegAddr>
bool I2CdevT<WIRE, RegAddr>::writeWord(uint8_t devAddr, RegAddr regAddr, uint16_t data) {
  return writeWords(devAddr, regAddr, (uint8_t) 1, &data);
}

/** write a single bit in a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-15)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
template<typename WIRE, typename RegAddr>
bool I2CdevT<WIRE, RegAddr>::writeBitW(uint8_t devAddr, RegAddr regAddr, uint8_t bitNum, uint16_t data) {
  uint16_t w;
  readWord(devAddr, regAddr, &w);
  w = (data != 0) ? (w | (1 << bitNum)) : (w & ~(1 << bitNum));
  return writeWord(devAddr, regAddr, w);
}

/** Write multiple bits in a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-15)
 * @param length Number of bits to write (not more than 16)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
template<typename WIRE, typename RegAddr>
bool I2CdevT<WIRE, RegAddr>::writeBitsW(uint8_t devAddr, RegAddr regAddr, uint8_t bitStart, uint8_t length, uint16_t data) {
  //              010 value to write
  // fedcba9876543210 bit numbers
  //    xxx           args: bitStart=12, length=3
  // 0001110000000000 mask word
  // 1010111110010110 original value (sample)
  // 1010001110010110 original & ~mask
  // 1010101110010110 masked | value
  uint16_t w;
  if (readWord(devAddr, regAddr, &w) != 0) {
    uint16_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    w &= ~(mask); // zero all important bits in existing word
    w |= data; // combine data with existing word
    return writeWord(devAddr, regAddr, w);
  } else {
    return false;
  }
}


typedef I2CdevT<TwoWire, uint16_t> I2Cdev16;
typedef I2CdevT<TwoWire, uint8_t> I2Cdev8;
typedef I2CdevT<TwoWire, uint8_t> I2Cdev;

#endif /* _I2CDEV_H_ */
