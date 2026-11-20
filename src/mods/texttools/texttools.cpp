#include "texttools.hpp"

#include "vhplatform.hpp"

#include <fstream>
#include <iostream>
#include <iomanip>


/** @brief Convert a value to a hexadecimal string. @param val Value to convert. @return Hexadecimal string. */
std::string addrhex(uint64_t val) {
    std::stringstream ss;
    ss << std::hex << val;
    std::string hex_str = ss.str();
    return hex_str;
}

/** @brief Dump a BF16 value. @param v BF16 value. */
void dump_bf16(uint16_t v)
{
    bool sign = (v >>15)& 1;
    uint8_t exp = (v & 0x7FFF) >> 7;
    uint8_t mnt =  v & 127;

    printf("%.4x  %c%.3d.%.3d \n", v, sign ? '-': ' ', exp, mnt);

}

/**
 * @brief Format a size in bytes as a compact "smart" string.
 * @details Uses binary units (1024-based). Values below 1 KiB are printed as
 *          plain bytes; larger values use K/M/G/T with the remainder truncated.
 *          Examples: 368 -> "368", 1024 -> "1K", 1048600 -> "1M".
 * @param val Size in bytes.
 * @return Human-readable size string.
 */
std::string smartsize(size_t val)
{
    const size_t k = F1K;
    const size_t m = F1M;
    const size_t g = F1K * F1M;
    const size_t t = F1M * F1M;

    if (val < k)
        return std::to_string(val);
    if (val < m)
        return std::to_string(val / k) + "K";
    if (val < g)
        return std::to_string(val / m) + "M";
    if (val < t)
        return std::to_string(val / g) + "G";

    return std::to_string(val / t) + "T";
}
