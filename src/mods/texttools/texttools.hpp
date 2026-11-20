#pragma once

#include <string>
#include <cstdint>

/** @brief Convert a value to hexadecimal string. @param val Value. @return Hex string. */
std::string addrhex(uint64_t val);
/** @brief Dump a BF16 value. @param v Value. */
void dump_bf16(uint16_t v);
/**
 * @brief Format a size in bytes as a compact "smart" string.
 * @details Uses binary units (1024-based). Values below 1 KiB are printed as
 *          plain bytes; larger values use K/M/G/T with the remainder truncated.
 *          Examples: 368 -> "368", 1024 -> "1K", 1048600 -> "1M".
 * @param val Size in bytes.
 * @return Human-readable size string.
 */
std::string smartsize(size_t val);