#pragma once

#include <QWidget>

class ResultPanel final : public QWidget
{
    Q_OBJECT

public:
    explicit ResultPanel(QWidget *parent = nullptr);
};
