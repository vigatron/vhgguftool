#pragma once

#include "vhplatform.hpp"
#include "vhfile.hpp"

#include <stdexcept>


/**
 * @brief Low-level helper functions for reading GGUF primitive values.
 */
class VHGGUFIO
{
public:
    /**
     * @brief Read exactly n bytes from the file or throw on failure.
     * @param file GGUF file handle.
     * @param dst Destination buffer.
     * @param n Number of bytes to read.
     */
    static void read_bytes(VHFile &file, void *dst, std::size_t n)
    {
        if (!file.read(dst, n))
            throw std::runtime_error("Unexpected end of GGUF file");
    }

    /**
     * @brief Read a little-endian 32-bit unsigned integer.
     * @param file GGUF file handle.
     * @return Read value.
     */
    static uint32_t read_u32(VHFile &file)
    {
        uint8_t b[4];

        read_bytes(file, b, sizeof(b));

        return uint32_t(b[0]) |
               (uint32_t(b[1]) << 8) |
               (uint32_t(b[2]) << 16) |
               (uint32_t(b[3]) << 24);
    }

    /**
     * @brief Read a little-endian 64-bit unsigned integer.
     * @param file GGUF file handle.
     * @return Read value.
     */
    static uint64_t read_u64(VHFile &file)
    {
        uint8_t b[8];
        read_bytes(file, b, sizeof(b));
        uint64_t v = 0;
        for (int i = 0; i < 8; ++i)
            v |= uint64_t(b[i]) << (8 * i);
        return v;
    }

    /**
     * @brief Read a length-prefixed GGUF string.
     * @param file GGUF file handle.
     * @return Read string.
     */
    static std::string read_string(VHFile &file)
    {
        uint64_t n = read_u64(file);
        if (n > 16 * 1024 * 1024)
            throw std::runtime_error("Invalid GGUF string length");

        std::string s(static_cast<size_t>(n), '\0');
        if (n)
            read_bytes(file, s.data(), static_cast<size_t>(n));

        return s;
    }
};