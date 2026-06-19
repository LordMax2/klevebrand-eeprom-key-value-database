#include <Arduino.h>
#include <unity.h>

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

void setUp(void) {}
void tearDown(void) {}

void test1() {
    char key[16] = "testa\0";
    //database.insert(key, 1339l);
    long read = database.get<long>(key);

    TEST_ASSERT_EQUAL(1339l, read);
}

void test2() {
    TestStruct test {123, 456, 789};
    char key[16] = "test2\0";
    database.insert<sizeof(TestStruct)>(key, (const byte*)&test);
    auto read = database.get<TestStruct>(key);

    TEST_ASSERT_EQUAL(test.a, read.a);
    TEST_ASSERT_EQUAL(test.b, read.b);
    TEST_ASSERT_EQUAL(test.c, read.c);
}

void setup() {
    delay(2000);

    UNITY_BEGIN();

    if (!driver.setup()) {
        TEST_FAIL_MESSAGE("EEPROM not connected");
    }

    RUN_TEST(test1);
    RUN_TEST(test2);
    UNITY_PRINT_MSG("Read successful: 1339l");

    UNITY_END();
}

void loop() {}
