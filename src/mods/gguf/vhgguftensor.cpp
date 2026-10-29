#include "vhgguftensor.hpp"

/** @brief Calculate the total number of elements in the tensor. */
u64 VHGUFFTensor::dim_full_size() const
{
    u64 volume = 1;

    for (u64 dim : dims)
    {
        if (dim != 0 && volume > std::numeric_limits<u64>::max() / dim)
        {
            throw std::runtime_error("Tensor dimension overflow");
        }

        volume *= dim;
    }

    return volume;
}

/** @brief Get the size in bytes of one tensor element (0 for blocked types). */
u64 VHGUFFTensor::type_size_bytes() const
{
    switch (type)
    {
    case GGML_TYPE_F32:
        return 4;
    case GGML_TYPE_F16:
        return 2;
    case GGML_TYPE_BF16:
        return 2;
    case GGML_TYPE_F64:
        return 8;
    case GGML_TYPE_I8:
        return 1;
    case GGML_TYPE_I16:
        return 2;
    case GGML_TYPE_I32:
        return 4;
    case GGML_TYPE_I64:
        return 8;

    default:
        // Blocked (quantized) types have no integer size per element
        return 0;
    }
}

/** @brief Get the block layout (elements and bytes per block) of the tensor type. */
bool VHGUFFTensor::type_block_layout(u64 *block_elems, u64 *block_bytes) const
{
    switch (type)
    {
    // Non-blocked types
    case GGML_TYPE_F32:  *block_elems = 1;   *block_bytes = 4;   return true;
    case GGML_TYPE_F16:  *block_elems = 1;   *block_bytes = 2;   return true;
    case GGML_TYPE_BF16: *block_elems = 1;   *block_bytes = 2;   return true;
    case GGML_TYPE_F64:  *block_elems = 1;   *block_bytes = 8;   return true;
    case GGML_TYPE_I8:   *block_elems = 1;   *block_bytes = 1;   return true;
    case GGML_TYPE_I16:  *block_elems = 1;   *block_bytes = 2;   return true;
    case GGML_TYPE_I32:  *block_elems = 1;   *block_bytes = 4;   return true;
    case GGML_TYPE_I64:  *block_elems = 1;   *block_bytes = 8;   return true;

    // Blocked (quantized) types: elements per block / bytes per block
    case GGML_TYPE_Q4_0:   *block_elems = 32;  *block_bytes = 18;  return true;
    case GGML_TYPE_Q4_1:   *block_elems = 32;  *block_bytes = 20;  return true;
    case GGML_TYPE_Q5_0:   *block_elems = 32;  *block_bytes = 22;  return true;
    case GGML_TYPE_Q5_1:   *block_elems = 32;  *block_bytes = 24;  return true;
    case GGML_TYPE_Q8_0:   *block_elems = 32;  *block_bytes = 34;  return true;
    case GGML_TYPE_Q8_1:   *block_elems = 32;  *block_bytes = 36;  return true;

    case GGML_TYPE_Q2_K:   *block_elems = 256; *block_bytes = 84;  return true;
    case GGML_TYPE_Q3_K:   *block_elems = 256; *block_bytes = 110; return true;
    case GGML_TYPE_Q4_K:   *block_elems = 256; *block_bytes = 144; return true;
    case GGML_TYPE_Q5_K:   *block_elems = 256; *block_bytes = 176; return true;
    case GGML_TYPE_Q6_K:   *block_elems = 256; *block_bytes = 210; return true;
    case GGML_TYPE_Q8_K:   *block_elems = 256; *block_bytes = 292; return true;

    case GGML_TYPE_IQ2_XXS: *block_elems = 256; *block_bytes = 66;  return true;
    case GGML_TYPE_IQ2_XS:  *block_elems = 256; *block_bytes = 74;  return true;
    case GGML_TYPE_IQ2_S:   *block_elems = 256; *block_bytes = 82;  return true;
    case GGML_TYPE_IQ3_XXS: *block_elems = 256; *block_bytes = 98;  return true;
    case GGML_TYPE_IQ3_S:   *block_elems = 256; *block_bytes = 110; return true;
    case GGML_TYPE_IQ1_S:   *block_elems = 256; *block_bytes = 50;  return true;
    case GGML_TYPE_IQ1_M:   *block_elems = 256; *block_bytes = 56;  return true;
    case GGML_TYPE_IQ4_NL:  *block_elems = 32;  *block_bytes = 18;  return true;
    case GGML_TYPE_IQ4_XS:  *block_elems = 256; *block_bytes = 136; return true;

    case GGML_TYPE_TQ1_0:   *block_elems = 256; *block_bytes = 54;  return true;
    case GGML_TYPE_TQ2_0:   *block_elems = 256; *block_bytes = 66;  return true;

    case GGML_TYPE_MXFP4:   *block_elems = 32;  *block_bytes = 17;  return true;
    case GGML_TYPE_NVFP4:   *block_elems = 64;  *block_bytes = 36;  return true;

    case GGML_TYPE_Q1_0:    *block_elems = 128; *block_bytes = 18;  return true;
    case GGML_TYPE_Q2_0:    *block_elems = 64;  *block_bytes = 18;  return true;

    default:
        return false;
    }
}

/** @brief Calculate the size in bytes for the given number of elements. */
bool VHGUFFTensor::bytes_for_elements(u64 elems, u64 *bytes) const
{
    u64 block_elems = 0;
    u64 block_bytes = 0;

    if (!type_block_layout(&block_elems, &block_bytes))
        return false;

    if (block_elems == 0 || (elems % block_elems) != 0)
        return false;

    const u64 blocks = elems / block_elems;

    if (blocks != 0 && block_bytes > std::numeric_limits<u64>::max() / blocks)
        return false;

    *bytes = blocks * block_bytes;

    return true;
}

/** @brief Get the size of one dimension in elements. @param idx Dimension index. */
u64 VHGUFFTensor::dim_size_part(uint8_t idx) const
{
    return dims[idx];
}

/** @brief Get the relative offset of the tensor data block. */
u64 VHGUFFTensor::block_pos() const
{
    return offset;
}

/** @brief Get the size in bytes of one dimension. @param idx Dimension index. */
u64 VHGUFFTensor::block_size_part(uint8_t idx) const
{
    u64 bytes = 0;

    if (!bytes_for_elements(dim_size_part(idx), &bytes))
        throw std::runtime_error("Invalid tensor block size");

    return bytes;
}

/** @brief Get the total size in bytes of tensor data. */
u64 VHGUFFTensor::block_size_full() const
{
    u64 bytes = 0;

    if (!bytes_for_elements(dim_full_size(), &bytes))
        throw std::runtime_error("Invalid tensor block size");

    return bytes;
}

/** @brief Print tensor metadata to standard output. */
void VHGUFFTensor::dump_meta() const
{
    std::cout << "NAME: " << name;
    std::cout << " dims_size=" << dim_full_size() << " (" << dims.size() << ")";
    std::cout << " type=" << type;
    std::cout << " file_offset=" << offset << " " << addrhex(offset);
    std::cout << "\n";
}

/** @brief Print detailed tensor information. @param data_start Data section start. @param file_size Total file size. */
void VHGUFFTensor::dump(u64 data_start, u64 file_size) const
{
    if (offset > file_size - std::min(data_start, file_size))
        throw std::runtime_error("Tensor offset out of file bounds");

    u64 addr_file = data_start + offset;

    if (addr_file > file_size)
        throw std::runtime_error("Invalid absolute offset");

    // Name
    std::cout << "\n";
    std::cout << name;

    // Dimension
    std::cout << "\n  shape=[";

    for (size_t i = 0; i < dims.size(); ++i)
    {
        if (i)
            std::cout << ", ";
        std::cout << dims[i];
    }

    std::cout << "] ";

    // Props
    std::cout << "\n  ggml_type=" << type;
    std::cout << "\n  tensor_offset=" << offset;
    std::cout << "\n  file_offset=" << addr_file << " x" << addrhex(addr_file);

    u64 bsize = 0;
    if (bytes_for_elements(dim_full_size(), &bsize))
        std::cout << "\n  block_size=" << bsize;
    else
        std::cout << "\n  block_size=unsupported";

    std::cout << "\n";
}
