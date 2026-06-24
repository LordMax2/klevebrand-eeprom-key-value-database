#pragma once

#include <stdint.h>

class BaseStorageDriver
{
    virtual int read(int start, uint8_t *buffer, int length);

    virtual int write(int start, const uint8_t *buffer, int length);

    virtual long getSize();

public:
    virtual ~BaseStorageDriver() = default;
};
