#pragma once

#include "gguf/vhgguf.hpp"

/** @brief Show sign maps for a range of widths. @param arrsigns Sign bits. @param width_min Minimum width. @param width_max Maximum width. @return vok. */
verr cmdShowSignsMap(const std::vector<uint8_t> & arrsigns, uint16_t width_min, uint16_t width_max);
