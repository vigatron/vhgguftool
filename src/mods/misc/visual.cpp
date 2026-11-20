#include "visual.hpp"

/** @brief Print a 2D bit field. @param arr Bit array. @param w Width. */
void ShowBitField2D(const std::vector<uint8_t> &arr, size_t w)
{

    printf("show() w=%d", (uint16_t)w);

    for (size_t idx = 0; idx < arr.size() * 8; idx++)
    {
        if (!(idx % w))
            printf("\n");
        bool val = VHBits::BitVal(arr.data(), idx);
        printf("%c", val ? 'X' : '.');
    }

    printf("\n\n");
}


/** @brief Extract sign bits of a tensor. @param gguf GGUF reader. @param tensorname Tensor name. @param layer Layer index. @return Sign bit array. */
std::vector<uint8_t> ExtractTensorsSigns(
    VHGGUF &gguf,
    const std::string &tensorname,
    std::size_t layer)
{
    std::vector<uint8_t> r;

    size_t tidx;

    // Valid key ?
    if (!gguf.FindTensorID(tensorname, &tidx))
        return r;

    const VHGUFFTensor &tensor = gguf.GetTensor(tidx);

    //
    std::vector<uint16_t> arrbf16 = gguf.ReadTensorContent(tensor, layer, 1);

    //
    r.resize(static_cast<std::size_t>(arrbf16.size() / 8), 0);

    for (size_t idx = 0; idx < arrbf16.size(); idx++)
    {
        if ((arrbf16[idx] >> 15) & 1)
            VHBits::BitSet(r.data(), idx);
    }

    return r;
}
