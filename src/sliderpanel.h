#pragma once

#include <QElapsedTimer>
#include <QWidget>

class QLabel;
class QDoubleSpinBox;
class QPushButton;
class MotionSlider;
class QTimer;

class SliderPanel final : public QWidget
{
    Q_OBJECT

public:
    explicit SliderPanel(QWidget *parent = nullptr);
    double maxTimeSeconds() const;
    double minPosition() const;
    double maxPosition() const;

signals:
    void recordingStateChanged(bool recording);
    void limitsChanged(double maxTimeSeconds, double minPosition, double maxPosition);
    void previewChanged(double timeSeconds, double position, bool active);
    void resetRequested();

public slots:
    void resetPanel();

private:
    void setRecording(bool recording);
    void updateReadout();
    void emitLimitsChanged();

    MotionSlider *m_slider = nullptr;
    QDoubleSpinBox *m_timeInput = nullptr;
    QDoubleSpinBox *m_minPositionInput = nullptr;
    QDoubleSpinBox *m_maxPositionInput = nullptr;
    QPushButton *m_resetButton = nullptr;
    QLabel *m_timeLabel = nullptr;
    QLabel *m_positionLabel = nullptr;
    QLabel *m_hintLabel = nullptr;
    QTimer *m_timer = nullptr;
    QElapsedTimer m_elapsed;
    bool m_recording = false;
};
