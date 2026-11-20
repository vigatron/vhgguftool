#pragma once

#include "vhplatform.hpp"

// Структура для удобной передачи аргументов в логику приложения
struct CLIArgs
{
    CLIArgs() { }

    bool PartialTensorExtraction() const {
        return param_tensor_only_s || param_tensor_only_e || param_tensor_only_m;
    }

    //
    std::string modelFileName;

    //
    std::string cmd;

    bool showInfo = false;
    bool showMeta = false;
    bool showTensors = false;
    bool showAll = false;

    bool cmdGUI = false;
    bool cmdExtractMeta = false;
    bool cmdExtractTensor = false;
    bool cmdVersion = false;

    // Extract *Meta* Info
    std::string meta_key;  // Key MetaInfo для извлечения

    // Файл сохранения MetaInfo
    std::string saveMetaFile;

    // Extract *Tensor* Data
    std::string tensor_name;  // Имя Tensor для извлечения
    size_t tensor_layer_from;   // Starting Layer
    size_t tensor_layer_count;  // Layers Count (0 = all)

    bool param_tensor_only_s = false;
    bool param_tensor_only_e = false;
    bool param_tensor_only_m = false;

    // Файл сохранения TensorData
    std::string saveTensorFile;
};
