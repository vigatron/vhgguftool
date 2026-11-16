# vhgguftool

Language: C++17

Version: 0.1.0

A console tool for analyzing `.gguf` files (GGUF v2/v3) and extracting raw data from them.

`vhgguftool` works with a model file in read-only mode: it never modifies the source
`.gguf` file and only writes the data you explicitly ask it to extract.

## Features

Information display:

- General model information (Summary): GGUF version, tensor count, alignment,
  data section start, file size.
- Meta Info (metadata) list: key, value type, entry offset in the file, entry size
  and the value (for scalar and string types).
- Tensors list: name, shape, GGML type, offset inside the data section, absolute
  file offset and data block size in bytes.

Data extraction to a user-specified file:

- A single Meta Info entry value (raw value bytes).
- Tensor data, entirely or partially (layer range), for rank-1 and rank-2 tensors,
  as raw 16-bit words (e.g. BF16/F16).

## Command-line parameters

### General parameters and information display

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `-model-file` | File | — | Path to the `.gguf` model file. **Required** for every command except `--help` / `--version` |
| `-show-model-info` | Flag | off | Show general model information (Summary) |
| `-show-metadata-list` | Flag | off | Show Meta Info sections |
| `-show-tensors-list` | Flag | off | Show tensor parameters |
| `-show-all` | Flag | off | Show all information in detail: model-info, metadata-list, tensors-list |
| `-format` | String | `text` | Output format: `text` or `json` (*reserved*) |
| `--help` | Cmd | — | Show the command list and exit |
| `--version` | Cmd | — | Show program version and exit |

`-show-all` sets `-show-model-info`, `-show-metadata-list` and
`-show-tensors-list` at once; individual flags can be combined freely, so the
output order is always: model info → metadata → tensors.

### Meta Info extraction

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `-cmd-extract-metadata` | Cmd | — | Extract a Meta Info entry value |
| `-param-meta-key` | String | — | Exact name of the Meta Info key to extract |
| `-save-meta-file` | File | `meta_dump.bin` | Output file for the extracted Meta Info value |

The key must exist in the file (exact, case-sensitive match), otherwise the
command fails.

### Tensor extraction

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `-cmd-extract-tensordata` | Cmd | — | Extract tensor data |
| `-param-tensor-name` | String | — | Exact tensor name to extract (e.g. `token_embd.weight`) |
| `-param-tensor-laynum` | Int | `0` | Starting layer (accepted range `0..131072`) |
| `-param-tensor-laycnt` | Int | `1` | Number of layers to extract, `0` = whole tensor (accepted range `0..131072`) |
| `-save-tensor-file` | File | `tensor_dump.bin` | Output file for the extracted tensor data |

Only one command (`-cmd-extract-metadata` or `-cmd-extract-tensordata`) is executed
per run; when both are given, the metadata command wins.

## How it works

Every invocation (except `--help` / `--version`) reads the whole model structure
before executing a command:

1. Open the `.gguf` file (`-model-file`).
2. Parse and validate the header: `GGUF` magic, version (only 2 and 3 are
   supported), tensor count and metadata count (each must be ≤ 1 000 000).
3. Parse all metadata entries. If `general.alignment` is present, its value
   becomes the data alignment (default is 32).
4. Parse all tensor descriptors (name, rank ≤ 4, dimensions, GGML type, relative
   offset).
5. Calibrate: compute `data_start = align_up(current_position, alignment)` and read
   the file size.
6. Execute the requested display / extraction commands.

Because the whole structure is parsed anyway, any error in the container layout
(e.g. an unsupported metadata type) is reported before any request is served.

## Requirements

- CMake ≥ 3.16
- A C++17 compiler (the project is tested with GCC 14)
- Ninja (optional, but used by default in the reference build)
- Git, for cloning submodules

## Building

The project uses three Git submodules, so clone recursively (or initialize the
submodules afterwards):

```bash
git clone --recursive <repository-url> vhgguftool
cd vhgguftool

# if the repository was cloned without submodules:
git submodule update --init --recursive
```

Configure and build with CMake:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The executable is produced as `build/vhgguftool`.

Notes:

- `CMAKE_EXPORT_COMPILE_COMMANDS` is enabled, `build/compile_commands.json` is
  generated for IDE/clangd integration.
- The version is defined once in `CMakeLists.txt` (`project(... VERSION 0.1.0 ...)`)
  and is propagated to the code through the generated header
  `generated/version_vhgguftool.h` (template: `src/version.h.in`).
- `-show-model-info`, `-show-metadata-list`, `-show-tensors-list` and `-show-all`
  may be combined; `-show-all` is equivalent to enabling all three.
- Display commands print to standard output; extraction commands write files and
  print nothing on success.

## Output modes

- **`text`** (default) — human-readable text output to the console.
- **`json`** — JSON output (*reserved / not supported yet*). The option is accepted
  for forward compatibility with a future MCP/JSON mode, but currently the output
  format is always text.

Select the mode with `-format text|json`. Any value other than `json` is treated
as text, and `json` currently behaves exactly like `text`.

## Output format

### Model info (`-show-model-info`)

```
GGUF version: 3
Tensors: 335
Alignment: 32
Data start: 10962496
File size: 1557662528
```

- `Alignment` — value of `general.alignment` metadata, or 32 if absent.
- `Data start` — absolute file offset of the tensor data section (descriptors end,
  aligned up to the alignment).
- `File size` — size of the `.gguf` file in bytes.

### Meta Info list (`-show-metadata-list`)

```
*** META-Data Dump ***

KEY: general.architecture TYPE: 8 OFFSET: 24 SIZE: 46 VAL: qwen35
KEY: general.base_model.count TYPE: 4 OFFSET: 379 SIZE: 40 VAL: 1
KEY: qwen35.rope.freq_base TYPE: 6 OFFSET: 1005 SIZE: 37 VAL: 1e+07
KEY: general.tags TYPE: 9 OFFSET: 632 SIZE: 62 VAL: array<8>[1]
```

Columns:

- `KEY` — metadata key name.
- `TYPE` — GGUF value type code (see [GGUF type codes](#gguf-type-codes)).
- `OFFSET` — absolute offset of the entry start in the file (the key length prefix).
- `SIZE` — total size of the entry in bytes (key + type + value).
- `VAL` — value: numbers, `true`/`false`, string contents, or
  `array<element_type>[count]` for arrays. Array elements themselves are skipped
  while parsing and are not printed.

If a file has no metadata entries, `(no metadata entries)` is printed.

### Tensors list (`-show-tensors-list`)

```
*** Tensors Info Table ***

token_embd.weight
  shape=[1024, 248320]
  ggml_type=30
  tensor_offset=0
  file_offset=10962496 xa74640
  block_size=508559360
```

Fields per tensor:

- `shape` — dimensions in GGML order (`ne[0]` first, i.e. the fastest-varying
  dimension).
- `ggml_type` — GGML type code (30 = BF16, 0 = F32, 1 = F16, quantized types
  and so on; see `src/mods/gguf/ggml_type.hpp`).
- `tensor_offset` — offset of the tensor data relative to `data_start`
  (as stored in the GGUF file).
- `file_offset` — absolute offset in the file (`data_start + tensor_offset`),
  followed by its hexadecimal form.
- `block_size` — tensor data size in bytes (`elements × type_size`).

Out-of-bounds tensor offsets are rejected with an error instead of being printed.

## Data extraction

### Meta Info extraction

`-cmd-extract-metadata -param-meta-key <key> -save-meta-file <file>` writes the
raw value bytes of the requested metadata entry:

- `STRING` → plain text without the `u64` length prefix (no trailing NUL).
- Scalars (`UINT8` … `FLOAT64`) → the raw little-endian value bytes.
- `ARRAY` → the raw array payload *including* the element-type/count header
  (elements are stored as-is, little-endian).

No conversion or reformatting is performed — the output is exactly the bytes that
follow the entry's type field in the `.gguf` file.

### Tensor extraction

`-cmd-extract-tensordata -param-tensor-name <name> -param-tensor-laynum <from>
-param-tensor-laycnt <count> -save-tensor-file <file>` copies raw 16-bit words
(little-endian, e.g. BF16 or F16) from the tensor data block:

- A "layer" is `shape[0]` elements (one row of a rank-2 tensor).
- `-param-tensor-laynum` — index of the first layer.
- `-param-tensor-laycnt` — number of layers to copy; `0` means "the whole tensor".
- Output size = `layers × shape[0] × 2` bytes; a rank-1 tensor requires
  `-param-tensor-laynum 0`.

Restrictions (the command fails if violated):

- The tensor name must exist (exact, case-sensitive match).
- Tensor rank must be 1 or 2.
- For rank-2 tensors `laynum + laycnt` must not exceed `shape[1]`.
- The whole requested range must be readable; a short read is reported as an error
  and no partial data is written.

No dequantization is performed: quantized tensors are copied as raw words, so the
command is intended for `BF16` / `F16` model files (and for inspecting raw
storage layout in general).

## Usage examples

```bash
# Show general model information and metadata
vhgguftool -model-file model.gguf -show-model-info -show-metadata-list

# Show all information in detail (model, metadata, tensors)
vhgguftool -model-file model.gguf -show-all

# Show tensor parameters only
vhgguftool -model-file model.gguf -show-tensors-list

# Extract a Meta Info value (e.g. the chat template, plain text)
vhgguftool -model-file model.gguf -cmd-extract-metadata \
    -param-meta-key tokenizer.chat_template -save-meta-file chat_template.txt

# Extract a binary Meta Info value (a license link, raw value bytes)
vhgguftool -model-file model.gguf -cmd-extract-metadata \
    -param-meta-key general.license.link -save-meta-file meta.bin

# Extract 4 layers of a tensor, starting from layer 3
vhgguftool -model-file model.gguf -cmd-extract-tensordata \
    -param-tensor-name blk.0.attn_norm.weight \
    -param-tensor-laynum 3 -param-tensor-laycnt 4 \
    -save-tensor-file dump3.bin

# Extract a whole tensor
vhgguftool -model-file model.gguf -cmd-extract-tensordata \
    -param-tensor-name token_embd.weight -param-tensor-laycnt 0 \
    -save-tensor-file token_embd.bin

# Show command list / program version
vhgguftool --help
vhgguftool --version
```

## Exit codes

| Code | Meaning |
|------|---------|
| `0` | Success (including `--help` / `--version`) |
| `1` | CLI parsing error, missing `-model-file`, or an uncaught exception (invalid/unsupported GGUF layout) |
| `2` | Model scan failed (open, header, metadata, tensors or calibration) |
| `3` | Requested command failed (key/tensor not found, read or write error) |

Diagnostic messages are printed to standard output by the error-reporting helpers
(and exceptions are printed to standard error by the `main` handler).

## GGUF type codes

Metadata value types printed in the `TYPE` column:

| Code | Type | Code | Type |
|------|------|------|------|
| 0 | `UINT8` | 7 | `BOOL` |
| 1 | `INT8` | 8 | `STRING` |
| 2 | `UINT16` | 9 | `ARRAY` |
| 3 | `INT16` | 10 | `UINT64` |
| 4 | `UINT32` | 11 | `INT64` |
| 5 | `INT32` | 12 | `FLOAT64` |
| 6 | `FLOAT32` | | |

Tensor `ggml_type` codes follow the GGML enumeration (`ggml_type` in
`src/mods/gguf/ggml_type.hpp`), e.g. `0` = `F32`, `1` = `F16`, `30` = `BF16`,
`12` = `Q4_K`, `39` = `MXFP4`.

## Project structure

```
vhgguftool/
├── CMakeLists.txt              build configuration, version, targets
├── README.md
├── AGENTS.md                   project notes for AI agents
├── model/                      local test models (git-ignored symlinks)
├── src/
│   ├── main.cpp/.hpp           entry point, scan and command dispatch
│   ├── cli.cpp/.hpp            CLI description and parsing (vhlibargs)
│   ├── visual.cpp/.hpp         bit-field visualization, sign extraction helpers
│   ├── cmd_show_signs_map.cpp  reserved experiment: 2D sign maps
│   ├── aiplatform.hpp          common types, GGUF value type codes
│   ├── version.h.in            version header template
│   ├── mods/
│   │   ├── cliparams.hpp       parsed CLI arguments structure
│   │   ├── texttools/          hex/bf16 formatting helpers
│   │   └── gguf/
│   │       ├── vhgguf.*        GGUF container reader and dumps
│   │       ├── vhggufmetainfo.*  metadata entry descriptor
│   │       ├── vhgguftensor.*    tensor descriptor
│   │       ├── vhggufio.hpp      little-endian primitives, GGUF strings
│   │       ├── vhggufparam.hpp   legacy metadata container
│   │       └── ggml_type.hpp     GGML tensor type enumeration
│   └── extmods/                external submodules (read-only)
└── build/                      CMake build directory (git-ignored)
```

## Dependencies and submodules

External libraries live in `src/extmods` and are included as Git submodules
(read-only, never edited by this project):

| Submodule | Purpose |
|-----------|---------|
| `vhlibplatform` | base types (`verr`, `u8/u32/u64`), error reporting, bit helpers |
| `vhlibargs` | command-line parser (`VHCmdLineParser`) |
| `vhlibfsys` | binary file I/O (`VHFile`) |

The commented-out `vhlibsvg` submodule is currently not used.

## Limitations

- `json` output mode is reserved and not implemented yet; all output is text.
- Array values in the metadata list are shown as `array<type>[count]` without
  elements.
- Metadata extraction returns raw bytes, not converted values; for arrays the
  element-type/count header is included.
- Tensor extraction supports only rank-1 and rank-2 tensors, does not dequantize,
  and interprets data as 16-bit words.
- `-param-tensor-laycnt 0` means "whole tensor"; combining it with a non-zero
  `-param-tensor-laynum` is not a defined layer range.
- Only one command is executed per run; display flags can be combined with only
  one extraction command in practice.
- `cmd_show_signs_map` / `visual.cpp` contain experimental helpers that are not
  wired to the command line yet.
- The built-in `--help` text for `-show-tensors-list` currently says
  "show all information in detail"; the actual behavior is documented above
  (tensor list only).

## License

No license file is currently included in the repository.

(C) 2026 Viktor Glebov / V01G04A81
