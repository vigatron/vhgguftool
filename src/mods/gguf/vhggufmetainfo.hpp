#pragma once

#include "aiplatform.hpp"

#include "vhfile.hpp"
#include "vhggufio.hpp"

#include <stdexcept>
#include <string>

/**
 * @brief Descriptor of a single GGUF metadata entry.
 *
 * Stores the key, value type, value (for scalar types), array info, and the
 * position/size of the entry inside the GGUF file.
 */
class VHGUFFMetaInfo
{
public:
    /** @brief Default constructor. */
    VHGUFFMetaInfo();

    /**
     * @brief Read one metadata entry from the file.
     * @param file GGUF file handle.
     * @return `vok` on success, error code otherwise.
     */
    verr readmeta(VHFile &file);

    /** @brief Print the metadata entry to standard output. */
    void dump() const;

    /** @brief Get the metadata key. @return Key string. */
    const std::string &key() const;

    /** @brief Get the metadata value type. @return GGUF value type code. */
    u32 type() const;

    /**
     * @brief Get the human-readable name of a GGUF metadata value type.
     * @param type GGUF value type code.
     * @return Type name (e.g. "UINT8", "STRING"), or "UNKNOWN" for unknown codes.
     */
    static std::string type_name(u32 type);

    /** @brief Format the metadata value as a string. @return Value string. */
    std::string value_string() const;

    /** @brief Get the file offset of the entry start. @return Offset in bytes. */
    u64 file_offset() const;

    /** @brief Get the size of the entry in bytes. @return Size in bytes. */
    u64 byte_size() const;

    // Scalar getters
    /** @brief Get value as uint8. @return Value. */
    u8 value_u8() const;
    /** @brief Get value as int8. @return Value. */
    int8_t value_i8() const;
    /** @brief Get value as bool. @return Value. */
    bool value_bool() const;

    /** @brief Get value as uint16. @return Value. */
    uint16_t value_u16() const;
    /** @brief Get value as int16. @return Value. */
    int16_t value_i16() const;

    /** @brief Get value as uint32. @return Value. */
    u32 value_u32() const;
    /** @brief Get value as int32. @return Value. */
    int32_t value_i32() const;
    /** @brief Get value as float32. @return Value. */
    float value_f32() const;

    /** @brief Get value as uint64. @return Value. */
    u64 value_u64() const;
    /** @brief Get value as int64. @return Value. */
    int64_t value_i64() const;
    /** @brief Get value as float64. @return Value. */
    double value_f64() const;

    /** @brief Get value as string. @return Value string. */
    const std::string &value_str() const;

    // Array info
    /** @brief Get array element type. @return GGUF value type code. */
    u32 array_element_type() const;
    /** @brief Get array element count. @return Count. */
    u64 array_count() const;

private:
    void skip_primitive(u32 type, VHFile &file);
    void skip_value(u32 type, VHFile &file);

    std::string key_;
    u32 type_ = 0;
    u64 file_offset_ = 0;
    u64 byte_size_ = 0;

    // Scalar storage
    u8 value_u8_ = 0;
    int8_t value_i8_ = 0;
    bool value_bool_ = false;

    uint16_t value_u16_ = 0;
    int16_t value_i16_ = 0;

    u32 value_u32_ = 0;
    int32_t value_i32_ = 0;
    float value_f32_ = 0.0f;

    u64 value_u64_ = 0;
    int64_t value_i64_ = 0;
    double value_f64_ = 0.0;

    std::string value_str_;

    // Array info
    u32 array_element_type_ = 0;
    u64 array_count_ = 0;
};