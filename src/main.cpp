#include "main.hpp"
#include "visual.hpp"
#include "cmd_show_signs_map.hpp"

#include "vhfsys.hpp"


/**
 * 
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

/**
 * @brief Execute application logic.
 * @param args Parsed CLI arguments.
 * @return Result code.
 * */
verr RunCommand(VHGGUF & gguf, const CLIArgs &args)
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

    // cmd Extract Meta Info
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

    //
    if(args.cmdExtractTensor) {

        size_t tidx;
        if (!gguf.FindTensorID(args.tensor_name, &tidx))
            return verrmsg(1, "Tensor data not found: " + args.tensor_name);

        const VHGUFFTensor &tensor = gguf.GetTensor(tidx);

        std::vector<uint8_t> data = gguf.ReadTensorBytes(tensor, args.tensor_layer_from, args.tensor_layer_count);
        if(!data.size())
            return verrmsg(2, "Cant extract tensor layers");

        // Save Result
        VHFile outfile(args.saveTensorFile);
        if (!outfile.write(data.data(), data.size()))
            return verrmsg(2, "Cant save tensor data to file: " + args.saveTensorFile);


        //
        // cmdShowSignsMap(data, 32, 64);
        ShowBitField2D(data, 16);

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
