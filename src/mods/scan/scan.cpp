#include "scan.hpp"

/**
 * @brief Open and fully parse the GGUF model structure.
 * @details Opens the model file and runs the full scan pipeline:
 *          header, metadata, tensors and calibration. Every stage must
 *          succeed, otherwise the first failing stage's error code is returned.
 * @param gguf GGUF reader instance to populate.
 * @param args Parsed CLI arguments (provides the model file path).
 * @return `vok` on success, error code otherwise.
 */
verr Scan_GGUF_File(VHGGUF & gguf, const CLIArgs &args)
{

    if (vok != gguf.open(args.modelFileName))
        return verrmsg(1, "Invalid Model file " + std::string(args.modelFileName));

    if (vok != gguf.ProcessHeader())
        return verrmsg(2, "Invalid GGUF Header File");

    if (vok != gguf.ProcessMetadata())
        return verrmsg(3, "ProcessMetadata failed");

    if (vok != gguf.ProcessTensors())
        return verrmsg(4, "ProcessTensors failed");

    if (vok != gguf.Calibrate())
        return verrmsg(5, "Calibration failed");

    return vok;
}