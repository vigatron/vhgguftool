/**
 * @file vlinesep.cpp
 * @brief Реализация вертикального визуального разделителя.
 */

#include "vlinesep.hpp"

/**
 * @brief Конструирует разделитель.
 * @param parent Родительский виджет, если есть.
 */
VLineSep::VLineSep(QWidget *parent)
    : QFrame(parent)
{
    setMinimumWidth(4);
    setFrameShape(QFrame::VLine);
    setFrameShadow(QFrame::Sunken);
}
