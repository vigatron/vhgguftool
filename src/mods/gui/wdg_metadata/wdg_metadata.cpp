/**
 * @file wdg_metadata.cpp
 * @brief Реализация виджета отображения Meta Info модели gguf.
 */

#include "wdg_metadata.hpp"

#include "gguf/vhgguf.hpp"
#include "gguf/vhggufmetainfo.hpp"

#include <QHeaderView>
#include <QTreeWidget>
#include <QVBoxLayout>

/**
 * @brief Конструирует виджет.
 * @details Создаёт QTreeWidget с заголовками колонок. Первая колонка (Key)
 *          подгоняется под содержимое и видна полностью, последняя
 *          растягивается на оставшееся место.
 *          Создание элементов выполняется в CreateObjects(),
 *          формирование layout — в CreateLayout().
 * @param parent Родительский виджет, если есть.
 */
VHWidgetGGUFMetadata::VHWidgetGGUFMetadata(QWidget *parent)
    : QWidget(parent)
{
    CreateObjects();
    CreateLayout();
}

/**
 * @brief Создаёт UI/GUI элементы виджета.
 * @details Все элементы создаются через `new` и сохраняются в членах класса.
 */
void VHWidgetGGUFMetadata::CreateObjects()
{
    m_layout = new QVBoxLayout(this);

    m_tree = new QTreeWidget(this);
    m_tree->setColumnCount(5);
    m_tree->setHeaderLabels({
        QStringLiteral("Key"),
        QStringLiteral("Type"),
        QStringLiteral("Offset"),
        QStringLiteral("Size"),
        QStringLiteral("Value")});

    // Разрешаем пользователю свободно менять ширину колонок.
    m_tree->header()->setSectionResizeMode(QHeaderView::Interactive);
    m_tree->setColumnWidth(0, 220); // Key
    m_tree->setColumnWidth(1, 110); // Type
    m_tree->setColumnWidth(2, 100); // Offset
    m_tree->setColumnWidth(3, 80);  // Size
    m_tree->setColumnWidth(4, 300); // Value
}

/**
 * @brief Формирует layout виджета из созданных компонентов.
 */
void VHWidgetGGUFMetadata::CreateLayout()
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_tree);
}

/**
 * @brief Загружает список metadata из модели.
 * @details При `gguf == nullptr` дерево очищается. Многострочное поле
 *          Value сокращается до одной строки с `...` в конце (полное
 *          значение доступно через всплывающую подсказку).
 * @param gguf Указатель на модель gguf (может быть пустым).
 */
void VHWidgetGGUFMetadata::setModel(const VHGGUF * gguf)
{
    m_tree->clear();

    if (!gguf)
        return;

    const u64 count = gguf->get_metadata_count();
    for (u64 i = 0; i < count; ++i) {
        const VHGUFFMetaInfo &meta = gguf->GetMeta(static_cast<std::size_t>(i));

        // Value может быть многострочным: показываем одну строку + "...".
        const QString fullValue = QString::fromStdString(meta.value_string());
        QString valueText = fullValue;
        valueText.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
        valueText.replace(QChar::CarriageReturn, QChar::LineFeed);
        const qsizetype nl = valueText.indexOf(QChar::LineFeed);
        if (nl >= 0)
            valueText = valueText.left(nl) + QStringLiteral("...");

        auto *item = new QTreeWidgetItem(m_tree);
        item->setText(0, QString::fromStdString(meta.key()));

        // Тип: краткое имя + числовой код в скобках, например "STRING (8)".
        const u32 metaType = meta.type();
        item->setText(1, QStringLiteral("%1 (%2)")
                             .arg(QString::fromStdString(
                                      VHGUFFMetaInfo::type_name(metaType)))
                             .arg(metaType));
        item->setText(2, QString::number(meta.file_offset()));
        item->setText(3, QString::number(meta.byte_size()));
        item->setText(4, valueText);
        item->setToolTip(4, fullValue);
    }
}