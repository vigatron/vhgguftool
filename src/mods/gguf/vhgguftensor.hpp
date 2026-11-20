#pragma once

#include "aiplatform.hpp"

#include "ggml_type.hpp"

#include "texttools/texttools.hpp"

/**
 * @brief Descriptor of a single tensor stored in a GGUF file.
 */
class VHGUFFTensor
{
public:
    std::string name;   ///< Tensor name.
    std::vector<u64> dims; ///< Tensor dimensions.
    u32 type;           ///< GGML type code of the tensor.
    u64 offset;         ///< Offset of tensor data relative to the data section start.

    /**
     * @brief Calculate the total number of elements in the tensor.
     * @return Number of elements.
     */
    u64 dim_full_size() const;

    /**
     * @brief Get the size in bytes of a single tensor element.
     * @details Applicable to non-blocked types only. For blocked (quantized)
     *          types the size is not an integer number of bytes per element,
     *          see type_block_layout() and bytes_for_elements().
     * @return Element size in bytes, or 0 for blocked types.
     */
    u64 type_size_bytes() const;

    /**
     * @brief Get the short name of a GGML tensor type.
     * @param type GGML tensor type code.
     * @return Type name (e.g. "F32", "BF16", "Q4_K"), or "UNKNOWN".
     */
    static std::string type_name(u32 type);

    /**
     * @brief Get the block layout of the tensor type.
     * @details For non-blocked types the block contains one element.
     * @param block_elems Output number of elements per block.
     * @param block_bytes Output number of bytes per block.
     * @return `true` if the tensor type is supported, `false` otherwise.
     */
    bool type_block_layout(u64 *block_elems, u64 *block_bytes) const;

    /**
     * @brief Calculate the size in bytes for the given number of elements.
     * @param elems Number of tensor elements.
     * @param bytes Output size in bytes.
     * @return `true` on success, `false` if the type is unsupported,
     *         the element count is not block-aligned or the size overflows.
     */
    bool bytes_for_elements(u64 elems, u64 *bytes) const;

    /**
     * @brief Get the size of one dimension in elements.
     * @param idx Dimension index.
     * @return Dimension size.
     */
    u64 dim_size_part(uint8_t idx) const;

    /**
     * @brief Get the relative offset of the tensor data block.
     * @return Relative block offset.
     */
    u64 block_pos() const;

    /**
     * @brief Get the size in bytes of a single dimension.
     * @param idx Dimension index.
     * @return Dimension size in bytes.
     * @throws std::runtime_error if the type is unsupported or the dimension is not block-aligned.
     */
    u64 block_size_part(uint8_t idx) const;

    /**
     * @brief Get the total size in bytes of the tensor data.
     * @return Total tensor data size in bytes.
     * @throws std::runtime_error if the type is unsupported or the element count is not block-aligned.
     */
    u64 block_size_full() const;

    /**
     * @brief Print tensor metadata to standard output.
     */
    void dump_meta() const;

    /**
     * @brief Print detailed tensor information including file offsets.
     * @param data_start Start offset of the tensor data section.
     * @param file_size Total size of the GGUF file.
     */
    void dump(u64 data_start, u64 file_size) const;

private:
};