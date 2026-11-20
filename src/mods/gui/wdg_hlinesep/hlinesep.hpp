/**
 * @file hlinesep.hpp
 * @brief Горизонтальный визуальный разделитель для вертикальных layout'ов.
 */

#pragma once

#include <QFrame>

/**
 * @brief Горизонтальный визуальный разделитель.
 * @details «Вдавленная» (Sunken) горизонтальная линия минимальной высоты 4 px.
 *          Используется для разделения элементов в вертикальных контейнерах.
 */
class HLineSep : public QFrame
{
public:
    /**
     * @brief Конструирует разделитель.
     * @param parent Родительский виджет, если есть.
     */
    explicit HLineSep(QWidget *parent = nullptr);
};
