#include "sliderpanel.h"

#include <QEvent>
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>

SliderPanel::SliderPanel(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(10);

    auto *title = new QLabel(tr("Slider Recording Area"), this);
    title->setObjectName("sectionTitle");

    m_hintLabel = new QLabel(tr("Press and drag the one-dimensional slider to record a draft motion."), this);
    m_hintLabel->setWordWrap(true);

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(0, 1000);
    m_slider->setValue(0);
    m_slider->installEventFilter(this);

    auto *readoutFrame = new QFrame(this);
    auto *readoutLayout = new QVBoxLayout(readoutFrame);
    readoutLayout->setContentsMargins(0, 0, 0, 0);

    m_timeLabel = new QLabel(this);
    m_positionLabel = new QLabel(this);
    readoutLayout->addWidget(m_timeLabel);
    readoutLayout->addWidget(m_positionLabel);

    layout->addWidget(title);
    layout->addWidget(m_hintLabel);
    layout->addWidget(m_slider);
    layout->addWidget(readoutFrame);
    layout->addStretch(1);

    m_timer = new QTimer(this);
    m_timer->setInterval(33);
    connect(m_timer, &QTimer::timeout, this, &SliderPanel::updateReadout);
    connect(m_slider, &QSlider::valueChanged, this, &SliderPanel::updateReadout);

    updateReadout();
}

bool SliderPanel::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_slider) {
        if (event->type() == QEvent::MouseButtonPress) {
            setRecording(true);
        } else if (event->type() == QEvent::MouseButtonRelease) {
            setRecording(false);
        }
    }

    return QWidget::eventFilter(watched, event);
}

void SliderPanel::setRecording(bool recording)
{
    if (m_recording == recording) {
        return;
    }

    m_recording = recording;
    if (m_recording) {
        m_elapsed.restart();
        m_timer->start();
    } else {
        m_timer->stop();
    }

    updateReadout();
    emit recordingStateChanged(m_recording);
}

void SliderPanel::updateReadout()
{
    const double seconds = m_recording ? m_elapsed.elapsed() / 1000.0 : 0.0;
    const double normalized = m_slider->value() / 1000.0;

    m_timeLabel->setText(tr("Recording Time: %1 s").arg(seconds, 0, 'f', 2));
    m_positionLabel->setText(tr("Slider Position: %1").arg(normalized, 0, 'f', 3));
}
