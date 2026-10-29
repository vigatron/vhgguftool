#include "texttools.hpp"

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
