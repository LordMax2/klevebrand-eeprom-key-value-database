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

    char key[16] = "test\0";

    const auto start_micros_insert = micros();

    database.insert(key, 1339l);

    const auto end_micros_insert = micros();

    const auto start_micros = micros();

    const auto read = database.get<long>(key);

    const auto end_micros = micros();

    Serial.print("Reading data: ");
    Serial.println(read);

    Serial.println("Time: ");
    Serial.println("Insert:");
    Serial.println(end_micros_insert - start_micros_insert);
    Serial.println("Read:");
    Serial.println(end_micros - start_micros);

    Serial.println("Done.");
}

void loop() {
    delay(1000);
}