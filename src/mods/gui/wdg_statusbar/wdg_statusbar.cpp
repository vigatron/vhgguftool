/**
 * @file wdg_statusbar.cpp
 * @brief Реализация строки состояния главного окна.
 */

#include "wdg_statusbar.hpp"

#include "wdg_vlinesep/vlinesep.hpp"

#include <QFileInfo>
#include <QLabel>

/**
 * @brief Конструирует строку состояния.
 * @details Добавляет пять постоянных полей: первое — "Model",
 *          второе — имя файла модели (без пути), третье — количество записей Metadata,
 *          четвертое — количество тензоров, пятое пустое. Поля разделяются
 *          вертикальным визуальным сепаратором VLineSep.
 * @param modelFile Путь или имя файла модели (в строке состояния — только имя файла).
 * @param metadataCount Количество записей Metadata.
 * @param tensorCount Количество тензоров.
 * @param parent Родительский виджет, если есть.
 */
VHWidgetStatusBar::VHWidgetStatusBar(const std::string &modelFile, u64 metadataCount,
                                     u64 tensorCount, QWidget *parent)
    : QStatusBar(parent)
{
    m_labelModel = new QLabel(QStringLiteral("Model"), this);
    m_labelModel->setMinimumWidth(120);
    addPermanentWidget(m_labelModel);

    m_sep1 = new VLineSep(this);
    addPermanentWidget(m_sep1);

    m_labelModelFile = new QLabel(this);
    m_labelModelFile->setMinimumWidth(120);
    addPermanentWidget(m_labelModelFile);

    m_sep2 = new VLineSep(this);
    addPermanentWidget(m_sep2);

    m_labelMetadata = new QLabel(this);
    m_labelMetadata->setMinimumWidth(120);
    addPermanentWidget(m_labelMetadata);

    m_sep3 = new VLineSep(this);
    addPermanentWidget(m_sep3);

    m_labelTensors = new QLabel(this);
    m_labelTensors->setMinimumWidth(120);
    addPermanentWidget(m_labelTensors);

    m_sep4 = new VLineSep(this);
    addPermanentWidget(m_sep4);

    m_labelReserved = new QLabel(this);
    m_labelReserved->setMinimumWidth(120);
    addPermanentWidget(m_labelReserved);

    setModelInfo(modelFile, metadataCount, tensorCount);
}

/**
 * @brief Обновляет имя файла модели и счётчики Metadata/Tensors.
 * @details Имя модели отображается без полного пути — только имя файла.
 *          Заменяет содержимое полей "Model", "Metadata" и "Tensors".
 * @param modelFile Путь или имя файла модели (в строке состояния — только имя файла).
 * @param metadataCount Количество записей Metadata.
 * @param tensorCount Количество тензоров.
 */
void VHWidgetStatusBar::setModelInfo(const std::string &modelFile, u64 metadataCount,
                                     u64 tensorCount)
{
    // Отображаем только имя файла модели, без пути к нему.
    const QString fileName = QFileInfo(QString::fromStdString(modelFile)).fileName();
    m_labelModelFile->setText(fileName);
    m_labelMetadata->setText(
        QStringLiteral("Metadata: %1")
            .arg(QString::number(static_cast<qulonglong>(metadataCount))));
    m_labelTensors->setText(
        QStringLiteral("Tensors: %1")
            .arg(QString::number(static_cast<qulonglong>(tensorCount))));
}
