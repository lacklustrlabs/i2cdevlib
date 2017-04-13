// I2Cdev library collection - DS1307 I2C device class header file
// Based on Maxim DS1307 datasheet, 2008
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

#ifndef _DS1307_H_
#define _DS1307_H_

#include "I2Cdev.h"

// comment this out if you already have a JeeLabs' DateTime class in your code
// or if you don't need DateTime functionality
#define DS1307_INCLUDE_DATETIME_CLASS

// comment this out if you don't need DateTime functionality
#define DS1307_INCLUDE_DATETIME_METHODS

#define DS1307_ADDRESS              0x68 // this device only has one address
#define DS1307_DEFAULT_ADDRESS      0x68

#define DS1307_RA_SECONDS           0x00
#define DS1307_RA_MINUTES           0x01
#define DS1307_RA_HOURS             0x02
#define DS1307_RA_DAY               0x03
#define DS1307_RA_DATE              0x04
#define DS1307_RA_MONTH             0x05
#define DS1307_RA_YEAR              0x06
#define DS1307_RA_CONTROL           0x07
#define DS1307_RA_RAM               0x08

#define DS1307_SECONDS_CH_BIT       7
#define DS1307_SECONDS_10_BIT       6
#define DS1307_SECONDS_10_LENGTH    3
#define DS1307_SECONDS_1_BIT        3
#define DS1307_SECONDS_1_LENGTH     4

#define DS1307_MINUTES_10_BIT       6
#define DS1307_MINUTES_10_LENGTH    3
#define DS1307_MINUTES_1_BIT        3
#define DS1307_MINUTES_1_LENGTH     4

#define DS1307_HOURS_MODE_BIT       6 // 0 = 24-hour mode, 1 = 12-hour mode
#define DS1307_HOURS_AMPM_BIT       5 // 2nd HOURS_10 bit if in 24-hour mode
#define DS1307_HOURS_10_BIT         4
#define DS1307_HOURS_1_BIT          3
#define DS1307_HOURS_1_LENGTH       4

#define DS1307_DAY_BIT              2
#define DS1307_DAY_LENGTH           3

#define DS1307_DATE_10_BIT          5
#define DS1307_DATE_10_LENGTH       2
#define DS1307_DATE_1_BIT           3
#define DS1307_DATE_1_LENGTH        4

#define DS1307_MONTH_10_BIT         4
#define DS1307_MONTH_1_BIT          3
#define DS1307_MONTH_1_LENGTH       4

#define DS1307_YEAR_10H_BIT         7
#define DS1307_YEAR_10H_LENGTH      4
#define DS1307_YEAR_1H_BIT          3
#define DS1307_YEAR_1H_LENGTH       4

#define DS1307_CONTROL_OUT_BIT      7
#define DS1307_CONTROL_SQWE_BIT     4
#define DS1307_CONTROL_RS_BIT       1
#define DS1307_CONTROL_RS_LENGTH    2

#define DS1307_SQW_RATE_1           0x0
#define DS1307_SQW_RATE_4096        0x1
#define DS1307_SQW_RATE_8192        0x2
#define DS1307_SQW_RATE_32768       0x3

#ifdef DS1307_INCLUDE_DATETIME_CLASS
    // DateTime class courtesy of public domain JeeLabs code
    // simple general-purpose date/time class (no TZ / DST / leap second handling!)
    class DateTime {
        public:
            DateTime (uint32_t t=0);
            DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour=0, uint8_t min=0, uint8_t sec=0);
            DateTime (const char* date, const char* time);
            uint16_t year() const       { return 2000 + yOff; }
            uint8_t month() const       { return m; }
            uint8_t day() const         { return d; }
            uint8_t hour() const        { return hh; }
            uint8_t minute() const      { return mm; }
            uint8_t second() const      { return ss; }
            uint8_t dayOfWeek() const;
    
            // 32-bit times as seconds since 1/1/2000
            long secondstime() const;
            // 32-bit times as seconds since 1/1/1970
            uint32_t unixtime(void) const;
        
        protected:
            uint8_t yOff, m, d, hh, mm, ss;
    };
#endif

template <typename WIRE>
class DS1307 {
        DS1307() = delete;
        DS1307( const DS1307& other ) = delete; // non construction-copyable
        DS1307 & operator=( const DS1307& ) = delete; // non copyable

    public:
        DS1307(I2CdevT<WIRE, uint8_t>& i2cdev, uint8_t address=DS1307_DEFAULT_ADDRESS);

        void initialize();
        bool testConnection();

        // SECONDS register
        bool getClockRunning();
        void setClockRunning(bool running);
        uint8_t getSeconds(); // 0-59
        void setSeconds(uint8_t seconds);

        // MINUTES register
        uint8_t getMinutes(); // 0-59
        void setMinutes(uint8_t minutes);

        // HOURS register
        uint8_t getMode(); // 0-1
        void setMode(uint8_t mode);
        uint8_t getAMPM(); // 0-1
        void setAMPM(uint8_t ampm);
        uint8_t getHours12(); // 1-12
        void setHours12(uint8_t hours, uint8_t ampm);
        uint8_t getHours24(); // 0-23
        void setHours24(uint8_t hours);

        // DAY register
        uint8_t getDayOfWeek(); // 1-7
        void setDayOfWeek(uint8_t dow);

        // DATE register
        uint8_t getDay(); // 1-31
        void setDay(uint8_t day);
        
        // MONTH register
        uint8_t getMonth(); // 1-12
        void setMonth(uint8_t month);

        // YEAR register
        uint16_t getYear(); // 1970, 2000, 2011, etc
        void setYear(uint16_t year);
        
        // CONTROL register
        bool getFixedOutputLevel();
        void setFixedOutputLevel(bool level);
        bool getSquareWaveEnabled();
        void setSquareWaveEnabled(bool enabled);
        uint8_t getSquareWaveRate();
        void setSquareWaveRate(uint8_t rate);
        
        // RAM registers
        uint8_t getMemoryByte(uint8_t offset);
        void setMemoryByte(uint8_t offset, uint8_t value);

        // convenience methods

        void getDate(uint16_t *year, uint8_t *month, uint8_t *day);
        void setDate(uint16_t year, uint8_t month, uint8_t day);

        void getTime12(uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm);
        void setTime12(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm);
        
        void getTime24(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
        void setTime24(uint8_t hours, uint8_t minutes, uint8_t seconds);
        
        void getDateTime12(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm);
        void setDateTime12(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm);
        
        void getDateTime24(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
        void setDateTime24(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds);
        
        #ifdef DS1307_INCLUDE_DATETIME_METHODS
            DateTime getDateTime();
            void setDateTime(const DateTime& dt);
        #endif

    private:
        I2CdevT<WIRE, uint8_t>& _i2cdev;
        uint8_t devAddr;
        uint8_t buffer[1];
        bool mode12;
        bool clockHalt;
};


/** Specific address constructor.
 * @param address I2C address
 * @see DS1307_DEFAULT_ADDRESS
 * @see DS1307_ADDRESS
 */
template <typename WIRE>
DS1307<WIRE>::DS1307(I2CdevT<WIRE, uint8_t>& i2cdev, uint8_t address=DS1307_DEFAULT_ADDRESS):_i2cdev(i2cdev) {
    devAddr = address;
}

/** Power on and prepare for general usage.
 * This method reads the current 12/24-hour mode setting into the locally stored
 * variable so that it isn't accidentally changed by the setHour*() methods.
 */
template <typename WIRE>
void DS1307<WIRE>::initialize() {
    getMode(); // automatically sets internal "mode12" member variable
    getClockRunning(); // automatically sets internal "clockHalt" member variable
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
template <typename WIRE>
bool DS1307<WIRE>::testConnection() {
    if (_i2cdev.readByte(devAddr, DS1307_RA_SECONDS, buffer) == 1) {
        return true;
    }
    return false;
}

// SECONDS register
template <typename WIRE>
bool DS1307<WIRE>::getClockRunning() {
    _i2cdev.readBit(devAddr, DS1307_RA_SECONDS, DS1307_SECONDS_CH_BIT, buffer);
    clockHalt = buffer[0];
    return !buffer[0];
}

template <typename WIRE>
void DS1307<WIRE>::setClockRunning(bool running) {
    _i2cdev.writeBit(devAddr, DS1307_RA_SECONDS, DS1307_SECONDS_CH_BIT, !running);
}

template <typename WIRE>
uint8_t DS1307<WIRE>::getSeconds() {
    // Byte: [7 = CH] [6:4 = 10SEC] [3:0 = 1SEC]
    _i2cdev.readByte(devAddr, DS1307_RA_SECONDS, buffer);
    clockHalt = buffer[0] & 0x80;
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x70) >> 4) * 10;
}

template <typename WIRE>
void DS1307<WIRE>::setSeconds(uint8_t seconds) {
    if (seconds > 59) return;
    uint8_t value = (clockHalt ? 0x80 : 0x00) + ((seconds / 10) << 4) + (seconds % 10);
    _i2cdev.writeByte(devAddr, DS1307_RA_SECONDS, value);
}

// MINUTES register
template <typename WIRE>
uint8_t DS1307<WIRE>::getMinutes() {
    // Byte: [7 = 0] [6:4 = 10MIN] [3:0 = 1MIN]
    _i2cdev.readByte(devAddr, DS1307_RA_MINUTES, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x70) >> 4) * 10;
}

template <typename WIRE>
void DS1307<WIRE>::setMinutes(uint8_t minutes) {
    if (minutes > 59) return;
    uint8_t value = ((minutes / 10) << 4) + (minutes % 10);
    _i2cdev.writeByte(devAddr, DS1307_RA_MINUTES, value);
}

// HOURS register
template <typename WIRE>
uint8_t DS1307<WIRE>::getMode() {
    _i2cdev.readBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_MODE_BIT, buffer);
    mode12 = buffer[0];
    return buffer[0];
}

template <typename WIRE>
void DS1307<WIRE>::setMode(uint8_t mode) {
    _i2cdev.writeBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_MODE_BIT, mode);
}

template <typename WIRE>
uint8_t DS1307<WIRE>::getAMPM() {
    _i2cdev.readBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_AMPM_BIT, buffer);
    return buffer[0];
}

template <typename WIRE>
void DS1307<WIRE>::setAMPM(uint8_t ampm) {
    _i2cdev.writeBit(devAddr, DS1307_RA_HOURS, DS1307_HOURS_AMPM_BIT, ampm);
}

template <typename WIRE>
uint8_t DS1307<WIRE>::getHours12() {
    _i2cdev.readByte(devAddr, DS1307_RA_HOURS, buffer);
    mode12 = buffer[0] & 0x40;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        return (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        uint8_t hours = (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;

        // convert 24-hour to 12-hour format, since that's what's requested
        if (hours > 12) hours -= 12;
        else if (hours == 0) hours = 12;
        return hours;
    }
}

template <typename WIRE>
void DS1307<WIRE>::setHours12(uint8_t hours, uint8_t ampm) {
    if (hours > 12 || hours < 1) return;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        if (ampm > 0) ampm = 0x20;
        uint8_t value = 0x40 + ampm + ((hours / 10) << 4) + (hours % 10);
        _i2cdev.writeByte(devAddr, DS1307_RA_HOURS, value);
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        if (ampm > 0) hours += 12;
        if (hours == 0) hours = 12; // 12 AM
        else if (hours == 24) hours = 12; // 12 PM, after +12 adjustment
        uint8_t value = ((hours / 10) << 4) + (hours % 10);
        _i2cdev.writeByte(devAddr, DS1307_RA_HOURS, value);
    }
}

template <typename WIRE>
uint8_t DS1307<WIRE>::getHours24() {
    _i2cdev.readByte(devAddr, DS1307_RA_HOURS, buffer);
    mode12 = buffer[0] & 0x40;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        uint8_t hours = (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;

        // convert 12-hour to 24-hour format, since that's what's requested
        if (buffer[0] & 0x20) {
            // currently PM
            if (hours < 12) hours += 12;
        } else {
            // currently AM
            if (hours == 12) hours = 0;
        }
        return hours;
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        return (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;
    }
}

template <typename WIRE>
void DS1307<WIRE>::setHours24(uint8_t hours) {
    if (hours > 23) return;
    if (mode12) {
        // bit 6 is high, 12-hour mode
        // Byte: [5 = AM/PM] [4 = 10HR] [3:0 = 1HR]
        uint8_t ampm = 0;
        if (hours > 11) ampm = 0x20;
        if (hours > 12) hours -= 12;
        else if (hours == 0) hours = 12;
        uint8_t value = 0x40 + ampm + ((hours / 10) << 4) + (hours % 10);
        _i2cdev.writeByte(devAddr, DS1307_RA_HOURS, value);
    } else {
        // bit 6 is low, 24-hour mode (default)
        // Byte: [5:4 = 10HR] [3:0 = 1HR]
        uint8_t value = ((hours / 10) << 4) + (hours % 10);
        _i2cdev.writeByte(devAddr, DS1307_RA_HOURS, value);
    }
}

// DAY register
template <typename WIRE>
uint8_t DS1307<WIRE>::getDayOfWeek() {
    _i2cdev.readBits(devAddr, DS1307_RA_DAY, DS1307_DAY_BIT, DS1307_DAY_LENGTH, buffer);
    return buffer[0];
}

template <typename WIRE>
void DS1307<WIRE>::setDayOfWeek(uint8_t dow) {
    if (dow < 1 || dow > 7) return;
    _i2cdev.writeBits(devAddr, DS1307_RA_DAY, DS1307_DAY_BIT, DS1307_DAY_LENGTH, dow);
}

// DATE register
template <typename WIRE>
uint8_t DS1307<WIRE>::getDay() {
    // Byte: [7:6 = 0] [5:4 = 10DAY] [3:0 = 1DAY]
  _i2cdev.readByte(devAddr, DS1307_RA_DATE, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x30) >> 4) * 10;
}

template <typename WIRE>
void DS1307<WIRE>::setDay(uint8_t day) {
    uint8_t value = ((day / 10) << 4) + (day % 10);
    _i2cdev.writeByte(devAddr, DS1307_RA_DATE, value);
}

// MONTH register
template <typename WIRE>
uint8_t DS1307<WIRE>::getMonth() {
    // Byte: [7:5 = 0] [4 = 10MONTH] [3:0 = 1MONTH]
    _i2cdev.readByte(devAddr, DS1307_RA_MONTH, buffer);
    return (buffer[0] & 0x0F) + ((buffer[0] & 0x10) >> 4) * 10;
}

template <typename WIRE>
void DS1307<WIRE>::setMonth(uint8_t month) {
    if (month < 1 || month > 12) return;
    uint8_t value = ((month / 10) << 4) + (month % 10);
    _i2cdev.writeByte(devAddr, DS1307_RA_MONTH, value);
}

// YEAR register
template <typename WIRE>
uint16_t DS1307<WIRE>::getYear() {
    _i2cdev.readByte(devAddr, DS1307_RA_YEAR, buffer);
    return 2000 + (buffer[0] & 0x0F) + ((buffer[0] & 0xF0) >> 4) * 10;
}

template <typename WIRE>
void DS1307<WIRE>::setYear(uint16_t year) {
    if (year < 2000) return;
    year -= 2000;
    uint8_t value = ((year / 10) << 4) + (year % 10);
    _i2cdev.writeByte(devAddr, DS1307_RA_YEAR, value);
}

// CONTROL register
template <typename WIRE>
bool DS1307<WIRE>::getFixedOutputLevel() {
    _i2cdev.readBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_OUT_BIT, buffer);
    return buffer[0];
}

template <typename WIRE>
void DS1307<WIRE>::setFixedOutputLevel(bool level) {
    _i2cdev.writeBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_OUT_BIT, level);
}

template <typename WIRE>
bool DS1307<WIRE>::getSquareWaveEnabled() {
    _i2cdev.readBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_SQWE_BIT, buffer);
    return buffer[0];
}

template <typename WIRE>
void DS1307<WIRE>::setSquareWaveEnabled(bool enabled) {
    _i2cdev.writeBit(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_SQWE_BIT, enabled);
}

template <typename WIRE>
uint8_t DS1307<WIRE>::getSquareWaveRate() {
    _i2cdev.readBits(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_RS_BIT, DS1307_CONTROL_RS_LENGTH, buffer);
    return buffer[0];
}

template <typename WIRE>
void DS1307<WIRE>::setSquareWaveRate(uint8_t rate) {
    _i2cdev.writeBits(devAddr, DS1307_RA_CONTROL, DS1307_CONTROL_RS_BIT, DS1307_CONTROL_RS_LENGTH, rate);
}

// RAM registers
template <typename WIRE>
uint8_t DS1307<WIRE>::getMemoryByte(uint8_t offset) {
    if (offset > 55) return 0;
    _i2cdev.readByte(devAddr, DS1307_RA_RAM + offset, buffer);
    return buffer[0];
}

template <typename WIRE>
void DS1307<WIRE>::setMemoryByte(uint8_t offset, uint8_t value) {
    if (offset > 55) return;
    _i2cdev.writeByte(devAddr, DS1307_RA_RAM + offset, value);
}

// convenience methods
template <typename WIRE>
void DS1307<WIRE>::getDate(uint16_t *year, uint8_t *month, uint8_t *day) {
    *year = getYear();
    *month = getMonth();
    *day = getDay();
}
template <typename WIRE>
void DS1307<WIRE>::setDate(uint16_t year, uint8_t month, uint8_t day) {
    setYear(year);
    setMonth(month);
    setDay(day);
}

template <typename WIRE>
void DS1307<WIRE>::getTime12(uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm) {
    *hours = getHours12();
    *minutes = getMinutes();
    *seconds = getSeconds();
    *ampm = getAMPM();
}

template <typename WIRE>
void DS1307<WIRE>::setTime12(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm) {
    // write seconds first to reset divider chain and give
    // us 1 second to write remaining time info
    setSeconds(seconds);
    setMinutes(minutes);
    setHours12(hours, ampm);
}

template <typename WIRE>
void DS1307<WIRE>::getTime24(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    *hours = getHours24();
    *minutes = getMinutes();
    *seconds = getSeconds();
}

template <typename WIRE>
void DS1307<WIRE>::setTime24(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    // write seconds first to reset divider chain and give
    // us 1 second to write remaining time info
    setSeconds(seconds);
    setMinutes(minutes);
    setHours24(hours);
}

template <typename WIRE>
void DS1307<WIRE>::getDateTime12(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds, uint8_t *ampm) {
    getTime12(hours, minutes, seconds, ampm);
    getDate(year, month, day);
}

template <typename WIRE>
void DS1307<WIRE>::setDateTime12(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t ampm) {
    setTime12(hours, minutes, seconds, ampm);
    setDate(year, month, day);
}

template <typename WIRE>
void DS1307<WIRE>::getDateTime24(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    getTime24(hours, minutes, seconds);
    getDate(year, month, day);
}

template <typename WIRE>
void DS1307<WIRE>::setDateTime24(uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds) {
    setTime24(hours, minutes, seconds);
    setDate(year, month, day);
}

#ifdef DS1307_INCLUDE_DATETIME_METHODS
    template <typename WIRE>
    DateTime DS1307<WIRE>::getDateTime() {
        DateTime dt = DateTime(getYear(), getMonth(), getDay(), getHours24(), getMinutes(), getSeconds());
        return dt;
    }

    template <typename WIRE>
    void DS1307<WIRE>::setDateTime(const DateTime& dt) {
        setSeconds(dt.second());
        setMinutes(dt.minute());
        setHours24(dt.hour());
        setDay(dt.day());
        setMonth(dt.month());
        setYear(dt.year());
    }
#endif


#endif /* _DS1307_H_ */
