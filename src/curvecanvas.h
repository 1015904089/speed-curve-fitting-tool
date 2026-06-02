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

public slots:
    void setSceneLimits(double maxTimeSeconds, double minPosition, double maxPosition);
    void setSliderPreview(double timeSeconds, double position, bool active);
    void resetCanvas();

signals:
    void resetRequested();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QPointF screenToWorld(const QPointF &screen) const;
    QPointF worldToScreen(const QPointF &world) const;
    void resetViewport();
    void clampViewport();
    bool appendDraftPoint(const QPointF &screenPoint);
    void drawGrid(QPainter &painter);
    void drawDraftStroke(QPainter &painter);
    void drawControlMarkers(QPainter &painter);
    void drawSliderPreview(QPainter &painter);
    void drawRangeLabels(QPainter &painter);
    void drawInfoOverlay(QPainter &painter);
    QRect overlayDragHandleRect() const;
    void toggleOverlay();

    ViewTransform m_view;
    double m_maxTimeSeconds = 10.0;
    double m_minPosition = 0.0;
    double m_maxPosition = 100.0;
    double m_visibleTimeSeconds = 11.0;
    double m_visibleMinPosition = -10.0;
    double m_visibleMaxPosition = 110.0;
    double m_leftMargin = 72.0;
    double m_rightMargin = 22.0;
    double m_topMargin = 26.0;
    double m_bottomMargin = 44.0;
    std::vector<QPointF> m_draftPoints;
    bool m_drawing = false;
    bool m_panning = false;
    bool m_draggingOverlay = false;
    bool m_spacePan = false;
    bool m_overlayCollapsed = false;
    QRect m_overlayRect;
    QPoint m_overlayTopLeft = {12, 12};
    QPoint m_overlayDragOffset;
    double m_previewTime = 0.0;
    double m_previewPosition = 0.0;
    bool m_previewActive = false;
    QPoint m_lastMousePos;
};
