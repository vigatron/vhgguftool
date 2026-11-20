#pragma once

#include "gguf/vhgguf.hpp"
#include "mods/cli/cli.hpp"

/** @brief Execute application logic. @param args Parsed CLI arguments. @return Result code. */
verr RunCommand(VHGGUF & gguf, const CLIArgs &args);