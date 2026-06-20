#include "bucket.h"

template <class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::hashCode(const char* str) const
{
    long hash = 0;
    for (long i = 0; str[i] != '\0'; i++)
    {
        hash = (hash * 31 + str[i]) % (_eeprom_size / SlotSize);
    }
    return hash;
}

template <class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::getStartPosition(const char* key) const
{
    const long hash = hashCode(key);

    return hash * SlotSize;
}

template <class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::hash1(const char* str) const
{
    return 0;
}

template <class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::hash2(const char* str) const
{
    return 0;
}

template <class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::getStartPositionCuckoo(const char* str) const
{
    return 0;
}

template <class EepromDriver, long SlotSize, long KeyLength>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(char key[KeyLength], const long* data)
{
    return insert<sizeof(long)>(key, (const byte*)&data);
}

template <class EepromDriver, long SlotSize, long KeyLength>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(char key[KeyLength], const long data)
{
    return insert<sizeof(long)>(key, (const byte*)&data);
}

template <class EepromDriver, long SlotSize, long KeyLength>
template <long size>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(char key[KeyLength], const byte* data)
{
    // Prevent insert for now if size is too large, somehow maybe spread the object out in multiple slots in the future.
    if (size > SlotSize)
    {
        return false;
    }

    int start = getStartPosition(key);

    Bucket<KeyLength, size, SlotSize> existing_bucket;

    _driver.read(start, (byte*)&existing_bucket, SlotSize);

    Bucket<16, size, SlotSize> bucket_to_insert(key, data);

    _driver.write(start, (byte*)&bucket_to_insert, SlotSize);

    return true;
}

template <class EepromDriver, long SlotSize, long KeyLength>
template <typename T>
T KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::get(char key[KeyLength])
{
    const int start = getStartPosition(key);

    Bucket<KeyLength, sizeof(T), SlotSize> bucket;
    _driver.read(start, (byte*)&bucket, SlotSize);

    T result;

    memcpy(&result, bucket.data, sizeof(T));

    return result;
}
