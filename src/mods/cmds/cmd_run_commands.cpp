#include "cmd_run_commands.hpp"

#include "mods/misc/visual.hpp"

#include "vhfsys.hpp"

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
        ShowBitField2D(arru8, 256);

        return vok;
    }

    return vok;
}