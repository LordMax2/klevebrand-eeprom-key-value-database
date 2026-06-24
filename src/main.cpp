#include <Arduino.h>

#include "eeprom_i2c_driver.h"
#include "key_value_database.h"


EepromI2cDriver driver(0x50, STANDARD_EEPROM_SIZE_65536);
KeyValueDatabase<EepromI2cDriver, 64, 16> database(driver);

struct TestStruct {
    uint8_t a;
    uint16_t b;
    uint32_t c;
};

void test1() {
    constexpr char key[16] = "testa\0";

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
    constexpr TestStruct test{123, 456, 789};

    constexpr char key[16] = "test5\0";

    const auto start_micros_insert = micros();

    database.insert<sizeof(TestStruct)>(key, (const byte *) &test);

    const auto end_micros_insert = micros();

    const auto start_micros = micros();

    const auto read = database.get<TestStruct>(key);

    const auto end_micros = micros();

    Serial.print("Reading data: ");
    Serial.print(read.a);
    Serial.print(", ");
    Serial.print(read.b);
    Serial.print(", ");
    Serial.print(read.c);
    Serial.print(", benchmark: insert time: ");
    Serial.print(end_micros_insert - start_micros_insert);
    Serial.print(", read time: ");
    Serial.println(end_micros - start_micros);
}

void test3() {
    database.insert("A$", 220l);
    database.insert("A\x97", 230l);
    database.insert("A\xC7", 240l);
    database.insert("ABC", 250l);

    Serial.println();
    Serial.println("Reading data: ");
    auto a = database.get<long>("A$");
    auto b = database.get<long>("A\x97");
    auto c = database.get<long>("A\xC7");
    auto d = database.get<long>("ABC");
    Serial.println("---");
    Serial.println(a);
    Serial.println(b);
    Serial.println(c);
    Serial.println(d);
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
