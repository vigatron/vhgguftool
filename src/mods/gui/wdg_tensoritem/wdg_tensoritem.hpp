/**
 * @file wdg_tensoritem.hpp
 * @brief Элемент дерева тензоров с корректной сортировкой по колонкам.
 */

#pragma once

#include <QTreeWidgetItem>

/**
 * @brief Элемент списка тензоров с числовой сортировкой.
 * @details По умолчанию QTreeWidgetItem сравнивает текст (Qt::DisplayRole),
 *          из-за чего колонки Type/BlockSize сортируются лексикографически
 *          ("10" < "5"). Этот класс сравнивает числовые колонки по значению
 *          из Qt::UserRole, а колонку Name — без учёта регистра.
 *          Числовые колонки: Type(2), TensorOffset(3), FileOffset(4),
 *          BlockSize(5).
 */
class VHTensorItem : public QTreeWidgetItem
{
public:
    /**
     * @brief Конструирует элемент.
     * @param parent Родительский элемент или дерево, если есть.
     */
    explicit VHTensorItem(QTreeWidget *parent = nullptr);

    /**
     * @brief Сравнивает элементы для сортировки.
     * @details Числовые колонки сравниваются как u64 из Qt::UserRole,
     *          Name — без учёта регистра, остальные — как текст.
     * @param other Сравниваемый элемент.
     * @return `true`, если этот элемент меньше `other`.
     */
    bool operator<(const QTreeWidgetItem &other) const override;
};