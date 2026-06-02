#include "curvecanvas.h"

#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QResizeEvent>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>

CurveCanvas::CurveCanvas(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAutoFillBackground(false);
    resetViewport();
}

void CurveCanvas::setSceneLimits(double maxTimeSeconds, double minPosition, double maxPosition)
{
    m_maxTimeSeconds = std::max(0.1, maxTimeSeconds);
    if (maxPosition <= minPosition) {
        maxPosition = minPosition + 1.0;
    }
    m_minPosition = minPosition;
    m_maxPosition = maxPosition;
    m_visibleTimeSeconds = m_maxTimeSeconds * 1.1;
    const double range = m_maxPosition - m_minPosition;
    m_visibleMinPosition = m_minPosition - range * 0.1;
    m_visibleMaxPosition = m_maxPosition + range * 0.1;
    resetViewport();
    update();
}

void CurveCanvas::resetCanvas()
{
    m_draftPoints.clear();
    m_previewActive = false;
    resetViewport();
    update();
}

void CurveCanvas::setSliderPreview(double timeSeconds, double position, bool active)
{
    m_previewTime = std::clamp(timeSeconds, 0.0, m_maxTimeSeconds);
    m_previewPosition = std::clamp(position, m_minPosition, m_maxPosition);
    m_previewActive = active;
    update();
}

void CurveCanvas::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    resetViewport();
}

void CurveCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), palette().color(QPalette::Base));

    drawGrid(painter);
    drawDraftStroke(painter);
    drawControlMarkers(painter);
    drawSliderPreview(painter);
    drawRangeLabels(painter);
    drawInfoOverlay(painter);
}

void CurveCanvas::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    m_lastMousePos = event->pos();

    if (event->button() == Qt::LeftButton) {
        if (m_overlayRect.contains(event->pos())) {
            if (overlayDragHandleRect().contains(event->pos())) {
                m_draggingOverlay = true;
                m_overlayDragOffset = event->pos() - m_overlayTopLeft;
            } else {
                toggleOverlay();
            }
            return;
        }
        m_drawing = appendDraftPoint(event->position());
    } else if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void CurveCanvas::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint delta = event->pos() - m_lastMousePos;

    if (m_drawing) {
        appendDraftPoint(event->position());
    } else if (m_draggingOverlay) {
        m_overlayTopLeft = event->pos() - m_overlayDragOffset;
        m_overlayTopLeft.setX(std::clamp(m_overlayTopLeft.x(), 0, std::max(0, width() - m_overlayRect.width())));
        m_overlayTopLeft.setY(std::clamp(m_overlayTopLeft.y(), 0, std::max(0, height() - m_overlayRect.height())));
        update();
    } else if (m_panning || m_spacePan) {
        m_view.offsetY += delta.y();
        clampViewport();
        update();
    }

    m_lastMousePos = event->pos();
}

void CurveCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_drawing = false;
        m_draggingOverlay = false;
    } else if (event->button() == Qt::MiddleButton) {
        m_panning = false;
        unsetCursor();
    }
}

void CurveCanvas::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_overlayRect.contains(event->pos())) {
        toggleOverlay();
        return;
    }

    QMenu menu(this);
    QAction *resetAction = menu.addAction(tr("Reset"));
    connect(resetAction, &QAction::triggered, this, &CurveCanvas::resetRequested);
    menu.exec(event->globalPos());
}

void CurveCanvas::wheelEvent(QWheelEvent *event)
{
    const QPointF before = screenToWorld(event->position());
    const double factor = event->angleDelta().y() > 0 ? 1.12 : 1.0 / 1.12;
    const double usableWidth = std::max(1.0, width() - m_leftMargin - m_rightMargin);
    const double usableHeight = std::max(1.0, height() - m_topMargin - m_bottomMargin);
    const double minScaleX = usableWidth / m_visibleTimeSeconds;
    const double visibleRange = std::max(1.0, m_visibleMaxPosition - m_visibleMinPosition);
    const double minScaleY = usableHeight / visibleRange;

    m_view.scaleX = std::clamp(m_view.scaleX * factor, minScaleX, minScaleX * 30.0);
    m_view.scaleY = std::clamp(m_view.scaleY * factor, minScaleY, minScaleY * 30.0);

    const QPointF afterScreen = worldToScreen(before);
    m_view.offsetY += event->position().y() - afterScreen.y();
    clampViewport();

    update();
}

void CurveCanvas::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_I) {
        toggleOverlay();
        return;
    }

    if (event->key() == Qt::Key_Escape) {
        m_draftPoints.clear();
        update();
        return;
    }

    QWidget::keyPressEvent(event);
}

QPointF CurveCanvas::screenToWorld(const QPointF &screen) const
{
    return {
        (screen.x() - m_view.offsetX) / m_view.scaleX,
        (m_view.offsetY - screen.y()) / m_view.scaleY,
    };
}

QPointF CurveCanvas::worldToScreen(const QPointF &world) const
{
    return {
        world.x() * m_view.scaleX + m_view.offsetX,
        m_view.offsetY - world.y() * m_view.scaleY,
    };
}

void CurveCanvas::drawGrid(QPainter &painter)
{
    const QColor minor = palette().color(QPalette::Midlight);
    const QColor major = palette().color(QPalette::Mid);
    const QColor axis = palette().color(QPalette::Text);

    const double baseStep = 1.0;
    const double minPixelStep = 52.0;
    double worldStepX = baseStep;
    double worldStepY = baseStep;

    while (worldStepX * m_view.scaleX < minPixelStep) {
        worldStepX *= 2.0;
    }
    while (worldStepY * m_view.scaleY < minPixelStep) {
        worldStepY *= 2.0;
    }

    const QPointF topLeft = screenToWorld(rect().topLeft());
    const QPointF bottomRight = screenToWorld(rect().bottomRight());

    painter.setPen(QPen(minor, 1));

    const int startX = std::max(0, static_cast<int>(std::floor(topLeft.x() / worldStepX)) - 1);
    const int endX = static_cast<int>(std::ceil(bottomRight.x() / worldStepX)) + 1;
    for (int i = startX; i <= endX; ++i) {
        const double x = i * worldStepX;
        const QPointF a = worldToScreen({x, topLeft.y()});
        const QPointF b = worldToScreen({x, bottomRight.y()});
        painter.setPen(QPen(std::abs(x) < 1e-9 ? axis : (i % 2 == 0 ? major : minor), std::abs(x) < 1e-9 ? 1.6 : 1.0));
        painter.drawLine(QPointF(a.x(), 0), QPointF(b.x(), height()));
        if (a.x() >= 0 && a.x() <= width()) {
            painter.drawText(QPointF(a.x() + 4, m_view.offsetY + 18), tr("%1s").arg(x, 0, 'f', x < 10 ? 1 : 0));
        }
    }

    const int startY = static_cast<int>(std::floor(bottomRight.y() / worldStepY)) - 1;
    const int endY = static_cast<int>(std::ceil(topLeft.y() / worldStepY)) + 1;
    for (int i = startY; i <= endY; ++i) {
        const double y = i * worldStepY;
        const QPointF a = worldToScreen({topLeft.x(), y});
        const QPointF b = worldToScreen({bottomRight.x(), y});
        painter.setPen(QPen(std::abs(y) < 1e-9 ? axis : (i % 2 == 0 ? major : minor), std::abs(y) < 1e-9 ? 1.6 : 1.0));
        painter.drawLine(QPointF(0, a.y()), QPointF(width(), b.y()));
        if (a.y() >= 0 && a.y() <= height()) {
            painter.drawText(QPointF(6, a.y() - 4), tr("%1").arg(y, 0, 'f', std::abs(y) < 10 ? 1 : 0));
        }
    }

    const QPointF limit = worldToScreen({m_maxTimeSeconds, m_maxPosition});
    painter.setPen(QPen(QColor(220, 150, 40), 1.4, Qt::DashLine));
    painter.drawLine(QPointF(limit.x(), 0), QPointF(limit.x(), height()));
    painter.drawLine(QPointF(0, limit.y()), QPointF(width(), limit.y()));
    const QPointF minLimit = worldToScreen({0.0, m_minPosition});
    painter.drawLine(QPointF(0, minLimit.y()), QPointF(width(), minLimit.y()));
}

void CurveCanvas::drawDraftStroke(QPainter &painter)
{
    if (m_draftPoints.empty()) {
        return;
    }

    QPainterPath path;
    path.moveTo(worldToScreen(m_draftPoints.front()));
    for (size_t i = 1; i < m_draftPoints.size(); ++i) {
        path.lineTo(worldToScreen(m_draftPoints[i]));
    }

    painter.setPen(QPen(QColor(40, 118, 210), 2.2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPath(path);
}

void CurveCanvas::drawControlMarkers(QPainter &painter)
{
    if (m_draftPoints.empty()) {
        return;
    }

    const QPointF start = worldToScreen(m_draftPoints.front());
    const QPointF end = worldToScreen(m_draftPoints.back());

    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(QColor(28, 145, 88));
    painter.drawEllipse(start, 6, 6);

    painter.setBrush(QColor(198, 73, 63));
    painter.drawEllipse(end, 6, 6);
}

void CurveCanvas::drawSliderPreview(QPainter &painter)
{
    if (!m_previewActive) {
        return;
    }

    const QPointF point = worldToScreen({m_previewTime, m_previewPosition});
    painter.setPen(QPen(QColor(190, 95, 210), 1.5, Qt::DashLine));
    painter.drawLine(QPointF(point.x(), 0), QPointF(point.x(), height()));
    painter.drawLine(QPointF(0, point.y()), QPointF(width(), point.y()));

    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(QColor(190, 95, 210));
    painter.drawEllipse(point, 7, 7);
}

void CurveCanvas::drawRangeLabels(QPainter &painter)
{
    painter.setPen(palette().color(QPalette::Text));
    const QPointF t0 = worldToScreen({0.0, 0.0});
    const QPointF tMax = worldToScreen({m_maxTimeSeconds, 0.0});
    const QPointF yMin = worldToScreen({0.0, m_minPosition});
    const QPointF yMax = worldToScreen({0.0, m_maxPosition});

    painter.drawText(QPointF(t0.x() + 4, height() - 8), tr("0s"));
    painter.drawText(QPointF(tMax.x() - 42, height() - 8), tr("%1s").arg(m_maxTimeSeconds, 0, 'f', 2));
    painter.drawText(QPointF(width() - 126, yMax.y() + 14), tr("Y Max %1").arg(m_maxPosition, 0, 'f', 2));
    painter.drawText(QPointF(width() - 126, yMin.y() - 6), tr("Y Min %1").arg(m_minPosition, 0, 'f', 2));
    painter.drawText(QPointF(m_leftMargin + 8, 18), tr("Y Axis: Velocity"));
}

void CurveCanvas::drawInfoOverlay(QPainter &painter)
{
    const QStringList lines = m_overlayCollapsed
        ? QStringList{tr("Curve Info  >")}
        : QStringList{
              tr("Curve Info  v"),
              tr("Time: --"),
              tr("Velocity(Y): --"),
              tr("Distance: --"),
              tr("Valid Range: --"),
          };

    const QFontMetrics fm(font());
    int widthHint = 150;
    for (const auto &line : lines) {
        widthHint = std::max(widthHint, fm.horizontalAdvance(line) + 24);
    }
    const QRect box(m_overlayTopLeft, QSize(widthHint, lines.size() * (fm.height() + 4) + 14));
    m_overlayRect = box;

    painter.setPen(QPen(palette().color(QPalette::Mid), 1));
    painter.setBrush(QColor(255, 255, 255, 218));
    painter.drawRoundedRect(box, 6, 6);

    const QRect dragHandle = overlayDragHandleRect();
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(210, 210, 210, 180));
    painter.drawRoundedRect(dragHandle.adjusted(3, 3, -3, -3), 5, 5);

    painter.setPen(QPen(QColor(120, 120, 120), 2, Qt::SolidLine, Qt::RoundCap));
    const int dotX = dragHandle.center().x();
    for (int dotY = dragHandle.top() + 11; dotY < dragHandle.bottom() - 8; dotY += 8) {
        painter.drawPoint(dotX, dotY);
    }

    painter.setPen(palette().color(QPalette::Text));
    int y = box.top() + 10 + fm.ascent();
    for (const auto &line : lines) {
        painter.drawText(box.left() + 24, y, line);
        y += fm.height() + 4;
    }
}

void CurveCanvas::toggleOverlay()
{
    m_overlayCollapsed = !m_overlayCollapsed;
    update();
}

QRect CurveCanvas::overlayDragHandleRect() const
{
    if (m_overlayRect.isNull()) {
        return {};
    }
    return QRect(m_overlayRect.left(), m_overlayRect.top(), 18, m_overlayRect.height());
}

void CurveCanvas::resetViewport()
{
    const double usableWidth = std::max(1.0, width() - m_leftMargin - m_rightMargin);
    const double usableHeight = std::max(1.0, height() - m_topMargin - m_bottomMargin);
    const double visibleRange = std::max(1.0, m_visibleMaxPosition - m_visibleMinPosition);

    m_view.scaleX = usableWidth / m_visibleTimeSeconds;
    m_view.scaleY = usableHeight / visibleRange;
    m_view.offsetX = m_leftMargin;
    m_view.offsetY = m_topMargin + m_visibleMaxPosition * m_view.scaleY;
}

void CurveCanvas::clampViewport()
{
    m_view.offsetX = m_leftMargin;
    m_view.offsetY = m_topMargin + m_visibleMaxPosition * m_view.scaleY;
}

bool CurveCanvas::appendDraftPoint(const QPointF &screenPoint)
{
    QPointF worldPoint = screenToWorld(screenPoint);
    if (worldPoint.x() < 0.0) {
        return true;
    }

    if (worldPoint.x() > m_maxTimeSeconds) {
        if (m_draftPoints.empty()) {
            return true;
        }
        worldPoint.setX(m_maxTimeSeconds);
    }
    worldPoint.setY(std::clamp(worldPoint.y(), m_minPosition, m_maxPosition));

    if (!m_draftPoints.empty()) {
        if (worldPoint.x() <= m_draftPoints.back().x()) {
            return true;
        }
        if (QLineF(worldToScreen(m_draftPoints.back()), screenPoint).length() <= 2.5) {
            return true;
        }
    }

    m_draftPoints.push_back(worldPoint);
    update();
    return true;
}
