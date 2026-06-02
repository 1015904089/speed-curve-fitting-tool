#pragma once

#include <QElapsedTimer>
#include <QWidget>

class QLabel;
class QSlider;
class QTimer;

class SliderPanel final : public QWidget
{
    Q_OBJECT

public:
    explicit SliderPanel(QWidget *parent = nullptr);

signals:
    void recordingStateChanged(bool recording);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setRecording(bool recording);
    void updateReadout();

    QSlider *m_slider = nullptr;
    QLabel *m_timeLabel = nullptr;
    QLabel *m_positionLabel = nullptr;
    QLabel *m_hintLabel = nullptr;
    QTimer *m_timer = nullptr;
    QElapsedTimer m_elapsed;
    bool m_recording = false;
};
