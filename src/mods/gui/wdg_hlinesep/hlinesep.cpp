/**
 * @file hlinesep.cpp
 * @brief Реализация горизонтального визуального разделителя.
 */

#include "hlinesep.hpp"

/**
 * @brief Конструирует разделитель.
 * @param parent Родительский виджет, если есть.
 */
HLineSep::HLineSep(QWidget *parent)
    : QFrame(parent)
{
    setMinimumHeight(4);
    setFrameShape(QFrame::HLine);
    setFrameShadow(QFrame::Sunken);
}
