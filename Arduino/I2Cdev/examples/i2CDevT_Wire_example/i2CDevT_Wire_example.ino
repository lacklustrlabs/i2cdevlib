
#include "I2Cdev.h"
#include "AT24C32.h"
#include "DS1307.h"

I2CdevT<TwoWire,uint16_t> i2cdev16(Wire);  // A I2Cdev template specialization with 16 bit register addresses
// I2Cdev is now shorthand for I2CdevT<TwoWire,uint8_t>
I2Cdev i2cdev8(Wire);                      // A I2Cdev template specialization with  8 bit register addresses

AT24C32<TwoWire> at24c32(i2cdev16); // AT24C32 uses 12 bit register addresses
DS1307<TwoWire> rtc(i2cdev8);       // uses the normal 8 bit register addresses

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
    i2cdev8.begin();
    i2cdev16.begin();

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
    Serial.println("");
}

void loop() {
   uint16_t year;
   uint8_t month, day, dow, hours, minutes, seconds;

    // read all clock info from device
    rtc.getDateTime24(&year, &month, &day, &hours, &minutes, &seconds);

    // display YYYY-MM-DD hh:mm:ss time
    Serial.print("rtc:\t");
    Serial.print(year); Serial.print("-");
    if (month < 10) Serial.print("0");
    Serial.print(month); Serial.print("-");
    if (day < 10) Serial.print("0");
    Serial.print(day); Serial.print(" ");
    if (hours < 10) Serial.print("0");
    Serial.print(hours); Serial.print(":");
    if (minutes < 10) Serial.print("0");
    Serial.print(minutes); Serial.print(":");
    if (seconds < 10) Serial.print("0");
    Serial.println(seconds);

    // wait one second so the next reading will be different
    delay(1000);
}
