#pragma once

#include "gguf/vhgguf.hpp"
#include "mods/cli/cli.hpp"

/** @brief Open and fully parse the GGUF model structure. @param gguf GGUF reader instance to populate. @param args Parsed CLI arguments (provides the model file path). @return `vok` on success, error code otherwise. */
verr Scan_GGUF_File(VHGGUF & gguf, const CLIArgs &args);