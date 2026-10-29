#include "cmd_show_signs_map.hpp"
#include "visual.hpp"


/** @brief Show sign maps for a range of widths. @param arrsigns Sign bits. @param width_min Minimum width. @param width_max Maximum width. @return vok. */
verr cmdShowSignsMap(const std::vector<uint8_t> & arrsigns, uint16_t width_min, uint16_t width_max) {

    printf("\n");
    for (size_t w = width_min; w <= width_max; w++)
    {
        ShowBitField2D(arrsigns, w);
    }
    return vok;
}
