#pragma once

#include "aiplatform.hpp"
#include "cliparams.hpp"

/** @brief Parse command line arguments. @param args Output arguments. @param argc Argument count. @param argv Argument values. @return Result code. */
verr ParseCLI(CLIArgs &args, int argc, char **argv);
