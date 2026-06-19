#include <Arduino.h>

#include "eeprom_i2c_driver.h"
#include "I2C_eeprom.h"
#include "key_value_database.h"

EepromI2cDriver driver(0x50, I2C_DEVICESIZE_24LC512);
KeyValueDatabase<EepromI2cDriver> database(driver, I2C_DEVICESIZE_24LC512);


void setup() {
    Serial.begin(115200);

    if (!driver.setup()) {
        Serial.println("EEPROM not connected");
        while (true);
    }

    Serial.println("EEPROM connected");

    const long start = 1337l;

    Serial.print("Inserting data: ");
    Serial.println(start);

    char key[16] = "test\0";

    database.insert<sizeof(long)>(key, (const byte*) &start);

    const auto read = database.get<long>(key);

    Serial.print("Reading data: ");

    Serial.println(read);

    Serial.println("Done.");
}

void loop() {
    delay(1000);
}