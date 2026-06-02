#pragma once

#include <QSplitter>

class RestorableSplitter final : public QSplitter
{
    Q_OBJECT

public:
    explicit RestorableSplitter(Qt::Orientation orientation, QWidget *parent = nullptr);

signals:
    void handleDoubleClicked();

protected:
    QSplitterHandle *createHandle() override;
};
