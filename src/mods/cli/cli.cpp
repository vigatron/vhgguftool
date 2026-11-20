#include "cli.hpp"
#include "argsparser.hpp"
#include "version_vhgguftool.h"

using namespace VHArgsParser;

/**
 * 
 */
static std::vector<VHArgTypeBase> KeysList()
{
    // Конфигурация ключей
    std::vector<VHArgTypeBase> keys = {

        // Обязательный параметр
        VHCLIArgFile("-model-file", "", "Путь к .gguf файлу модели"),

        // Вывод информации о содержимом GGUF
        VHCLIArgOpt("-show-model-info", false, "Показать общую информацию о модели (Summary)"),
        VHCLIArgOpt("-show-metadata-list", false, "Показать разделы Meta Info"),
        VHCLIArgOpt("-show-tensors-list", false, "Показать всю информацию подробно: model-info, metadata-list, tensors-list"),
        VHCLIArgOpt("-show-all", false, "показать всю информацию подробно"),

        // Команды
        VHCLIArgCmd("-gui", "Запустить графическую оболочку"),
        VHCLIArgCmd("-cmd-extract-metadata", "Извлечь раздел Meta Info"),
        VHCLIArgCmd("-cmd-extract-tensordata", "Извлечь слой Tensors"),
        VHCLIArgCmd("--help", "Help"),
        VHCLIArgCmd("--version", "Версия программы"),

        // *** Parameters: Extract Meta Info / Извлечение Meta Info
        VHCLIArgStr("-param-meta-key", "", "Ключ Meta Info для извлечения"),

        // *** Parameters: Extract Tensor / Извлечение Tensors
        VHCLIArgStr("-param-tensor-name", "", "Имя Tensor для извлечения"),
        VHCLIArgInt("-param-tensor-laynum", 0, 0, F128K, "Номер слоя"),
        VHCLIArgInt("-param-tensor-laycnt", 1, 0, F128K, "Количество слоев, 0=все слои"),

        VHCLIArgOpt("-param-tensor-only-s", false, "Только знак BF16"),
        VHCLIArgOpt("-param-tensor-only-e", false, "Только экспонента BF16"),
        VHCLIArgOpt("-param-tensor-only-m", false, "Только мантисса BF16"),

        // Файлы результата
        VHCLIArgFile("-save-meta-file", "meta_dump.bin", "Файл для записи Meta Info (bytesarray)"),
        VHCLIArgFile("-save-tensor-file", "tensor_dump.bin", "Файл для записи Tensor (bytesarray)"),

        // Формат вывода
        VHCLIArgStr("-format", "text", "Формат вывода: text или json"),

    };

    return keys;
}

/**
 * @brief Parse command line arguments.
 * @param args Output arguments.
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Result code.
 */
verr ParseCLI(CLIArgs &args, int argc, char **argv)
{

    VHCmdLineParser parser;
    parser.setAppName("vhgguftool");
    std::string desc = "Анализ и извлечение данных из .gguf. v" + VHGetVersionString();
    parser.setAppDesc(desc);


    // *** Парсинг командной строки ***
    std::vector<VHArgTypeBase> keys = KeysList();
    verr status = parser.ParseCommandLine(keys, argc, argv);
    if (status != vok)
        return verror(1);

    // *** Разбор командной строки ***
    args.cmd = parser.GetCommand();

    //
    if (args.cmd == "--help" || args.cmd == "--version")
    {
        args.cmdVersion = true;
        parser.ShowUsage(keys);
        return vok;
    }

    // Извлечение значений в структуру
    args.modelFileName = parser.GetFileName("-model-file");

    if (args.cmd == "-gui") {
        args.cmdGUI = true;
        return vok;
    }

    // Валидация обязательного параметра: для GUI-режима (без CLI команды)
    // файл модели не требуется.
    if (!args.cmd.empty() && args.modelFileName.empty())
    {
        std::cerr << "Ошибка: не указан обязательный параметр -model-file." << std::endl;
        parser.ShowUsage(keys);
        return 1;
    }

    // Prepare CLI struct
    args.showInfo = parser.GetOpt("-show-model-info");
    args.showMeta = parser.GetOpt("-show-metadata-list");
    args.showTensors = parser.GetOpt("-show-tensors-list");
    args.showAll = parser.GetOpt("-show-all");

    // Show info
    if (args.showAll)
    {
        args.showInfo = true;
        args.showMeta = true;
        args.showTensors = true;
    }

    // Extract Meta Info
    if (args.cmd == "-cmd-extract-metadata")
    {
        args.cmdExtractMeta = true;
        args.meta_key = parser.GetString("-param-meta-key");
        args.saveMetaFile = parser.GetFileName("-save-meta-file");
        return vok;
    }

    // Extract Tensor
    if (args.cmd == "-cmd-extract-tensordata")
    {
        args.cmdExtractTensor = true;
        args.tensor_name = parser.GetString("-param-tensor-name");
        args.tensor_layer_from = parser.GetInt("-param-tensor-laynum");
        args.tensor_layer_count = parser.GetInt("-param-tensor-laycnt");
        args.saveTensorFile = parser.GetFileName("-save-tensor-file");

        args.param_tensor_only_s = parser.GetOpt("-param-tensor-only-s");
        args.param_tensor_only_e = parser.GetOpt("-param-tensor-only-e");
        args.param_tensor_only_m = parser.GetOpt("-param-tensor-only-m");

        uint8_t check = 0;

        if (args.param_tensor_only_s)
            check++;
        if (args.param_tensor_only_e)
            check++;
        if (args.param_tensor_only_m)
            check++;

        if (check > 1)
            return verrmsg(1000, "Invalid options mixed -param-tensor-only-*");

        return vok;
    }

    return vok;
}
