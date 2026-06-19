#include <Arduino.h>

#include "eeprom_i2c_driver.h"
#include "I2C_eeprom.h"
#include "key_value_database.h"

EepromI2cDriver driver(0x50, I2C_DEVICESIZE_24LC512);
KeyValueDatabase<EepromI2cDriver> database(driver, I2C_DEVICESIZE_24LC512);

struct TestStruct {
    uint8_t a;
    uint16_t b;
    uint32_t c;
};

void test1() {
    char key[16] = "testa\0";

    const auto start_micros_insert = micros();

    database.insert(key, 1339l);

    const auto end_micros_insert = micros();

    const auto start_micros = micros();

    const auto read = database.get<long>(key);

    const auto end_micros = micros();

    Serial.print("Reading data: ");
    Serial.print(read);
    Serial.print(", benchmark: insert time: ");
    Serial.print(end_micros_insert - start_micros_insert);
    Serial.print(", read time: ");
    Serial.println(end_micros - start_micros);
}

void test2() {
   TestStruct test {123, 456, 789};

    char key[16] = "test2\0";

    database.insert<sizeof(TestStruct)>(key, (const byte*)&test);

    const auto read = database.get<TestStruct>(key);

    Serial.print("Reading data: ");
    Serial.print(read.a);
    Serial.print(", ");
    Serial.print(read.b);
    Serial.print(", ");
    Serial.println(read.c);
}


void setup() {
    Serial.begin(115200);

    if (!driver.setup()) {
        Serial.println("EEPROM not connected");
        while (true);
    }

    Serial.println("EEPROM connected");

    test1();

    Serial.println("Done.");
}

void loop() {
    delay(1000);
}