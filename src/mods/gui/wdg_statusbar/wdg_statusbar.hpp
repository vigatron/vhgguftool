/**
 * @file wdg_statusbar.hpp
 * @brief Строка состояния главного окна с зарезервированными полями.
 */

#pragma once

#include <QStatusBar>

#include "aiplatform.hpp"

#include <string>

class QLabel;
class VLineSep;

/**
 * @brief Строка состояния главного окна.
 * @details Содержит пять постоянных полей: первое — "Model",
 *          второе — имя файла модели (без пути), третье — количество записей Metadata,
 *          четвертое — количество тензоров, пятое пока пустое.
 */
class VHWidgetStatusBar : public QStatusBar
{
    Q_OBJECT

public:
    /**
     * @brief Конструирует строку состояния.
     * @param modelFile Имя файла модели для поля "Model".
     * @param metadataCount Количество записей Metadata.
     * @param tensorCount Количество тензоров.
     * @param parent Родительский виджет, если есть.
     */
    explicit VHWidgetStatusBar(const std::string &modelFile, u64 metadataCount,
                               u64 tensorCount, QWidget *parent = nullptr);

    /**
     * @brief Обновляет имя файла модели и счётчики Metadata/Tensors.
     * @details Используется при перезагрузке модели без пересоздания строки состояния.
     *          В строке состояния имя модели отображается без полного пути — только
     *          имя файла.
     * @param modelFile Путь или имя файла модели (отображается только имя файла).
     * @param metadataCount Количество записей Metadata.
     * @param tensorCount Количество тензоров.
     */
    void setModelInfo(const std::string &modelFile, u64 metadataCount, u64 tensorCount);

    /**
     * @brief Деструктор.
     */
    ~VHWidgetStatusBar() override = default;

private:
    QLabel * m_labelModel = nullptr;
    VLineSep * m_sep1 = nullptr;
    QLabel * m_labelModelFile = nullptr;
    VLineSep * m_sep2 = nullptr;
    QLabel * m_labelMetadata = nullptr;
    VLineSep * m_sep3 = nullptr;
    QLabel * m_labelTensors = nullptr;
    VLineSep * m_sep4 = nullptr;
    QLabel * m_labelReserved = nullptr;
};
