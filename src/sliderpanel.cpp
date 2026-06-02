#include "sliderpanel.h"

#include "motionslider.h"

#include <QDoubleSpinBox>
#include <QFrame>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QtGlobal>
#include <QVBoxLayout>

SliderPanel::SliderPanel(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(10);

    auto *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);

    auto *title = new QLabel(tr("Slider Recording Area"), this);
    title->setObjectName("sectionTitle");
    m_resetButton = new QPushButton(tr("Reset"), this);
    headerLayout->addWidget(title);
    headerLayout->addStretch(1);
    headerLayout->addWidget(m_resetButton);

    m_hintLabel = new QLabel(tr("Press and drag the one-dimensional slider to record position. The curve canvas Y axis is reserved for velocity."), this);
    m_hintLabel->setWordWrap(true);

    auto *inputFrame = new QFrame(this);
    auto *inputLayout = new QFormLayout(inputFrame);
    inputLayout->setContentsMargins(0, 0, 0, 0);

    m_timeInput = new QDoubleSpinBox(inputFrame);
    m_timeInput->setRange(0.1, 3600.0);
    m_timeInput->setDecimals(2);
    m_timeInput->setSuffix(tr(" s"));
    m_timeInput->setValue(10.0);

    m_minPositionInput = new QDoubleSpinBox(inputFrame);
    m_minPositionInput->setRange(-100000000.0, 100000000.0);
    m_minPositionInput->setDecimals(2);
    m_minPositionInput->setValue(0.0);

    m_maxPositionInput = new QDoubleSpinBox(inputFrame);
    m_maxPositionInput->setRange(-100000000.0, 100000000.0);
    m_maxPositionInput->setDecimals(2);
    m_maxPositionInput->setValue(100.0);

    inputLayout->addRow(tr("Max Time"), m_timeInput);
    inputLayout->addRow(tr("Y Min"), m_minPositionInput);
    inputLayout->addRow(tr("Y Max"), m_maxPositionInput);

    m_slider = new MotionSlider(this);

    auto *readoutFrame = new QFrame(this);
    auto *readoutLayout = new QVBoxLayout(readoutFrame);
    readoutLayout->setContentsMargins(0, 0, 0, 0);

    m_timeLabel = new QLabel(this);
    m_positionLabel = new QLabel(this);
    readoutLayout->addWidget(m_timeLabel);
    readoutLayout->addWidget(m_positionLabel);

    layout->addLayout(headerLayout);
    layout->addWidget(m_hintLabel);
    layout->addWidget(inputFrame);
    layout->addWidget(m_slider);
    layout->addWidget(readoutFrame);
    layout->addStretch(1);

    m_timer = new QTimer(this);
    m_timer->setInterval(33);
    connect(m_timer, &QTimer::timeout, this, &SliderPanel::updateReadout);
    connect(m_slider, &MotionSlider::normalizedPositionChanged, this, &SliderPanel::updateReadout);
    connect(m_slider, &MotionSlider::dragStarted, this, [this]() { setRecording(true); });
    connect(m_slider, &MotionSlider::dragFinished, this, [this]() { setRecording(false); });
    connect(m_resetButton, &QPushButton::clicked, this, &SliderPanel::resetRequested);
    connect(m_timeInput, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double) { emitLimitsChanged(); });
    connect(m_minPositionInput, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double) { emitLimitsChanged(); });
    connect(m_maxPositionInput, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double) { emitLimitsChanged(); });

    updateReadout();
    emitLimitsChanged();
}

double SliderPanel::maxTimeSeconds() const
{
    return m_timeInput->value();
}

double SliderPanel::minPosition() const
{
    return m_minPositionInput->value();
}

double SliderPanel::maxPosition() const
{
    const double minValue = minPosition();
    const double maxValue = m_maxPositionInput->value();
    return maxValue <= minValue ? minValue + 1.0 : maxValue;
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
    const double previewTime = std::clamp(seconds, 0.0, maxTimeSeconds());
    const double normalized = m_slider->normalizedPosition();
    const double position = minPosition() + normalized * (maxPosition() - minPosition());

    m_timeLabel->setText(tr("Recording Time: %1 s").arg(seconds, 0, 'f', 2));
    m_positionLabel->setText(tr("Recorded Position: %1 (%2)").arg(position, 0, 'f', 2).arg(normalized, 0, 'f', 3));
    emit previewChanged(previewTime, position, m_recording || normalized > 0.0);
}

void SliderPanel::resetPanel()
{
    m_slider->setNormalizedPosition(0.0);
    m_timeInput->setValue(10.0);
    m_minPositionInput->setValue(0.0);
    m_maxPositionInput->setValue(100.0);
    setRecording(false);
    emit previewChanged(0.0, minPosition(), false);
    emitLimitsChanged();
    updateReadout();
}

void SliderPanel::emitLimitsChanged()
{
    emit limitsChanged(maxTimeSeconds(), minPosition(), maxPosition());
    updateReadout();
}
