#include "bucket.h"

template <class EepromDriver, int SlotSize, int KeyLength>
int KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::hashCode(const char* str) const
{
    int hash = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        hash = (hash * 31 + str[i]) % _eeprom_size;
    }
    return hash;
}

template <class EepromDriver, int SlotSize, int KeyLength>
int KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::getAlignedStartPosition(const char* key) const
{
    int hash = hashCode(key);

    hash = hash % _eeprom_size;

    const int start = hash - hash % SlotSize;

    return start;
}

template <class EepromDriver, int SlotSize, int KeyLength>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(char key[KeyLength], const long* data)
{
    return insert<sizeof(long)>(key, (const byte*)&data);
}

template <class EepromDriver, int SlotSize, int KeyLength>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(char key[KeyLength], const long data)
{
    return insert<sizeof(long)>(key, (const byte*)&data);
}

template <class EepromDriver, int SlotSize, int KeyLength>
template <int size>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(char key[KeyLength], const byte* data)
{
    // Prevent insert for now if size is too large, somehow maybe spread the object out in multiple slots in the future.
    if (size > SlotSize)
    {
        return false;
    }

    int start = getAlignedStartPosition(key);

    Bucket<KeyLength, size, SlotSize> existing_bucket;

    _driver.read(start, (byte*)&existing_bucket, SlotSize);

    Bucket<16, size, SlotSize> bucket_to_insert(key, data);

    _driver.write(start, (byte*)&bucket_to_insert, SlotSize);

    return true;
}

template <class EepromDriver, int SlotSize, int KeyLength>
template <typename T>
T KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::get(char key[KeyLength])
{
    const int start = getAlignedStartPosition(key);

    Bucket<KeyLength, sizeof(T), SlotSize> bucket;
    _driver.read(start, (byte*)&bucket, SlotSize);

    T result;

    memcpy(&result, bucket.data, sizeof(T));

    return result;
}
