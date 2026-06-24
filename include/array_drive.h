/*
 * Example implementation for a regular array driver, the database works with any driver which implements the BaseStorageDriver interface.
 */

#pragma once

#include <Arduino.h>
#include "base_storage_driver.h"

template <long SlotSize>
class ArrayDriver : BaseStorageDriver
{
    const byte _buffer[1024][SlotSize]{};

public:
    int read(int start, uint8_t* buffer, int length) override
    {
        return _buffer[start];
    }

    int write(int start, const uint8_t* buffer, int length) override
    {
        memccpy(buffer, _buffer[start], SlotSize);

        return 1;
    }

    long getSize() override
    {
        return 1024l;
    }
};
