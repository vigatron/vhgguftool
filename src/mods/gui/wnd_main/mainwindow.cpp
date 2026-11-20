/**
 * @file mainwindow.cpp
 * @brief Реализация главного окна графического интерфейса.
 */

#include "mainwindow.hpp"

#include "gguf/vhgguf.hpp"
#include "version_vhgguftool.h"

#include "mods/scan/scan.hpp"
#include "mods/cmds/cmd_run_commands.hpp"

#include "wdg_metadata/wdg_metadata.hpp"
#include "wdg_tensors/wdg_tensors.hpp"
#include "wdg_statusbar/wdg_statusbar.hpp"
#include "wdg_menu/wdg_menu.hpp"

#include <QApplication>
#include <QCursor>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @brief Конструирует главное окно.
 * @details Создаёт центральный виджет и выполняет инициализацию:
 *          сканирование .gguf файла и обработку CLI-команды
 *          (если задана). В GUI-режиме (`-gui`) дополнительно
 *          формирует интерфейс. При ошибке код сохраняется
 *          и может быть получен через Run().
 *          Создание элементов выполняется в CreateObjects(),
 *          формирование layout — в CreateLayout().
 * @param args Разобранные параметры командной строки.
 * @param parent Родительский виджет, если есть.
 */
MainWindow::MainWindow(const CLIArgs &args, QWidget *parent)
    : QMainWindow(parent)
{
    m_args = args;

    setWindowTitle(QStringLiteral("vhgguftool v%1")
                       .arg(QString::fromStdString(VHGetVersionString())));
    resize(800, 600);

    if (m_args.modelFileName.size())
    {

        // Analise GGUF
        if (vok != Scan_GGUF_File(m_gguf, m_args))
        {
            m_exitCode = verrmsg(2, "Error: .gguf model scan failed");
            return;
        }

        // run application
        if (vok != RunCommand(m_gguf, m_args))
        {
            m_exitCode = verrmsg(3, "RunCommand() failed");
            return;
        }
    }

    // CLI-режим (без -gui): интерфейс не строим
    if (!m_args.cmdGUI)
    {

        return;
    }

    // Подсчёт параметров для строки состояния.
    const u64 metadataCount = m_gguf.get_metadata_count();
    const u64 tensorCount = m_gguf.get_tensor_count();

    CreateObjects(m_args.modelFileName, metadataCount, tensorCount);
    CreateLayout();
    setCentralWidget(m_centralWidget);

    // Наполняем виджеты данными модели.
    m_metadataWidget->setModel(&m_gguf);
    m_tensorsWidget->setModel(&m_gguf);

    // Строка состояния и главное меню.
    setStatusBar(m_statusBar);
    setMenuBar(m_menuBar);

    // Перезагрузка модели через меню "Load Model ...".
    connect(m_menuBar, &VHWidgetMenu::loadModelRequested,
            this, &MainWindow::onLoadModelRequested);
}

/**
 * @brief Запускает приложение.
 * @details Если при инициализации произошла ошибка (scan GGUF или
 *          RunCommand), возвращает сохранённый код выхода
 *          (сообщение уже напечатано через `verrmsg`). Для
 *          CLI-режима (`args.cmd != "-gui"`) возвращает `vok`.
 *          В GUI-режиме показывает окно и запускает цикл событий Qt.
 * @return Код выхода приложения.
 */
int MainWindow::Run()
{
    if (m_exitCode != vok)
        return static_cast<int>(m_exitCode);

    // CLI-режим: обработка выполнена в конструкторе
    if (!m_args.cmdGUI)
        return static_cast<int>(vok);

    show();
    return QApplication::exec();
}

/**
 * @brief Создаёт UI/GUI элементы главного окна.
 * @details Все элементы создаются через `new` и сохраняются в членах класса.
 * @param modelFile Имя файла модели для отображения в строке состояния.
 * @param metadataCount Количество записей Metadata.
 * @param tensorCount Количество тензоров.
 */
void MainWindow::CreateObjects(const std::string &modelFile, u64 metadataCount,
                               u64 tensorCount)
{
    // Центральный виджет.
    m_centralWidget = new QWidget(this);
    m_mainLayout = new QVBoxLayout(m_centralWidget);

    // Горизонтальный разделитель.
    m_splitter = new QSplitter(Qt::Horizontal, m_centralWidget);

    // Левая часть — два виджета один над другим.
    m_leftWidget = new QWidget(m_splitter);
    m_leftLayout = new QVBoxLayout(m_leftWidget);

    m_metadataWidget = new VHWidgetGGUFMetadata(m_leftWidget);
    m_tensorsWidget = new VHWidgetGGUFTensors(m_leftWidget);

    // Правая часть — зарезервирована на будущее.
    m_rightWidget = new QWidget(m_splitter);
    m_rightLayout = new QVBoxLayout(m_rightWidget);
    m_rightLabel = new QLabel(QStringLiteral("Reserved"), m_rightWidget);
    m_rightLabel->setAlignment(Qt::AlignCenter);

    // Строка состояния с моделью, количеством metadata и tensors.
    m_statusBar = new VHWidgetStatusBar(modelFile, metadataCount, tensorCount, this);

    // Главное меню.
    m_menuBar = new VHWidgetMenu(this);
}

/**
 * @brief Формирует layout главного окна из созданных компонентов.
 */
void MainWindow::CreateLayout()
{
    // Центральный layout.
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    // Левый вертикальный layout.
    m_leftLayout->setContentsMargins(0, 0, 0, 0);
    m_leftLayout->addWidget(m_metadataWidget);
    m_leftLayout->addWidget(m_tensorsWidget);

    // Правый вертикальный layout.
    m_rightLayout->addWidget(m_rightLabel);

    // Собираем горизонтальный разделитель.
    m_splitter->addWidget(m_leftWidget);
    m_splitter->addWidget(m_rightWidget);

    // Левая часть занимает 70% ширины, правая — остальные 30%.
    m_splitter->setStretchFactor(0, 7);
    m_splitter->setStretchFactor(1, 3);
    m_splitter->setSizes({560, 240});

    // Добавляем разделитель в центральный layout.
    m_mainLayout->addWidget(m_splitter);
}

/**
 * @brief Обрабатывает выбор "Load Model ..." в главном меню.
 * @details Открывает диалог выбора .gguf файла, сканирует его во временный
 *          объект VHGGUF и при успехе подменяет текущую модель через move.
 *          При ошибке сканирования текущая модель остаётся без изменений.
 *          После успешной загрузки обновляет деревья Metadata/Tensors
 *          и строку состояния.
 */
void MainWindow::onLoadModelRequested()
{
    const QString startDir =
        QFileInfo(QString::fromStdString(m_args.modelFileName)).absolutePath();

    const QString fileName = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Load Model ..."),
        startDir,
        QStringLiteral("GGUF files (*.gguf);;All files (*)"));

    // Пользователь отменил выбор.
    if (fileName.isEmpty())
        return;

    CLIArgs newArgs = m_args;
    newArgs.modelFileName = fileName.toStdString();

    // Сканируем во временную модель: при ошибке текущая останется нетронутой.
    QApplication::setOverrideCursor(Qt::WaitCursor);
    VHGGUF tmp;
    const verr scanResult = Scan_GGUF_File(tmp, newArgs);
    QApplication::restoreOverrideCursor();

    if (vok != scanResult)
    {
        QMessageBox::critical(
            this,
            QStringLiteral("Load Model ..."),
            QStringLiteral("Failed to load model:\n%1\n\nThe current model is unchanged.")
                .arg(fileName));
        return;
    }

    m_gguf = std::move(tmp);
    m_args = std::move(newArgs);

    // Обновляем деревья и строку состояния новой моделью.
    m_metadataWidget->setModel(&m_gguf);
    m_tensorsWidget->setModel(&m_gguf);
    m_statusBar->setModelInfo(m_args.modelFileName,
                              m_gguf.get_metadata_count(),
                              m_gguf.get_tensor_count());
}
