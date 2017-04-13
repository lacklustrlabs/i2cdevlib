// I2Cdev library collection - Main I2C device class
// Abstracts bit and byte I2C R/W functions into a convenient class
// 2013-06-05 by Jeff Rowberg <jeff@rowberg.net>
//
// Changelog:
//      2017-04-xx - initial release

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


#include "I2Cdev.h"
#include <SoftWire.h>

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param _timeout Optional read _timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (-1 indicates failure)
 */
template<>
int8_t I2CdevT<SoftWire, uint8_t>::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {

    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print("I2C (0x");
        I2CDEV_DEBUG_OUTPUT.print(devAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print(") reading ");
        I2CDEV_DEBUG_OUTPUT.print(length, DEC);
        I2CDEV_DEBUG_OUTPUT.print(" bytes from 0x");
        I2CDEV_DEBUG_OUTPUT.print(regAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print("...");
    #endif

    int8_t count = 0;
    uint32_t t1 = millis();


    // Arduino v1.0.1+, Wire library
    // Adds official support for repeated start condition, yay!

    // I2C/TWI subsystem uses internal buffer that breaks with large data requests
    // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
    // smaller chunks instead of all at once
    for (uint8_t k = 0; k < length; k += min(length, BUFFER_LENGTH)) {
        _wire.beginTransmission(devAddr);
        _wire.write(regAddr);
        _wire.endTransmission();
        _wire.beginTransmission(devAddr);
        _wire.requestFrom(devAddr, (uint8_t)min(length - k, BUFFER_LENGTH));

        for (; _wire.available() && (_timeout == 0 || millis() - t1 < _timeout); count++) {
            data[count] = _wire.read();
            #ifdef I2CDEV_SERIAL_DEBUG
                I2CDEV_DEBUG_OUTPUT.print(data[count], HEX);
                if (count + 1 < length) I2CDEV_DEBUG_OUTPUT.print(" ");
            #endif
        }
    }

    // check for _timeout
    if (_timeout > 0 && millis() - t1 >= _timeout && count < length) count = -1; // _timeout

    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print(". Done (");
        I2CDEV_DEBUG_OUTPUT.print(count, DEC);
        I2CDEV_DEBUG_OUTPUT.println(" read).");
    #endif

    return count;
}

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from (note: 16 bits)
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param _timeout Optional read _timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (-1 indicates failure)
 */
template<>
int8_t I2CdevT<SoftWire, uint16_t>::readBytes(uint8_t devAddr, uint16_t regAddr, uint8_t length, uint8_t *data) {

#ifdef I2CDEV_SERIAL_DEBUG
    I2CDEV_DEBUG_OUTPUT.print("I2C (0x");
    I2CDEV_DEBUG_OUTPUT.print(devAddr, HEX);
    I2CDEV_DEBUG_OUTPUT.print(") reading ");
    I2CDEV_DEBUG_OUTPUT.print(length, DEC);
    I2CDEV_DEBUG_OUTPUT.print(" bytes from 0x");
    I2CDEV_DEBUG_OUTPUT.print(regAddr, HEX);
    I2CDEV_DEBUG_OUTPUT.print("...");
#endif

int8_t count = 0;
uint32_t t1 = millis();


// Arduino v1.0.1+, Wire library
// Adds official support for repeated start condition, yay!

// I2C/TWI subsystem uses internal buffer that breaks with large data requests
// so if user requests more than BUFFER_LENGTH bytes, we have to do it in
// smaller chunks instead of all at once
for (uint8_t k = 0; k < length; k += min(length, BUFFER_LENGTH)) {
    _wire.beginTransmission(devAddr);
    _wire.write((uint8_t) (regAddr>>8));
    _wire.write((uint8_t) regAddr);
    _wire.endTransmission();
    _wire.beginTransmission(devAddr);
    _wire.requestFrom(devAddr, (uint8_t)min(length - k, BUFFER_LENGTH));

    for (; _wire.available() && (_timeout == 0 || millis() - t1 < _timeout); count++) {
        data[count] = _wire.read();
        #ifdef I2CDEV_SERIAL_DEBUG
            I2CDEV_DEBUG_OUTPUT.print(data[count], HEX);
            if (count + 1 < length) I2CDEV_DEBUG_OUTPUT.print(" ");
        #endif
    }
}

// check for _timeout
if (_timeout > 0 && millis() - t1 >= _timeout && count < length) count = -1; // _timeout

#ifdef I2CDEV_SERIAL_DEBUG
    I2CDEV_DEBUG_OUTPUT.print(". Done (");
    I2CDEV_DEBUG_OUTPUT.print(count, DEC);
    I2CDEV_DEBUG_OUTPUT.println(" read).");
#endif

return count;
}

/** Read multiple words from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of words to read
 * @param data Buffer to store read data in
 * @param _timeout Optional read _timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of words read (-1 indicates failure)
 */
template<>
int8_t I2CdevT<SoftWire, uint8_t>::readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data) {

    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print("I2C (0x");
        I2CDEV_DEBUG_OUTPUT.print(devAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print(") reading ");
        I2CDEV_DEBUG_OUTPUT.print(length, DEC);
        I2CDEV_DEBUG_OUTPUT.print(" words from 0x");
        I2CDEV_DEBUG_OUTPUT.print(regAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print("...");
    #endif

    int8_t count = 0;
    uint32_t t1 = millis();


    // Arduino v1.0.1+, Wire library
    // Adds official support for repeated start condition, yay!

    // I2C/TWI subsystem uses internal buffer that breaks with large data requests
    // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
    // smaller chunks instead of all at once
    for (uint8_t k = 0; k < length * 2; k += min(length * 2, BUFFER_LENGTH)) {
        _wire.beginTransmission(devAddr);
        _wire.write(regAddr);
        _wire.endTransmission();
        _wire.beginTransmission(devAddr);
        _wire.requestFrom(devAddr, (uint8_t)(length * 2)); // length=words, this wants bytes

        bool msb = true; // starts with MSB, then LSB
        for (; _wire.available() && count < length && (I2CdevT<SoftWire, uint8_t>::_timeout == 0 || millis() - t1 < I2CdevT<SoftWire, uint8_t>::_timeout);) {
            if (msb) {
                // first byte is bits 15-8 (MSb=15)
                data[count] = _wire.read() << 8;
            } else {
                // second byte is bits 7-0 (LSb=0)
                data[count] |= _wire.read();
                #ifdef I2CDEV_SERIAL_DEBUG
                    I2CDEV_DEBUG_OUTPUT.print(data[count], HEX);
                    if (count + 1 < length) I2CDEV_DEBUG_OUTPUT.print(" ");
                #endif
                count++;
            }
            msb = !msb;
        }

        _wire.endTransmission();
    }

    if (I2CdevT<SoftWire, uint8_t>::_timeout > 0 && millis() - t1 >= I2CdevT<SoftWire, uint8_t>::_timeout && count < length) count = -1; // _timeout

    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print(". Done (");
        I2CDEV_DEBUG_OUTPUT.print(count, DEC);
        I2CDEV_DEBUG_OUTPUT.println(" read).");
    #endif

    return count;
}

/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
template<>
bool I2CdevT<SoftWire, uint8_t>::writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print("I2C (0x");
        I2CDEV_DEBUG_OUTPUT.print(devAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print(") writing ");
        I2CDEV_DEBUG_OUTPUT.print(length, DEC);
        I2CDEV_DEBUG_OUTPUT.print(" bytes to 0x");
        I2CDEV_DEBUG_OUTPUT.print(regAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print("...");
    #endif
    uint8_t status = 0;
    _wire.beginTransmission(devAddr);
    _wire.write((uint8_t) regAddr); // send address
    for (uint8_t i = 0; i < length; i++) {
        #ifdef I2CDEV_SERIAL_DEBUG
            I2CDEV_DEBUG_OUTPUT.print(data[i], HEX);
            if (i + 1 < length) I2CDEV_DEBUG_OUTPUT.print(" ");
        #endif
        _wire.write((uint8_t) data[i]);
    }
    status = _wire.endTransmission();
    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.println(". Done.");
    #endif
    return status == 0;
}

/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to (note: 16 bits)
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
template<>
bool I2CdevT<SoftWire, uint16_t>::writeBytes(uint8_t devAddr, uint16_t regAddr, uint8_t length, uint8_t* data) {
    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print("I2C (0x");
        I2CDEV_DEBUG_OUTPUT.print(devAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print(") writing ");
        I2CDEV_DEBUG_OUTPUT.print(length, DEC);
        I2CDEV_DEBUG_OUTPUT.print(" bytes to 0x");
        I2CDEV_DEBUG_OUTPUT.print(regAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print("...");
    #endif
    uint8_t status = 0;
    _wire.beginTransmission(devAddr);
    _wire.write(regAddr>>8);
    _wire.write((uint8_t)regAddr);
    for (uint8_t i = 0; i < length; i++) {
        #ifdef I2CDEV_SERIAL_DEBUG
            I2CDEV_DEBUG_OUTPUT.print(data[i], HEX);
            if (i + 1 < length) I2CDEV_DEBUG_OUTPUT.print(" ");
        #endif
        _wire.write((uint8_t) data[i]);
    }
    status = _wire.endTransmission();
    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.println(". Done.");
    #endif
    return status == 0;
}

/** Write multiple words to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
template<>
bool I2CdevT<SoftWire, uint8_t>::writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t* data) {
    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print("I2C (0x");
        I2CDEV_DEBUG_OUTPUT.print(devAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print(") writing ");
        I2CDEV_DEBUG_OUTPUT.print(length, DEC);
        I2CDEV_DEBUG_OUTPUT.print(" words to 0x");
        I2CDEV_DEBUG_OUTPUT.print(regAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print("...");
    #endif
    uint8_t status = 0;
    _wire.beginTransmission(devAddr);
    _wire.write(regAddr); // send address
    for (uint8_t i = 0; i < length * 2; i++) {
        #ifdef I2CDEV_SERIAL_DEBUG
            I2CDEV_DEBUG_OUTPUT.print(data[i], HEX);
            if (i + 1 < length) I2CDEV_DEBUG_OUTPUT.print(" ");
        #endif
        _wire.write((uint8_t)(data[i] >> 8));    // send MSB
        _wire.write((uint8_t)data[i++]);         // send LSB
    }
    status = _wire.endTransmission();
    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.println(". Done.");
    #endif
    return status == 0;
}

#if defined(ARDUINO_ARCH_STM32F1)

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param _timeout Optional read _timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of bytes read (-1 indicates failure)
 */
template<>
int8_t I2CdevT<HardWire, uint8_t>::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {

    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print("I2C (0x");
        I2CDEV_DEBUG_OUTPUT.print(devAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print(") reading ");
        I2CDEV_DEBUG_OUTPUT.print(length, DEC);
        I2CDEV_DEBUG_OUTPUT.print(" bytes from 0x");
        I2CDEV_DEBUG_OUTPUT.print(regAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print("...");
    #endif

    int8_t count = 0;
    uint32_t t1 = millis();


    // Arduino v1.0.1+, Wire library
    // Adds official support for repeated start condition, yay!

    // I2C/TWI subsystem uses internal buffer that breaks with large data requests
    // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
    // smaller chunks instead of all at once
    for (uint8_t k = 0; k < length; k += min(length, BUFFER_LENGTH)) {
        _wire.beginTransmission(devAddr);
        _wire.write(regAddr);
        _wire.endTransmission();
        _wire.beginTransmission(devAddr);
        _wire.requestFrom(devAddr, (uint8_t)min(length - k, BUFFER_LENGTH));

        for (; _wire.available() && (_timeout == 0 || millis() - t1 < _timeout); count++) {
            data[count] = _wire.read();
            #ifdef I2CDEV_SERIAL_DEBUG
                I2CDEV_DEBUG_OUTPUT.print(data[count], HEX);
                if (count + 1 < length) I2CDEV_DEBUG_OUTPUT.print(" ");
            #endif
        }
    }

    // check for _timeout
    if (_timeout > 0 && millis() - t1 >= _timeout && count < length) count = -1; // _timeout

    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print(". Done (");
        I2CDEV_DEBUG_OUTPUT.print(count, DEC);
        I2CDEV_DEBUG_OUTPUT.println(" read).");
    #endif

    return count;
}

/** Read multiple words from a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of words to read
 * @param data Buffer to store read data in
 * @param _timeout Optional read _timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Number of words read (-1 indicates failure)
 */
template<>
int8_t I2CdevT<HardWire, uint8_t>::readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data) {

    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print("I2C (0x");
        I2CDEV_DEBUG_OUTPUT.print(devAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print(") reading ");
        I2CDEV_DEBUG_OUTPUT.print(length, DEC);
        I2CDEV_DEBUG_OUTPUT.print(" words from 0x");
        I2CDEV_DEBUG_OUTPUT.print(regAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print("...");
    #endif

    int8_t count = 0;
    uint32_t t1 = millis();


    // Arduino v1.0.1+, Wire library
    // Adds official support for repeated start condition, yay!

    // I2C/TWI subsystem uses internal buffer that breaks with large data requests
    // so if user requests more than BUFFER_LENGTH bytes, we have to do it in
    // smaller chunks instead of all at once
    for (uint8_t k = 0; k < length * 2; k += min(length * 2, BUFFER_LENGTH)) {
        _wire.beginTransmission(devAddr);
        _wire.write(regAddr);
        _wire.endTransmission();
        _wire.beginTransmission(devAddr);
        _wire.requestFrom(devAddr, (uint8_t)(length * 2)); // length=words, this wants bytes

        bool msb = true; // starts with MSB, then LSB
        for (; _wire.available() && count < length && (I2CdevT<HardWire, uint8_t>::_timeout == 0 || millis() - t1 < I2CdevT<HardWire, uint8_t>::_timeout);) {
            if (msb) {
                // first byte is bits 15-8 (MSb=15)
                data[count] = _wire.read() << 8;
            } else {
                // second byte is bits 7-0 (LSb=0)
                data[count] |= _wire.read();
                #ifdef I2CDEV_SERIAL_DEBUG
                    I2CDEV_DEBUG_OUTPUT.print(data[count], HEX);
                    if (count + 1 < length) I2CDEV_DEBUG_OUTPUT.print(" ");
                #endif
                count++;
            }
            msb = !msb;
        }

        _wire.endTransmission();
    }

    if (I2CdevT<HardWire, uint8_t>::_timeout > 0 && millis() - t1 >= I2CdevT<HardWire, uint8_t>::_timeout && count < length) count = -1; // _timeout

    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print(". Done (");
        I2CDEV_DEBUG_OUTPUT.print(count, DEC);
        I2CDEV_DEBUG_OUTPUT.println(" read).");
    #endif

    return count;
}

/** Write multiple bytes to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
template<>
bool I2CdevT<HardWire, uint8_t>::writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print("I2C (0x");
        I2CDEV_DEBUG_OUTPUT.print(devAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print(") writing ");
        I2CDEV_DEBUG_OUTPUT.print(length, DEC);
        I2CDEV_DEBUG_OUTPUT.print(" bytes to 0x");
        I2CDEV_DEBUG_OUTPUT.print(regAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print("...");
    #endif
    uint8_t status = 0;
    _wire.beginTransmission(devAddr);
    _wire.write((uint8_t) regAddr); // send address
    for (uint8_t i = 0; i < length; i++) {
        #ifdef I2CDEV_SERIAL_DEBUG
            I2CDEV_DEBUG_OUTPUT.print(data[i], HEX);
            if (i + 1 < length) I2CDEV_DEBUG_OUTPUT.print(" ");
        #endif
        _wire.write((uint8_t) data[i]);
    }
    status = _wire.endTransmission();
    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.println(". Done.");
    #endif
    return status == 0;
}

/** Write multiple words to a 16-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
template<>
bool I2CdevT<HardWire, uint8_t>::writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t* data) {
    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.print("I2C (0x");
        I2CDEV_DEBUG_OUTPUT.print(devAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print(") writing ");
        I2CDEV_DEBUG_OUTPUT.print(length, DEC);
        I2CDEV_DEBUG_OUTPUT.print(" words to 0x");
        I2CDEV_DEBUG_OUTPUT.print(regAddr, HEX);
        I2CDEV_DEBUG_OUTPUT.print("...");
    #endif
    uint8_t status = 0;
    _wire.beginTransmission(devAddr);
    _wire.write(regAddr); // send address
    for (uint8_t i = 0; i < length * 2; i++) {
        #ifdef I2CDEV_SERIAL_DEBUG
            I2CDEV_DEBUG_OUTPUT.print(data[i], HEX);
            if (i + 1 < length) I2CDEV_DEBUG_OUTPUT.print(" ");
        #endif
        _wire.write((uint8_t)(data[i] >> 8));    // send MSB
        _wire.write((uint8_t)data[i++]);         // send LSB
    }
    status = _wire.endTransmission();
    #ifdef I2CDEV_SERIAL_DEBUG
        I2CDEV_DEBUG_OUTPUT.println(". Done.");
    #endif
    return status == 0;
}

#endif
