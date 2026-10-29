#pragma once

#include "aiplatform.hpp"

#include "vhggufparam.hpp"

#include "vhggufmetainfo.hpp"
#include "vhgguftensor.hpp"

#include "texttools/texttools.hpp"

/**
 * @brief Reader and analyzer for GGUF model files.
 */
class VHGGUF
{

public:
    /** @brief Default constructor. */
    VHGGUF();

    /**
     * @brief Open a GGUF file.
     * @param fname Path to the GGUF file.
     * @return `vok` on success, error code otherwise.
     */
    verr open(const std::string &fname);

    /**
     * @brief Parse and validate the GGUF header.
     * @return `vok` on success, error code otherwise.
     */
    verr ProcessHeader();

    /** @brief Get the number of metadata entries. @return Metadata count. */
    u64 get_metadata_count() const;

    /** @brief Get the number of tensors. @return Tensor count. */
    u64 get_tensor_count() const;

    /** @brief Get the start offset of the tensor data section. @return Data start offset. */
    u64 get_data_start() const;


    /**
     * @brief Parse all metadata entries.
     * @return `vok` on success, error code otherwise.
     */
    verr ProcessMetadata();

    /**
     * @brief Parse all tensor descriptors.
     * @return `vok` on success, error code otherwise.
     */
    verr ProcessTensors();

    /**
     * @brief Calibrate data start offset and file size.
     * @return `vok` on success, error code otherwise.
     */
    verr Calibrate();

    /**
     * @brief Align a value up to the given alignment.
     * @param x Value to align.
     * @param a Alignment (power of two).
     * @return Aligned value.
     */
    u64 align_up(u64 x, u64 a);


    /**
     * @brief Get a tensor by index.
     * @param idx Tensor index.
     * @return Reference to the tensor.
     */
    const VHGUFFTensor &GetTensor(std::size_t idx) const;

    /**
     * @brief Get a metadata entry by index.
     * @param idx Metadata index.
     * @return Reference to the metadata entry.
     */
    const VHGUFFMetaInfo &GetMeta(std::size_t idx) const;

    /**
     * @brief Find a metadata entry by key.
     * @param key Metadata key to search for.
     * @param presult Output index of the found metadata entry.
     * @return `true` if found, `false` otherwise.
     */
    bool FindMetaID(const std::string &key, size_t *presult) const;

    /**
     * @brief Read raw bytes of a metadata entry value from the GGUF file.
     * @param idx Metadata entry index.
     * @param out Output buffer with metadata entry value bytes.
     * @return `true` on success, `false` otherwise.
     */
    bool ReadMetaData(std::size_t idx, std::vector<uint8_t> &out);

    /**
     * @brief Read raw tensor data bytes.
     * @details The read region is aligned to whole rows (the first dimension)
     *          and to whole quantization blocks. For 1-D tensors the whole
     *          tensor is read regardless of `layers_count`.
     * @param tensor Tensor descriptor.
     * @param from_layer Starting layer (row) index.
     * @param layers_count Number of layers (rows) to read (0 = all remaining).
     * @return Vector of the read bytes (empty on error).
     */
    std::vector<uint8_t> ReadTensorBytes(
        const VHGUFFTensor & tensor,
        size_t from_layer,
        size_t layers_count);

    /**
     * @brief Read tensor content as BF16 (16-bit) values.
     * @details Only 16-bit element types (BF16, F16) are supported; for other
     *          types an empty vector is returned, use ReadTensorBytes() instead.
     * @param tensor Tensor descriptor.
     * @param from_layer Starting layer index.
     * @param layers_count Number of layers to read (0 = all).
     * @return Vector of read 16-bit values.
     */
    std::vector<uint16_t> ReadTensorContent(
        const VHGUFFTensor & tensor,
        size_t from_layer,
        size_t layers_count);

    /**
     * @brief Read a block of raw bytes from the file.
     * @param blockoffs Absolute file offset of the block.
     * @param bytes Number of bytes to read.
     * @return Vector of the read bytes (empty on error or empty request).
     */
    std::vector<uint8_t> ReadDataBlock(u64 blockoffs, u64 bytes);

    /**
     * @brief Read a block of BF16 values from the file.
     * @param blockoffs Absolute file offset of the block.
     * @param count Number of BF16 values to read.
     * @return Vector of read values.
     */
    std::vector<uint16_t> ReadDataBlockBF16(u64 blockoffs, u64 count);

    /**
     * @brief Find a tensor by name.
     * @param keydesc Tensor name.
     * @param presult Output index of the found tensor.
     * @return `true` if found, `false` otherwise.
     */
    bool FindTensorID(const std::string &keydesc, size_t *presult) const;

    /**
     * @brief Print general GGUF information to standard output.
     */
    void dump_gguf_info();

    /**
     * @brief Print all metadata entries.
     */
    void dump_metainfo();

    /**
     * @brief Print information for all tensors.
     */
    void dump_tensors();


private:
    //
    VHFile file;

    // GGUF
    u32 gguf_version;
    u64 metadata_count;
    u64 tensor_count;
    u64 alignment = 32;

    u64 data_start;
    u64 file_size;

    // MetaInfo array
    std::vector<VHGUFFMetaInfo> arrmeta;

    // Tensors array
    std::vector<VHGUFFTensor> arrtensors;
};