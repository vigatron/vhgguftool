#include "main.hpp"

#include "mods/gui/wnd_main/mainwindow.hpp"

#include <QApplication>

/**
 * @brief Program entry point.
 * @param argc Argument count.
 * @param argv Argument values.
 * @return Exit code.
 * */
int main(int argc, char **argv)
{
    // Разбор коммандной строки
    CLIArgs cliargs;

    if (argc > 1)
    {
        // Parse Command Line
        if (vok != ParseCLI(cliargs, argc, argv))
            return verrmsg(1, "Error: parsing CLI");

        // App Info Only
        if (cliargs.cmdVersion)
            return vok;
    } else {
        cliargs.cmdGUI = true;
    }

    // Запуск приложения: CLI-команда или GUI (параметр -gui)
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("vhgguftool"));

    MainWindow window(cliargs);
    return window.Run();
}
