/**
 * @file wdg_metadata.hpp
 * @brief Виджет отображения Meta Info модели gguf в виде дерева.
 */

#pragma once

#include <QWidget>

class QTreeWidget;
class QVBoxLayout;
class VHGGUF;

/**
 * @brief Виджет отображения Meta Info модели gguf.
 * @details Содержит QTreeWidget для отображения списка metadata-записей.
 */
class VHWidgetGGUFMetadata : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Конструирует виджет.
     * @param parent Родительский виджет, если есть.
     */
    explicit VHWidgetGGUFMetadata(QWidget *parent = nullptr);

    /**
     * @brief Деструктор.
     */
    ~VHWidgetGGUFMetadata() override = default;

    /**
     * @brief Загружает список metadata из модели.
     * @details При `gguf == nullptr` дерево очищается.
     * @param gguf Указатель на модель gguf (может быть пустым).
     */
    void setModel(const VHGGUF * gguf);

private:
    /**
     * @brief Создаёт UI/GUI элементы виджета.
     * @details Все элементы создаются через `new` и сохраняются в членах класса.
     */
    void CreateObjects();

    /**
     * @brief Формирует layout виджета из созданных компонентов.
     */
    void CreateLayout();

    QVBoxLayout * m_layout = nullptr;
    QTreeWidget * m_tree = nullptr;
};