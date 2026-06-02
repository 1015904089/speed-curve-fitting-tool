#pragma once

#include <QMainWindow>
#include "restorablesplitter.h"

class CurveCanvas;
class ResultPanel;
class SliderPanel;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void restoreLayout();
    void saveLayout() const;
    void restoreDefaultLayout();
    void resetWorkspace();

    RestorableSplitter *m_mainSplitter = nullptr;
    RestorableSplitter *m_leftSplitter = nullptr;
    CurveCanvas *m_curveCanvas = nullptr;
    SliderPanel *m_sliderPanel = nullptr;
    ResultPanel *m_resultPanel = nullptr;
};
