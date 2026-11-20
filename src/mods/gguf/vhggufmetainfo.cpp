#include "vhggufmetainfo.hpp"

#include <cstring>
#include <sstream>

/** @brief Default constructor. */
VHGUFFMetaInfo::VHGUFFMetaInfo() {}

/**
 * @brief Get the human-readable name of a GGUF metadata value type.
 * @param type GGUF value type code.
 * @return Type name (e.g. "UINT8", "STRING"), or "UNKNOWN" for unknown codes.
 */
std::string VHGUFFMetaInfo::type_name(u32 type)
{
    switch (type) {
    case UINT8:   return "UINT8";
    case INT8:    return "INT8";
    case UINT16:  return "UINT16";
    case INT16:   return "INT16";
    case UINT32:  return "UINT32";
    case INT32:   return "INT32";
    case FLOAT32: return "FLOAT32";
    case BOOL:    return "BOOL";
    case STRING:  return "STRING";
    case ARRAY:   return "ARRAY";
    case UINT64:  return "UINT64";
    case INT64:   return "INT64";
    case FLOAT64: return "FLOAT64";
    default:      return "UNKNOWN";
    }
}

/** */
verr VHGUFFMetaInfo::readmeta(VHFile &file)
{
    u64 start_pos = 0;
    if (!file.pos(&start_pos))
        return verror(1);

    file_offset_ = start_pos;

    key_ = VHGGUFIO::read_string(file);
    type_ = VHGGUFIO::read_u32(file);

    switch (type_)
    {
    case UINT8:
    {
        u8 b = 0;
        if (!file.read(reinterpret_cast<char *>(&b), 1))
            throw std::runtime_error("Unexpected end of GGUF file");
        value_u8_ = b;
        break;
    }
    case INT8:
    {
        int8_t b = 0;
        if (!file.read(reinterpret_cast<char *>(&b), 1))
            throw std::runtime_error("Unexpected end of GGUF file");
        value_i8_ = b;
        break;
    }
    case BOOL:
    {
        u8 b = 0;
        if (!file.read(reinterpret_cast<char *>(&b), 1))
            throw std::runtime_error("Unexpected end of GGUF file");
        value_bool_ = (b != 0);
        break;
    }
    case UINT16:
    {
        u8 b[2];
        if (!file.read(reinterpret_cast<char *>(b), 2))
            throw std::runtime_error("Unexpected end of GGUF file");
        value_u16_ = static_cast<uint16_t>(b[0]) | (static_cast<uint16_t>(b[1]) << 8);
        break;
    }
    case INT16:
    {
        u8 b[2];
        if (!file.read(reinterpret_cast<char *>(b), 2))
            throw std::runtime_error("Unexpected end of GGUF file");
        value_i16_ = static_cast<int16_t>(static_cast<uint16_t>(b[0]) | (static_cast<uint16_t>(b[1]) << 8));
        break;
    }
    case UINT32:
        value_u32_ = VHGGUFIO::read_u32(file);
        break;
    case INT32:
        value_i32_ = static_cast<int32_t>(VHGGUFIO::read_u32(file));
        break;
    case FLOAT32:
    {
        u32 raw = VHGGUFIO::read_u32(file);
        std::memcpy(&value_f32_, &raw, sizeof(value_f32_));
        break;
    }
    case UINT64:
        value_u64_ = VHGGUFIO::read_u64(file);
        break;
    case INT64:
        value_i64_ = static_cast<int64_t>(VHGGUFIO::read_u64(file));
        break;
    case FLOAT64:
    {
        u64 raw = VHGGUFIO::read_u64(file);
        std::memcpy(&value_f64_, &raw, sizeof(value_f64_));
        break;
    }
    case STRING:
        value_str_ = VHGGUFIO::read_string(file);
        break;
    case ARRAY:
    {
        array_element_type_ = VHGGUFIO::read_u32(file);
        array_count_ = VHGGUFIO::read_u64(file);

        if (array_element_type_ == ARRAY || array_count_ > 100000000)
            throw std::runtime_error("Invalid GGUF metadata array");

        for (u64 i = 0; i < array_count_; ++i)
            skip_primitive(array_element_type_, file);
        break;
    }
    default:
        throw std::runtime_error("Unsupported GGUF metadata type");
    }

    u64 end_pos = 0;
    if (!file.pos(&end_pos))
        return verror(1);

    byte_size_ = end_pos - start_pos;

    return vok;
}

/** @brief Format the metadata value as a string (same as printed by dump()). @return Value string. */
std::string VHGUFFMetaInfo::value_string() const
{
    std::ostringstream oss;
    switch (type_)
    {
    case UINT8:
        oss << static_cast<int>(value_u8_);
        break;
    case INT8:
        oss << static_cast<int>(value_i8_);
        break;
    case BOOL:
        oss << (value_bool_ ? "true" : "false");
        break;
    case UINT16:
        oss << value_u16_;
        break;
    case INT16:
        oss << value_i16_;
        break;
    case UINT32:
        oss << value_u32_;
        break;
    case INT32:
        oss << value_i32_;
        break;
    case FLOAT32:
        oss << value_f32_;
        break;
    case UINT64:
        oss << value_u64_;
        break;
    case INT64:
        oss << value_i64_;
        break;
    case FLOAT64:
        oss << value_f64_;
        break;
    case STRING:
        oss << value_str_;
        break;
    case ARRAY:
        oss << "array<" << array_element_type_ << ">[" << array_count_ << "]";
        break;
    default:
        oss << "*";
    }
    return oss.str();
}

/** */
void VHGUFFMetaInfo::dump() const
{
    std::cout << "KEY: " << key_;
    std::cout << " TYPE: " << type_;
    std::cout << " OFFSET: " << file_offset_;
    std::cout << " SIZE: " << byte_size_;
    std::cout << " VAL: " << value_string();
    std::cout << "\n";
}

// Getters
/** */ const std::string &VHGUFFMetaInfo::key() const { return key_; }
/** */ u32 VHGUFFMetaInfo::type() const { return type_; }
/** */ u64 VHGUFFMetaInfo::file_offset() const { return file_offset_; }
/** */ u64 VHGUFFMetaInfo::byte_size() const { return byte_size_; }

/** */ u8 VHGUFFMetaInfo::value_u8() const { return value_u8_; }
/** */ int8_t VHGUFFMetaInfo::value_i8() const { return value_i8_; }
/** */ bool VHGUFFMetaInfo::value_bool() const { return value_bool_; }

/** */ uint16_t VHGUFFMetaInfo::value_u16() const { return value_u16_; }
/** */ int16_t VHGUFFMetaInfo::value_i16() const { return value_i16_; }

/** */ u32 VHGUFFMetaInfo::value_u32() const { return value_u32_; }
/** */ int32_t VHGUFFMetaInfo::value_i32() const { return value_i32_; }
/** */ float VHGUFFMetaInfo::value_f32() const { return value_f32_; }

/** */ u64 VHGUFFMetaInfo::value_u64() const { return value_u64_; }
/** */ int64_t VHGUFFMetaInfo::value_i64() const { return value_i64_; }
/** */ double VHGUFFMetaInfo::value_f64() const { return value_f64_; }

/** */ const std::string &VHGUFFMetaInfo::value_str() const { return value_str_; }

/** */ u32 VHGUFFMetaInfo::array_element_type() const { return array_element_type_; }
/** */ u64 VHGUFFMetaInfo::array_count() const { return array_count_; }

/** */
void VHGUFFMetaInfo::skip_primitive(u32 type, VHFile &file)
{
    size_t n = 0;

    switch (type)
    {
    case UINT8:
    case INT8:
    case BOOL:
        n = 1;
        break;

    case UINT16:
    case INT16:
        n = 2;
        break;

    case UINT32:
    case INT32:
    case FLOAT32:
        n = 4;
        break;

    case UINT64:
    case INT64:
    case FLOAT64:
        n = 8;
        break;

    case STRING:
        (void)VHGGUFIO::read_string(file);
        return;

    default:
        throw std::runtime_error("Unsupported GGUF metadata type");
    }

    char tmp[8];
    if (!file.read(tmp, n))
        throw std::runtime_error("Unexpected end of GGUF file");
}

/** */
void VHGUFFMetaInfo::skip_value(u32 type, VHFile &file)
{
    if (type != ARRAY)
    {
        skip_primitive(type, file);
        return;
    }

    u32 element_type = VHGGUFIO::read_u32(file);
    u64 count = VHGGUFIO::read_u64(file);

    if (element_type == ARRAY || count > 100000000)
        throw std::runtime_error("Invalid GGUF metadata array");

    for (u64 i = 0; i < count; ++i)
        skip_primitive(element_type, file);
}