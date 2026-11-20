/**
 * @file wdg_tensors.cpp
 * @brief Реализация виджета отображения списка тензоров модели gguf.
 */

#include "wdg_tensors.hpp"

#include "gguf/vhgguf.hpp"
#include "gguf/vhgguftensor.hpp"
#include "texttools/texttools.hpp"
#include "wdg_tensoritem/wdg_tensoritem.hpp"

#include <QHeaderView>
#include <QTreeWidget>
#include <QVariant>
#include <QVBoxLayout>

#include <limits>
#include <stdexcept>

/**
 * @brief Конструирует виджет.
 * @details Создаёт QTreeWidget с заголовками колонок. Первая колонка (Name)
 *          подгоняется под содержимое и видна полностью, последняя
 *          растягивается на оставшееся место.
 *          Создание элементов выполняется в CreateObjects(),
 *          формирование layout — в CreateLayout().
 * @param parent Родительский виджет, если есть.
 */
VHWidgetGGUFTensors::VHWidgetGGUFTensors(QWidget *parent)
    : QWidget(parent)
{
    CreateObjects();
    CreateLayout();
}

/**
 * @brief Создаёт UI/GUI элементы виджета.
 * @details Все элементы создаются через `new` и сохраняются в членах класса.
 */
void VHWidgetGGUFTensors::CreateObjects()
{
    m_layout = new QVBoxLayout(this);

    m_tree = new QTreeWidget(this);
    m_tree->setColumnCount(6);
    m_tree->setHeaderLabels({
        QStringLiteral("Name"),
        QStringLiteral("Shape"),
        QStringLiteral("Type"),
        QStringLiteral("TensorOffset"),
        QStringLiteral("FileOffset"),
        QStringLiteral("BlockSize")});

    // Разрешаем пользователю свободно менять ширину колонок.
    m_tree->header()->setSectionResizeMode(QHeaderView::Interactive);
    m_tree->setColumnWidth(0, 180); // Name
    m_tree->setColumnWidth(1, 140); // Shape
    m_tree->setColumnWidth(2, 100); // Type
    m_tree->setColumnWidth(3, 110); // TensorOffset
    m_tree->setColumnWidth(4, 110); // FileOffset
    m_tree->setColumnWidth(5, 110); // BlockSize

    // Сортировка по колонкам (клик по заголовку); стартовый порядок — по Name.
    m_tree->setSortingEnabled(true);
    m_tree->sortByColumn(0, Qt::AscendingOrder);
}

/**
 * @brief Формирует layout виджета из созданных компонентов.
 */
void VHWidgetGGUFTensors::CreateLayout()
{
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_tree);
}

/**
 * @brief Загружает список тензоров из модели.
 * @param gguf Указатель на модель gguf (может быть пустым).
 */
void VHWidgetGGUFTensors::setModel(const VHGGUF * gguf)
{
    // Запоминаем текущую сортировку, чтобы восстановить её после наполнения.
    const int sortColumn = m_tree->header()->sortIndicatorSection();
    const Qt::SortOrder sortOrder = m_tree->header()->sortIndicatorOrder();

    // На время наполнения отключаем сортировку (иначе O(n^2) на больших моделях).
    m_tree->setSortingEnabled(false);
    m_tree->clear();

    if (gguf) {
        const u64 data_start = gguf->get_data_start();
        const u64 count = gguf->get_tensor_count();

        for (u64 i = 0; i < count; ++i) {
            const VHGUFFTensor &t = gguf->GetTensor(static_cast<std::size_t>(i));

            QString shape;
            for (std::size_t d = 0; d < t.dims.size(); ++d) {
                if (d)
                    shape += QStringLiteral(", ");
                shape += QString::number(t.dims[d]);
            }

            u64 blockBytes = 0;
            bool blockOk = false;
            try {
                blockBytes = t.block_size_full();
                blockOk = true;
            } catch (const std::exception &) {
            }

            auto *item = new VHTensorItem(m_tree);
            item->setText(0, QString::fromStdString(t.name));
            item->setText(1, shape);

            // Тип тензора (GGML): краткое имя + числовой код, например "BF16 (30)".
            const u32 tensorType = t.type;
            item->setText(2, QStringLiteral("%1 (%2)")
                                 .arg(QString::fromStdString(
                                          VHGUFFTensor::type_name(tensorType)))
                                 .arg(tensorType));
            item->setText(3, QString::number(t.offset));
            item->setText(4, QString::number(data_start + t.offset));

            // Числовые значения для корректной сортировки колонок.
            item->setData(2, Qt::UserRole, QVariant::fromValue<qulonglong>(tensorType));
            item->setData(3, Qt::UserRole, QVariant::fromValue<qulonglong>(t.offset));
            item->setData(4, Qt::UserRole,
                          QVariant::fromValue<qulonglong>(data_start + t.offset));

            if (blockOk) {
                item->setText(5, QString::fromStdString(
                                     smartsize(static_cast<size_t>(blockBytes))));
                item->setData(5, Qt::UserRole, QVariant::fromValue<qulonglong>(blockBytes));
            } else {
                item->setText(5, QStringLiteral("n/a"));
                // "n/a" уходит в конец при сортировке по BlockSize.
                item->setData(5, Qt::UserRole,
                              QVariant::fromValue<qulonglong>(std::numeric_limits<u64>::max()));
            }
        }
    }

    m_tree->setSortingEnabled(true);
    m_tree->sortByColumn(sortColumn, sortOrder);
}