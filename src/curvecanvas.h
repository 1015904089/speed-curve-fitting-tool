#pragma once

#include <QPointF>
#include <QWidget>

#include <vector>

struct ViewTransform
{
    double scaleX = 90.0;
    double scaleY = 60.0;
    double offsetX = 72.0;
    double offsetY = 260.0;
};

class CurveCanvas final : public QWidget
{
    Q_OBJECT

public:
    explicit CurveCanvas(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QPointF screenToWorld(const QPointF &screen) const;
    QPointF worldToScreen(const QPointF &world) const;
    void drawGrid(QPainter &painter);
    void drawDraftStroke(QPainter &painter);
    void drawControlMarkers(QPainter &painter);
    void drawInfoOverlay(QPainter &painter);
    void toggleOverlay();

    ViewTransform m_view;
    std::vector<QPointF> m_draftPoints;
    bool m_drawing = false;
    bool m_panning = false;
    bool m_spacePan = false;
    bool m_overlayCollapsed = false;
    QPoint m_lastMousePos;
};
