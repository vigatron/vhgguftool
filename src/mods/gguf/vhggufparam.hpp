#pragma once

#include "vhplatform.hpp"
#include "vhfile.hpp"
#include "vhggufio.hpp"

#include <stdexcept>

/**
 * @brief Container for a single GGUF metadata key/value pair.
 */
class VHGGUFParam
{
public:
    /** @brief Default constructor. */
    VHGGUFParam() {}

    /** @brief Get the metadata key. @return Metadata key string. */
    const std::string &val_key() const { return param_key; }

    /** @brief Get the metadata value type. @return GGUF value type code. */
    u32 val_param_type() const { return param_type; }

    /** @brief Get the value as a 32-bit unsigned integer. @return Value. */
    u32 val_param_u32() const { return param_u32; }

    /** @brief Get the value as a string. @return Value string. */
    const std::string &val_param_str() const { return param_str; }

    /**
     * @brief Read and parse one metadata entry from the file.
     * @param file GGUF file handle.
     */
    void readparam(VHFile &file)
    {
        param_key = VHGGUFIO::read_string(file);
        param_type = VHGGUFIO::read_u32(file);

        //
        switch (param_type)
        {
        case UINT32:
            param_u32 = VHGGUFIO::read_u32(file);
            break;

        case STRING:
            param_str = VHGGUFIO::read_string(file);
            break;

        default:
            skip_value(param_type, file);
        }
    }

    /**
     * @brief Skip a primitive metadata value.
     * @param type GGUF value type code.
     * @param file GGUF file handle.
     */
    void skip_primitive(u32 type, VHFile &file)
    {
        size_t n;

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

    /**
     * @brief Skip a metadata value, including arrays.
     * @param type GGUF value type code.
     * @param file GGUF file handle.
     */
    void skip_value(u32 type, VHFile &file)
    {
        if (type != ARRAY)
        {
            skip_primitive(type, file);
            return;
        }

        // Skip array
        u32 element_type = VHGGUFIO::read_u32(file);
        u64 count = VHGGUFIO::read_u64(file);

        if (element_type == ARRAY || count > 100000000)
            throw std::runtime_error("Invalid GGUF metadata array");

        for (u64 i = 0; i < count; ++i)
            skip_primitive(element_type, file);
    }

    /** @brief Print the metadata entry to standard output. */
    void dump()
    {

        std::cout << "KEY: " << val_key();
        std::cout << " TYPE: " << val_param_type();
        std::cout << " ";
        std::cout << " VAL: ";

        switch (val_param_type())
        {
        case UINT32:
            std::cout << val_param_u32();
            break;
        case STRING:
            std::cout << val_param_str();
            break;
        default:
            std::cout << "*";
        }

        std::cout << "\n";
    }

private:
    // Param
    std::string param_key;  ///< Metadata key.
    u32 param_type;         ///< Metadata value type.
    u32 param_u32;          ///< Cached 32-bit value.
    std::string param_str;  ///< Cached string value.
};