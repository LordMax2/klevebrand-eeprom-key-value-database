#ifndef KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H
#define KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H

template <int KeyLength, int DataSize, int SlotSize>
struct Bucket
{
    Bucket() = default;

    Bucket(const char key[KeyLength], const byte* data)
    {
        memcpy(this->key, key, KeyLength);
        memcpy(this->data, data, DataSize);
    }

    char key[KeyLength] = {};
    byte data[DataSize] = {};
    byte padding[SlotSize - sizeof(char[KeyLength]) - sizeof(byte[DataSize])] = {};
};


#endif //KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H
