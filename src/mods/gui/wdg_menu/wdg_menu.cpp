/**
 * @file wdg_menu.cpp
 * @brief Реализация главного меню приложения.
 */

#include "wdg_menu.hpp"

#include <QAction>
#include <QMenu>

/**
 * @brief Конструирует меню.
 * @details Создаёт разделы "File" (Load Model ..., Exit) и "Info" (About).
 *          Действие "Load Model ..." подключено к сигналу loadModelRequested().
 * @param parent Родительский виджет, если есть.
 */
VHWidgetMenu::VHWidgetMenu(QWidget *parent)
    : QMenuBar(parent)
{
    // Раздел File.
    m_fileMenu = addMenu(QStringLiteral("File"));
    m_actionLoadModel = m_fileMenu->addAction(QStringLiteral("Load Model ..."));
    m_actionExit = m_fileMenu->addAction(QStringLiteral("Exit"));

    // Раздел Info.
    m_infoMenu = addMenu(QStringLiteral("Info"));
    m_actionAbout = m_infoMenu->addAction(QStringLiteral("About"));

    // Выбор "Load Model ..." -> сигнал для обработчика в MainWindow.
    connect(m_actionLoadModel, &QAction::triggered,
            this, &VHWidgetMenu::loadModelRequested);
}
