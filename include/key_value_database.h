//
// Created by maxkl on 2026-06-19.
//

#ifndef KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H
#define KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H

#include <Arduino.h>
#include <HardwareSerial.h>

#include "bucket.h"

static constexpr long STANDARD_EEPROM_SIZE_65536 = 65536;
static constexpr long STANDARD_EEPROM_SIZE_32768 = 32768;
static constexpr long STANDARD_EEPROM_SIZE_16384 = 16384;
static constexpr long STANDARD_EEPROM_SIZE_8192 = 8192;
static constexpr long STANDARD_EEPROM_SIZE_4096 = 4096;
static constexpr long STANDARD_EEPROM_SIZE_2048 = 2048;
static constexpr long STANDARD_EEPROM_SIZE_1024 = 1024;
static constexpr long STANDARD_EEPROM_SIZE_512 = 512;
static constexpr long STANDARD_EEPROM_SIZE_256 = 256;
static constexpr long STANDARD_EEPROM_SIZE_128 = 128;

template <class EepromDriver>
class KeyValueDatabase
{
    EepromDriver _driver;

    long _eeprom_size;

    static constexpr int SLOT_SIZE = 128;
    static constexpr int KEY_LENGTH = 16;

    int hashCode(const char* str) const
    {
        int hash = 0;
        for (int i = 0; str[i] != '\0'; i++)
        {
            hash = (hash * 31 + str[i]) % _eeprom_size;
        }
        return hash;
    }

    int getAlignedStartPosition(const char* key) const
    {
        int hash = hashCode(key);

        hash = hash % _eeprom_size;

        const int start = hash - hash % SLOT_SIZE;

        return start;
    }

public:
    explicit KeyValueDatabase(EepromDriver driver) : _driver(driver), _eeprom_size(driver.getSize())
    {
    }

    bool insert(char key[KEY_LENGTH], const long* data)
    {
        return insert<sizeof(long)>(key, (const byte*)&data);
    }

    bool insert(char key[KEY_LENGTH], const long data)
    {
        return insert<sizeof(long)>(key, (const byte*)&data);
    }

    template <int size>
    bool insert(char key[KEY_LENGTH], const byte* data)
    {
        // Prevent insert for now if size is too large, somehow maybe spread the object out in multiple slots in the future.
        if (size > SLOT_SIZE)
        {
            return false;
        }

        const int start = getAlignedStartPosition(key);

        Bucket<KEY_LENGTH, size, SLOT_SIZE> bucket;

        _driver.read(start, (byte*)&bucket, SLOT_SIZE);

        bool is_occupied = false;

        for (int i = 0; i < 16; i++)
        {
            if (bucket.key[i] == '\0')
            {
                is_occupied = true;
            }
        }

        if (is_occupied && false)
        {
            // Rehash and find new position
            Serial.println("Is occupied");

            return true;
        }

        Bucket<16, size, SLOT_SIZE> bucket_to_insert(key, data);

        _driver.write(start, (byte*)&bucket_to_insert, SLOT_SIZE);

        return true;
    }

    template <typename T>
    T get(char key[KEY_LENGTH])
    {
        const int start = getAlignedStartPosition(key);

        Bucket<KEY_LENGTH, sizeof(T), SLOT_SIZE> bucket;
        _driver.read(start, (byte*)&bucket, SLOT_SIZE);

        T result;

        memcpy(&result, bucket.data, sizeof(T));

        return result;
    }
};

#endif //KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H
