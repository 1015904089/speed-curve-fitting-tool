#include "motionslider.h"

#include <QMouseEvent>
#include <QPainter>
#include <QtGlobal>

#include <algorithm>

MotionSlider::MotionSlider(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(46);
    setMouseTracking(true);
}

double MotionSlider::normalizedPosition() const
{
    return m_position;
}

void MotionSlider::setNormalizedPosition(double value)
{
    const double clamped = std::clamp(value, 0.0, 1.0);
    if (qFuzzyCompare(m_position, clamped)) {
        return;
    }

    m_position = clamped;
    update();
    emit normalizedPositionChanged(m_position);
}

void MotionSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF track = trackRect();
    painter.setPen(QPen(palette().color(QPalette::Mid), 1));
    painter.setBrush(palette().color(QPalette::Button));
    painter.drawRoundedRect(track, 6, 6);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(72, 135, 210));
    painter.drawRoundedRect(QRectF(track.left(), track.top(), track.width() * m_position, track.height()), 6, 6);

    const double handleX = track.left() + track.width() * m_position;
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(QColor(55, 105, 180));
    painter.drawEllipse(QPointF(handleX, track.center().y()), 10, 10);
}

void MotionSlider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    m_dragging = true;
    updateFromMouse(event->position().x());
    emit dragStarted();
}

void MotionSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        updateFromMouse(event->position().x());
    }
}

void MotionSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !m_dragging) {
        return;
    }

    updateFromMouse(event->position().x());
    m_dragging = false;
    emit dragFinished();
}

QRectF MotionSlider::trackRect() const
{
    return QRectF(16, 12, std::max(1, width() - 32), 14);
}

void MotionSlider::updateFromMouse(double x)
{
    const QRectF track = trackRect();
    const double normalized = (x - track.left()) / track.width();
    setNormalizedPosition(normalized);
}
