/**
 * @file wdg_tensoritem.cpp
 * @brief Реализация элемента дерева тензоров с числовой сортировкой.
 */

#include "wdg_tensoritem.hpp"

#include <QTreeWidget>

/**
 * @brief Конструирует элемент.
 * @param parent Родительский элемент или дерево, если есть.
 */
VHTensorItem::VHTensorItem(QTreeWidget *parent)
    : QTreeWidgetItem(parent)
{
}

/**
 * @brief Сравнивает элементы для сортировки.
 * @details Числовые колонки (Type, TensorOffset, FileOffset, BlockSize)
 *          сравниваются как u64 из Qt::UserRole, Name — без учёта регистра,
 *          остальные колонки — как текст.
 * @param other Сравниваемый элемент.
 * @return `true`, если этот элемент меньше `other`.
 */
bool VHTensorItem::operator<(const QTreeWidgetItem &other) const
{
    const QTreeWidget *tree = treeWidget();
    const int column = tree ? tree->sortColumn() : 0;

    // Числовые колонки: Type(2), TensorOffset(3), FileOffset(4), BlockSize(5).
    if (column >= 2) {
        const qulonglong a = data(column, Qt::UserRole).toULongLong();
        const qulonglong b = other.data(column, Qt::UserRole).toULongLong();
        return a < b;
    }

    // Name(0) — без учёта регистра; Shape(1) — как текст.
    return QString::compare(text(column), other.text(column), Qt::CaseInsensitive) < 0;
}