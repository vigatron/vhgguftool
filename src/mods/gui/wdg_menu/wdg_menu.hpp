/**
 * @file wdg_menu.hpp
 * @brief Главное меню приложения.
 */

#pragma once

#include <QMenuBar>

class QMenu;
class QAction;

/**
 * @brief Главное меню приложения.
 * @details Содержит разделы "File" (Load Model ..., Exit) и "Info" (About).
 *          Выбор "Load Model ..." порождает сигнал loadModelRequested().
 *          Действия Exit и About пока не подключены к обработчикам.
 */
class VHWidgetMenu : public QMenuBar
{
    Q_OBJECT

public:
    /**
     * @brief Конструирует меню.
     * @param parent Родительский виджет, если есть.
     */
    explicit VHWidgetMenu(QWidget *parent = nullptr);

    /**
     * @brief Деструктор.
     */
    ~VHWidgetMenu() override = default;

signals:
    /**
     * @brief Сигнал о выборе действия "Load Model ..." в разделе File.
     */
    void loadModelRequested();

private:
    QMenu * m_fileMenu = nullptr;
    QMenu * m_infoMenu = nullptr;
    QAction * m_actionLoadModel = nullptr;
    QAction * m_actionExit = nullptr;
    QAction * m_actionAbout = nullptr;
};
