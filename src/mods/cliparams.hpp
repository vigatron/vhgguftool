#pragma once

#include "vhplatform.hpp"

// Структура для удобной передачи аргументов в логику приложения
struct CLIArgs
{
    std::string modelFileName;
    std::string format;

    bool showInfo = false;
    bool showMeta = false;
    bool showTensors = false;
    bool showAll = false;

    bool cmdExtractMeta = false;
    bool cmdExtractTensor = false;
    bool cmdVersion = false;

    // Extract Meta Info
    std::string meta_key;  // Key MetaInfo для извлечения

    // Extract Tensor Data
    std::string tensor_name;  // Имя Tensor для извлечения
    size_t tensor_layer_from;   // Starting Layer
    size_t tensor_layer_count;  // Layers Count (0 = all)

    // Файл сохранения MetaInfo
    std::string saveMetaFile;
    
    // Файл сохранения TensorData
    std::string saveTensorFile;
};
