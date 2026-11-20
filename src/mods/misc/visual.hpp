#pragma once

#include "vhgguf.hpp"

/** @brief Print a 2D bit field. @param arr Bit array. @param w Width. */
void ShowBitField2D(const std::vector<uint8_t> &arr, size_t w);

/** @brief Extract sign bits of a tensor. @param gguf GGUF reader. @param tensorname Tensor name. @param layer Layer index. @return Sign bit array. */
std::vector<uint8_t> ExtractTensorsSigns(
    VHGGUF &gguf,
    const std::string &tensorname,
    std::size_t layer);
