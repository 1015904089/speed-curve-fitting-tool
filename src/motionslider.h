#pragma once

#include <QWidget>

class MotionSlider final : public QWidget
{
    Q_OBJECT

public:
    explicit MotionSlider(QWidget *parent = nullptr);

    double normalizedPosition() const;
    void setNormalizedPosition(double value);

signals:
    void normalizedPositionChanged(double value);
    void dragStarted();
    void dragFinished();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QRectF trackRect() const;
    void updateFromMouse(double x);

    double m_position = 0.0;
    bool m_dragging = false;
};
