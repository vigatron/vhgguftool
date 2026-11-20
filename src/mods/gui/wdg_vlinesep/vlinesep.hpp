/**
 * @file vlinesep.hpp
 * @brief Вертикальный визуальный разделитель для горизонтальных layout'ов.
 */

#pragma once

#include <QFrame>

/**
 * @brief Вертикальный визуальный разделитель.
 * @details «Вдавленная» (Sunken) вертикальная линия минимальной ширины 4 px.
 *          Используется для разделения полей в горизонтальных контейнерах
 *          (например, в строке состояния).
 */
class VLineSep : public QFrame
{
public:
    /**
     * @brief Конструирует разделитель.
     * @param parent Родительский виджет, если есть.
     */
    explicit VLineSep(QWidget *parent = nullptr);
};
