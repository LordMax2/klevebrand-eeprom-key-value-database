#include "bucket.h"
#include "hashtable_values.h"

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
CuckooPositionResult KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::getInsertPosition(const char* key)
{
    const long existing_position = findPosition(key);

    if (existing_position != -1)
    {
        return {false, -1, existing_position};
    }

    const long hash1_start_position = hash1(key) * SlotSize;

    Bucket<KeyLength, 0, SlotSize> existing_bucket;

    _driver.read(hash1_start_position, (byte*)&existing_bucket, SlotSize);

    const bool is_empty = !existing_bucket.hasNullTerminator();
    const bool occupied_by_the_same_key = existing_bucket.keyEquals(key);

    // Is empty, just return the start position, no moving needed.
    if (is_empty || occupied_by_the_same_key)
    {
        return {false, -1, hash1_start_position};
    }

    const long relocation_hash2_start_position = hash2(existing_bucket.key) * SlotSize;

    Bucket<KeyLength, 0, SlotSize> existing_bucket_for_relocating_bucket;

    _driver.read(relocation_hash2_start_position, (byte*)&existing_bucket_for_relocating_bucket, SlotSize);

    const bool relocation_is_empty = !existing_bucket_for_relocating_bucket.hasNullTerminator();
    const bool relocation_occupied_by_the_same_key = existing_bucket_for_relocating_bucket.keyEquals(
        existing_bucket.key);

    // Relocation is empty, return the relocation start position
    if (relocation_is_empty || relocation_occupied_by_the_same_key)
    {
        return {true, relocation_hash2_start_position, hash1_start_position};
    }

    // If we get here, we unfortunately have a cycle, PANIC
    return {true, -1, -1};
}

template <class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::findPosition(const char* key)
{
    const long hash1_start_position = hash1(key) * SlotSize;

    Bucket<KeyLength, 0, SlotSize> existing_bucket_at_hash1_start;

    _driver.read(hash1_start_position, (byte*)&existing_bucket_at_hash1_start, SlotSize);

    if (existing_bucket_at_hash1_start.keyEquals(key)) return hash1_start_position;

    const long hash2_start_position = hash2(key) * SlotSize;

    _driver.read(hash2_start_position, (byte*)&existing_bucket_at_hash1_start, SlotSize);

    if (existing_bucket_at_hash1_start.keyEquals(key)) return hash2_start_position;

    return -1;
}

template <class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::hash1(const char* str) const
{
    uint16_t h = 0x811C;

    for (long i = 0; i < KeyLength; i++)
    {
        if (str[i] == '\0')
        {
            break;
        }

        uint8_t byte_val = (uint8_t)str[i];
        h = (h << 5) + h;

        if (i % 2 == 0)
        {
            h ^= pgm_read_word(&(Table_H1_Byte0[byte_val]));
        }
        else
        {
            h ^= pgm_read_word(&(Table_H1_Byte1[byte_val]));
        }
    }

    return h & ((_eeprom_size / SlotSize) - 1);
}

template <class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::hash2(const char* str) const
{
    uint16_t h = 0xCBF2;

    for (long i = 0; i < KeyLength; i++)
    {
        if (str[i] == '\0')
        {
            break;
        }

        uint8_t byte_val = (uint8_t)str[i];
        h = (h << 5) + h;

        if (i % 2 == 0)
        {
            h ^= pgm_read_word(&(Table_H2_Byte0[byte_val]));
        }
        else
        {
            h ^= pgm_read_word(&(Table_H2_Byte1[byte_val]));
        }
    }

    return h & ((_eeprom_size / SlotSize) - 1);
}

template <class EepromDriver, long SlotSize, long KeyLength>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(const char* key, const long* data)
{
    return insert<sizeof(long)>(key, (const byte*)data);
}

template <class EepromDriver, long SlotSize, long KeyLength>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(const char* key, const long data)
{
    return insert<sizeof(long)>(key, (const byte*)&data);
}

template <class EepromDriver, long SlotSize, long KeyLength>
template <long DataSize>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(const char* key, const byte* data)
{
    // Prevent insert for now if size is too large, somehow maybe spread the object out in multiple slots in the future.
    if (DataSize > SlotSize)
    {
        Serial.println("Size is too large for this EEPROM.");

        return false;
    }

    Bucket<KeyLength, DataSize, SlotSize> existing_bucket;
    Bucket<KeyLength, DataSize, SlotSize> existing_bucket2;

    const auto existing_start_position = findPosition(key);

    if (existing_start_position != -1) return existing_start_position;

    const auto hash1_start = hash1(key) * SlotSize;

    _driver.read(hash1_start, existing_bucket, SlotSize);

    if (!existing_bucket.hasNullTerminator()) return hash1_start;

    const auto hash2_start = hash2(existing_bucket.key) * SlotSize;

    _driver.read(hash2_start, existing_bucket2, SlotSize);

    if (!existing_bucket2.hasNullTerminator() || existing_bucket2.keyEquals(existing_bucket.key))
    {
        _driver.write(hash2_start, existing_bucket2, SlotSize);
    }
    else
    {
        // CYCLE
        // insert(existing_bucket2.key, existing_bucket2.data); // Should be possible to do something like this, not right now but in future

        return false;
    }

    Bucket<KeyLength, DataSize, SlotSize> bucket_to_insert(key, data);

    _driver.write(hash1_start, bucket_to_insert, SlotSize);

    return true;
}

template <class EepromDriver, long SlotSize, long KeyLength>
template <typename T>
T KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::get(const char* key)
{
    const long position = findPosition(key);

    if (position == -1)
    {
        Serial.println("Key not found.");

        return T{};
    }

    Bucket<KeyLength, sizeof(T), SlotSize> bucket;
    _driver.read(position, (byte*)&bucket, SlotSize);

    T result;

    memcpy(&result, bucket.data, sizeof(T));

    return result;
}
