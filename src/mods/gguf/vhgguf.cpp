#include "vhgguf.hpp"

/** @brief Default constructor. */
VHGGUF::VHGGUF() {}

/** */
verr VHGGUF::open(const std::string &fname)
{
    file.assign(fname);
    return file.openfile() ? vok : verror(1);
}

/** */
verr VHGGUF::ProcessHeader()
{
    // Header
    char magic[4];

    if (!file.read(magic, 4))
        return verrmsg(1, "Cannot read GGUF header");

    if (std::string(magic, 4) != "GGUF")
        throw std::runtime_error("Not a GGUF file");

    gguf_version = VHGGUFIO::read_u32(file);
    tensor_count = VHGGUFIO::read_u64(file);
    metadata_count = VHGGUFIO::read_u64(file);

    if (gguf_version < 2 || gguf_version > 3)
        return verrmsg(1, "Unsupported GGUF version");

    if (tensor_count > 1000000 || metadata_count > 1000000)
        return verrmsg(2, "Invalid GGUF counts");

    return vok;
}

/** */
u64 VHGGUF::get_metadata_count() const
{
    return metadata_count;
}

/** */
u64 VHGGUF::get_tensor_count() const
{
    return tensor_count;
}

/** */
u64 VHGGUF::get_data_start() const
{
    return data_start;
}

/** */
verr VHGGUF::ProcessMetadata()
{
    arrmeta.clear();
    arrmeta.reserve(static_cast<size_t>(get_metadata_count()));

    // META-Data
    for (u64 i = 0; i < get_metadata_count(); ++i)
    {
        u64 curfilepos;
        if (!file.pos(&curfilepos))
            return verror(1);

        // std::cout << "@" << addrhex(curfilepos) << ": ";

        VHGUFFMetaInfo meta;
        if (vok != meta.readmeta(file))
            return verror(1);

        // Keep alignment info
        if (meta.key() == "general.alignment" && meta.type() == UINT32)
        {
            alignment = meta.value_u32();
        }

        // meta.dump();

        arrmeta.push_back(std::move(meta));
    }

    return vok;
}

/** */
verr VHGGUF::ProcessTensors()
{
    arrtensors.reserve(static_cast<size_t>(get_tensor_count()));

    for (u64 i = 0; i < tensor_count; ++i)
    {
        u64 curfilepos;
        if (!file.pos(&curfilepos))
            return verror(1);

        // std::cout << "@" << addrhex(curfilepos) << ": ";

        VHGUFFTensor t;

        t.name = VHGGUFIO::read_string(file);

        // Read dimension
        u32 n_dims = VHGGUFIO::read_u32(file);
        if (n_dims > 4)
            throw std::runtime_error("Unexpected tensor rank");
        for (u32 d = 0; d < n_dims; ++d)
            t.dims.push_back(VHGGUFIO::read_u64(file));

        // Read type
        t.type = VHGGUFIO::read_u32(file);
        t.offset = VHGGUFIO::read_u64(file);

        // t.dump_meta();

        arrtensors.push_back(std::move(t));
    }

    return vok;
}

/** */
verr VHGGUF::Calibrate()
{
    // Start of data
    u64 cur_pos;

    if (!file.pos(&cur_pos))
        return verror(1);

    data_start = align_up(cur_pos, alignment);

    u64 fsize;
    if (!file.filesize(&fsize))
        return verror(1);

    file_size = fsize;

    return vok;
}

/** */
u64 VHGGUF::align_up(u64 x, u64 a)
{
    if (!a || (a & (a - 1)))
        throw std::runtime_error("Invalid alignment");
    if (x > std::numeric_limits<u64>::max() - (a - 1))
        throw std::runtime_error("Offset overflow");
    return (x + a - 1) & ~(a - 1);
}

/** */
const VHGUFFMetaInfo &VHGGUF::GetMeta(std::size_t idx) const
{
    return arrmeta[idx];
}

/** */
const VHGUFFTensor &VHGGUF::GetTensor(std::size_t idx) const
{
    return arrtensors[idx];
}

/** @brief Find a metadata entry by key. @return true on success. */
bool VHGGUF::FindMetaID(const std::string &key, size_t *presult) const
{
    for (size_t idx = 0; idx < arrmeta.size(); idx++)
    {
        if (arrmeta[idx].key() == key)
        {
            *presult = idx;
            return true;
        }
    }

    return false;
}

/** @brief Read raw bytes of a metadata entry value from the GGUF file. @return true on success. */
bool VHGGUF::ReadMetaData(std::size_t idx, std::vector<uint8_t> &out)
{
    const VHGUFFMetaInfo &meta = arrmeta[idx];

    if (!file.seek(meta.file_offset()))
        return false;

    // Value offset inside the entry:
    // key  -> u64 length + key bytes  (8 + key length)
    // type -> u32                       (4)
    const u64 value_off = 8 + static_cast<u64>(meta.key().size()) + 4;
    const u64 value_size = meta.byte_size() - value_off;

    out.clear();

    if (value_size == 0)
        return true;

    out.resize(static_cast<std::size_t>(value_size));

    if (!file.read(out.data(), static_cast<std::size_t>(value_size), meta.file_offset() + value_off))
        return false;

    // STRING value: only the text content is taken (u64 length prefix is skipped)
    if (meta.type() == STRING)
    {
        if (value_size > 8)
            out.erase(out.begin(), out.begin() + 8);
        else
            out.clear();
    }

    // For other types (scalars, arrays) the whole value bytes are taken

    return true;
}

/** @brief Read raw tensor data bytes (type-aware, block-aligned). */
std::vector<uint8_t> VHGGUF::ReadTensorBytes(
    const VHGUFFTensor &tensor,
    size_t from_layer,
    size_t layers_count)
{
    std::vector<uint8_t> r;

    // Nothing or not supported
    if (!tensor.dims.size() || tensor.dims.size() > 2)
        return r;

    // Bytes of one row (the first dimension)
    u64 row_bytes = 0;
    if (!tensor.bytes_for_elements(tensor.dims[0], &row_bytes))
        return r;

    u64 from = static_cast<u64>(from_layer);
    u64 count = 0;

    if (tensor.dims.size() == 1)
    {
        // One-dimensional tensor: only the whole tensor can be read
        if (from_layer != 0)
            return r;
        count = 1;
    }
    else
    {
        const u64 rows = tensor.dims[1];

        if (from > rows)
            return r;

        count = layers_count ? static_cast<u64>(layers_count) : (rows - from);

        if (count > rows - from)
            return r;
    }

    // Total size with overflow checks
    if (row_bytes != 0 && count > std::numeric_limits<u64>::max() / row_bytes)
        return r;

    const u64 total_bytes = row_bytes * count;

    if (row_bytes != 0 && from > std::numeric_limits<u64>::max() / row_bytes)
        return r;

    u64 blockoffs = get_data_start() + tensor.block_pos();
    blockoffs += from * row_bytes;

    return ReadDataBlock(blockoffs, total_bytes);
}

/** @brief Read tensor content as BF16 (16-bit) values. */
std::vector<uint16_t> VHGGUF::ReadTensorContent(
    const VHGUFFTensor &tensor,
    size_t from_layer,
    size_t layers_count)
{
    std::vector<uint16_t> r;

    // Only 16-bit element types can be represented as uint16
    if (tensor.type_size_bytes() != sizeof(uint16_t))
        return r;

    const std::vector<uint8_t> bytes = ReadTensorBytes(tensor, from_layer, layers_count);

    if (bytes.size() % sizeof(uint16_t))
        return r;

    r.resize(bytes.size() / sizeof(uint16_t));

    if (!bytes.empty())
        std::memcpy(r.data(), bytes.data(), bytes.size());

    return r;
}

/** @brief Read a block of raw bytes from the file. */
std::vector<uint8_t> VHGGUF::ReadDataBlock(u64 blockoffs, u64 bytes)
{
    std::vector<uint8_t> r;

    // empty request
    if (bytes == 0)
        return r;

    // The number of bytes must fit into size_t
    if (bytes > std::numeric_limits<std::size_t>::max())
        return r;

    if (!file.seek(blockoffs))
        return r;

    r.resize(static_cast<std::size_t>(bytes));

    if (!file.read(r.data(), r.size()))
    {
        r.clear();
        return r;
    }

    return r;
}

/** @brief Read a block of BF16 values from the file. */
std::vector<uint16_t> VHGGUF::ReadDataBlockBF16(u64 blockoffs, u64 count)
{
    std::vector<uint16_t> r;

    // The number of bytes must fit into size_t
    if (count > std::numeric_limits<std::size_t>::max() / sizeof(uint16_t))
        return r;

    const std::vector<uint8_t> bytes =
        ReadDataBlock(blockoffs, count * static_cast<u64>(sizeof(uint16_t)));

    if (bytes.size() % sizeof(uint16_t))
        return r;

    r.resize(bytes.size() / sizeof(uint16_t));

    if (!bytes.empty())
        std::memcpy(r.data(), bytes.data(), bytes.size());

    return r;
}

/** */
bool VHGGUF::FindTensorID(const std::string &keydesc, size_t *presult) const
{
    for (size_t idx = 0; idx < arrtensors.size(); idx++)
    {
        if (arrtensors[idx].name == keydesc)
        {
            *presult = idx;
            return true;
        }
    }

    return false;
}

/** */
void VHGGUF::dump_gguf_info()
{
    // Info
    std::cout << "GGUF version: " << gguf_version;
    std::cout << "\nTensors: " << tensor_count;
    std::cout << "\nAlignment: " << alignment;
    std::cout << "\nData start: " << data_start;
    std::cout << "\nFile size: " << file_size << "\n";
}

/**
 * @brief Print all metadata entries.
 */
void VHGGUF::dump_metainfo()
{
    std::cout << "\n";
    std::cout << "\n*** META-Data Dump ***\n";
    std::cout << "\n";

    if (arrmeta.empty())
    {
        std::cout << "(no metadata entries)\n";
        return;
    }

    for (const auto &meta : arrmeta)
    {
        meta.dump();
    }
}

/** */
void VHGGUF::dump_tensors()
{
    std::cout << "\n";
    std::cout << "*** Tensors Info Table ***\n";
    std::cout << "\n";

    for (const auto &t : arrtensors)
    {
        t.dump(data_start, file_size);
    }
}
