#include "main.hpp"
#include "visual.hpp"
#include "cmd_show_signs_map.hpp"

#include "vhfsys.hpp"


/**
 * @brief Open and fully parse the GGUF model structure.
 * @details Opens the model file and runs the full scan pipeline:
 *          header, metadata, tensors and calibration. Every stage must
 *          succeed, otherwise the first failing stage's error code is returned.
 * @param gguf GGUF reader instance to populate.
 * @param args Parsed CLI arguments (provides the model file path).
 * @return `vok` on success, error code otherwise.
 */
static verr Scan_GGUF_File(VHGGUF & gguf, const CLIArgs &args)
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

/**
 * @brief Execute application logic.
 * @param args Parsed CLI arguments.
 * @return Result code.
 * */
static verr RunCommand(VHGGUF & gguf, const CLIArgs &args)
{

    // cmd Show GGUF Info
    if(args.showInfo)
        gguf.dump_gguf_info();

    // cmd Show MetaInfo ?
    if(args.showMeta)
        gguf.dump_metainfo();

    // cmd Show Tensors ?
    if(args.showTensors)
        gguf.dump_tensors();

    // Command: Extract Meta Info
    if(args.cmdExtractMeta) {

        size_t midx;
        if (!gguf.FindMetaID(args.meta_key, &midx))
            return verrmsg(1, "Meta info not found: " + args.meta_key);

        std::vector<uint8_t> data;
        if (!gguf.ReadMetaData(midx, data))
            return verrmsg(2, "Cant extract meta info");

        // Save Result
        VHFile outfile(args.saveMetaFile);
        if (!outfile.write(reinterpret_cast<const uint8_t*>(data.data()), data.size()))
            return verrmsg(2, "Cant save meta info to file: " + args.saveMetaFile);

        return vok;
    }

    // Command: Extract tensor data array
    if(args.cmdExtractTensor) {

        size_t tidx;
        if (!gguf.FindTensorID(args.tensor_name, &tidx))
            return verrmsg(1, "Tensor data not found: " + args.tensor_name);

        //
        const VHGUFFTensor &tensor = gguf.GetTensor(tidx);

        std::vector<uint8_t> arru8;

        if(args.PartialTensorExtraction()) { 

            std::vector<uint16_t> arru16 = gguf.ReadTensorContent(
                tensor,
                args.tensor_layer_from,
                args.tensor_layer_count);

            arru8.resize(arru16.size());

            // Extract fields before saving?
            if(args.param_tensor_only_s) {

                for(size_t i=0;i<arru16.size();i++)
                    arru8[i] = arru16[i]>>15;

            } else if(args.param_tensor_only_e) {

                for(size_t i=0;i<arru16.size();i++)
                    arru8[i] = (arru16[i]>>7) & 0xFF;

            } else if(args.param_tensor_only_m) {

                for(size_t i=0;i<arru16.size();i++)
                    arru8[i] = arru16[i] & 0x7F;

            }

        } else {

            arru8 = gguf.ReadTensorBytes(
                tensor,
                args.tensor_layer_from,
                args.tensor_layer_count);

        }


        if(!arru8.size())
            return verrmsg(2, "Cant extract tensor layers");


        // Save Result
        VHFile outfile(args.saveTensorFile);

        if (!outfile.write(arru8.data(), arru8.size()))
            return verrmsg(2, "Cant save tensor data to file: " + args.saveTensorFile);

        // Misc
        // cmdShowSignsMap(data, 32, 64);
        // ShowBitField2D(data, 16);

        return vok;
    }

    return vok;
}


/** 
 * @brief Program entry point.
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Exit code.
 * */
int main(int argc, char **argv)
{
    VHGGUF gguf;
    CLIArgs args;

    try
    {
        // Parse Command Line
        if (vok != ParseCLI(args, argc, argv))
            return verrmsg(1, "Error: parsing CLI");

        // App Info Only
        if(args.cmdVersion)
            return vok;

         // Analise GGUF
        if(vok != Scan_GGUF_File(gguf, args))
            return verrmsg(2, "Error: .gguf model scan failed");

        // run application
        if(vok != RunCommand(gguf, args))
            return verrmsg(3, "RunCommand() failed");

        // Success !
        return vok;
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}
