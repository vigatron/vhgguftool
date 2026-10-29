#pragma once

#include <string>
#include <cstdint>

/** @brief Convert a value to hexadecimal string. @param val Value. @return Hex string. */
std::string addrhex(uint64_t val);
/** @brief Dump a BF16 value. @param v Value. */
void dump_bf16(uint16_t v);