#include "curvecanvas.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QWheelEvent>

#include <algorithm>
#include <cmath>

CurveCanvas::CurveCanvas(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAutoFillBackground(false);
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
    drawInfoOverlay(painter);
}

void CurveCanvas::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    m_lastMousePos = event->pos();

    if (event->button() == Qt::LeftButton) {
        m_drawing = true;
        m_draftPoints.push_back(screenToWorld(event->position()));
        update();
    } else if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void CurveCanvas::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint delta = event->pos() - m_lastMousePos;

    if (m_drawing) {
        const QPointF worldPoint = screenToWorld(event->position());
        if (m_draftPoints.empty() || QLineF(worldToScreen(m_draftPoints.back()), event->position()).length() > 2.5) {
            m_draftPoints.push_back(worldPoint);
            update();
        }
    } else if (m_panning || m_spacePan) {
        m_view.offsetX += delta.x();
        m_view.offsetY += delta.y();
        update();
    }

    m_lastMousePos = event->pos();
}

void CurveCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_drawing = false;
    } else if (event->button() == Qt::MiddleButton) {
        m_panning = false;
        unsetCursor();
    }
}

void CurveCanvas::wheelEvent(QWheelEvent *event)
{
    const QPointF before = screenToWorld(event->position());
    const double factor = event->angleDelta().y() > 0 ? 1.12 : 1.0 / 1.12;

    m_view.scaleX = std::clamp(m_view.scaleX * factor, 25.0, 500.0);
    m_view.scaleY = std::clamp(m_view.scaleY * factor, 20.0, 420.0);

    const QPointF afterScreen = worldToScreen(before);
    m_view.offsetX += event->position().x() - afterScreen.x();
    m_view.offsetY += event->position().y() - afterScreen.y();

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

    const int startX = static_cast<int>(std::floor(topLeft.x() / worldStepX)) - 1;
    const int endX = static_cast<int>(std::ceil(bottomRight.x() / worldStepX)) + 1;
    for (int i = startX; i <= endX; ++i) {
        const double x = i * worldStepX;
        const QPointF a = worldToScreen({x, topLeft.y()});
        const QPointF b = worldToScreen({x, bottomRight.y()});
        painter.setPen(QPen(std::abs(x) < 1e-9 ? axis : (i % 2 == 0 ? major : minor), std::abs(x) < 1e-9 ? 1.6 : 1.0));
        painter.drawLine(QPointF(a.x(), 0), QPointF(b.x(), height()));
    }

    const int startY = static_cast<int>(std::floor(bottomRight.y() / worldStepY)) - 1;
    const int endY = static_cast<int>(std::ceil(topLeft.y() / worldStepY)) + 1;
    for (int i = startY; i <= endY; ++i) {
        const double y = i * worldStepY;
        const QPointF a = worldToScreen({topLeft.x(), y});
        const QPointF b = worldToScreen({bottomRight.x(), y});
        painter.setPen(QPen(std::abs(y) < 1e-9 ? axis : (i % 2 == 0 ? major : minor), std::abs(y) < 1e-9 ? 1.6 : 1.0));
        painter.drawLine(QPointF(0, a.y()), QPointF(width(), b.y()));
    }
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

void CurveCanvas::drawInfoOverlay(QPainter &painter)
{
    const QStringList lines = m_overlayCollapsed
        ? QStringList{tr("Curve Info")}
        : QStringList{
              tr("Curve Info"),
              tr("Time: --"),
              tr("Velocity: --"),
              tr("Distance: --"),
              tr("Valid Range: --"),
          };

    const QFontMetrics fm(font());
    int widthHint = 150;
    for (const auto &line : lines) {
        widthHint = std::max(widthHint, fm.horizontalAdvance(line) + 24);
    }
    const QRect box(12, 12, widthHint, lines.size() * (fm.height() + 4) + 14);

    painter.setPen(QPen(palette().color(QPalette::Mid), 1));
    painter.setBrush(QColor(255, 255, 255, 218));
    painter.drawRoundedRect(box, 6, 6);

    painter.setPen(palette().color(QPalette::Text));
    int y = box.top() + 10 + fm.ascent();
    for (const auto &line : lines) {
        painter.drawText(box.left() + 12, y, line);
        y += fm.height() + 4;
    }
}

void CurveCanvas::toggleOverlay()
{
    m_overlayCollapsed = !m_overlayCollapsed;
    update();
}
