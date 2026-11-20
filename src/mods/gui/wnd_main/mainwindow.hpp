/**
 * @file mainwindow.hpp
 * @brief Главное окно графического интерфейса vhgguftool.
 */

#pragma once

#include <QMainWindow>

#include "aiplatform.hpp"
#include "mods/cli/cli.hpp"
#include "gguf/vhgguf.hpp"

#include <string>

class QWidget;
class QVBoxLayout;
class QSplitter;
class QLabel;
class VHWidgetGGUFMetadata;
class VHWidgetGGUFTensors;
class VHWidgetStatusBar;
class VHWidgetMenu;

/**
 * @brief Главное окно приложения vhgguftool на Qt6 Widgets.
 * @details Центральный виджет с горизонтальным разделителем:
 *          слева — два виджета один над другим (Meta Info сверху,
 *          Tensors снизу, 70% ширины), справа — зарезервированное
 *          место (30% ширины) под будущую визуализацию.
 *          Создание элементов выполняется в CreateObjects(),
 *          формирование layout — в CreateLayout().
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Конструирует главное окно.
     * @details Создаёт центральный виджет и выполняет инициализацию:
     *          сканирование .gguf файла и обработку CLI-команды
     *          (если задана). В GUI-режиме (`-gui`) дополнительно
     *          формирует интерфейс. При ошибке код сохраняется
     *          и может быть получен через Run().
     * @param args Разобранные параметры командной строки.
     * @param parent Родительский виджет, если есть.
     */
    explicit MainWindow(const CLIArgs &args, QWidget *parent = nullptr);

    /**
     * @brief Деструктор главного окна.
     */
    ~MainWindow() override = default;

    /**
     * @brief Запускает приложение.
     * @details Если при инициализации произошла ошибка (scan GGUF или
     *          RunCommand), возвращает сохранённый код выхода
     *          (сообщение уже напечатано через `verrmsg`). Для
     *          CLI-режима (`args.cmd != "-gui"`) возвращает `vok`.
     *          В GUI-режиме показывает окно и запускает цикл событий Qt.
     * @return Код выхода приложения.
     */
    int Run();

private:
    /**
     * @brief Создаёт UI/GUI элементы главного окна.
     * @details Все элементы создаются через `new` и сохраняются в членах класса.
     * @param modelFile Имя файла модели для отображения в строке состояния.
     * @param metadataCount Количество записей Metadata.
     * @param tensorCount Количество тензоров.
     */
    void CreateObjects(const std::string &modelFile, u64 metadataCount, u64 tensorCount);

    /**
     * @brief Формирует layout главного окна из созданных компонентов.
     */
    void CreateLayout();

    /**
     * @brief Обрабатывает выбор "Load Model ..." в главном меню.
     * @details Открывает диалог выбора .gguf файла, сканирует его во временный
     *          объект VHGGUF и при успехе подменяет текущую модель (старая
     *          модель при ошибке сохраняется). После успешной загрузки
     *          обновляет деревья Metadata/Tensors и строку состояния.
     */
    void onLoadModelRequested();

    VHGGUF m_gguf;
    CLIArgs m_args;
    verr m_exitCode = vok;

    QWidget * m_centralWidget = nullptr;
    QVBoxLayout * m_mainLayout = nullptr;
    QSplitter * m_splitter = nullptr;
    QWidget * m_leftWidget = nullptr;
    QVBoxLayout * m_leftLayout = nullptr;
    VHWidgetGGUFMetadata * m_metadataWidget = nullptr;
    VHWidgetGGUFTensors * m_tensorsWidget = nullptr;
    QWidget * m_rightWidget = nullptr;
    QVBoxLayout * m_rightLayout = nullptr;
    QLabel * m_rightLabel = nullptr;
    VHWidgetStatusBar * m_statusBar = nullptr;
    VHWidgetMenu * m_menuBar = nullptr;
};
