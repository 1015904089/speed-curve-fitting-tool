#include "mainwindow.h"

#include "curvecanvas.h"
#include "resultpanel.h"
#include "sliderpanel.h"

#include <QCloseEvent>
#include <QSettings>
#include <QShortcut>
#include <QStatusBar>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Speed Curve Fitting Tool"));
    resize(1280, 760);

    m_mainSplitter = new RestorableSplitter(Qt::Horizontal, this);
    m_leftSplitter = new RestorableSplitter(Qt::Vertical, m_mainSplitter);

    m_curveCanvas = new CurveCanvas(m_leftSplitter);
    m_sliderPanel = new SliderPanel(m_leftSplitter);
    m_resultPanel = new ResultPanel(m_mainSplitter);

    m_curveCanvas->setMinimumSize(520, 320);
    m_sliderPanel->setMinimumSize(520, 150);
    m_resultPanel->setMinimumWidth(280);

    m_leftSplitter->addWidget(m_curveCanvas);
    m_leftSplitter->addWidget(m_sliderPanel);
    m_mainSplitter->addWidget(m_leftSplitter);
    m_mainSplitter->addWidget(m_resultPanel);

    setCentralWidget(m_mainSplitter);
    statusBar()->showMessage(tr("V1 whiteboard prototype"));

    connect(m_sliderPanel, &SliderPanel::recordingStateChanged, this, [this](bool recording) {
        statusBar()->showMessage(recording ? tr("Recording slider draft...") : tr("Slider recording stopped"));
    });
    connect(m_sliderPanel, &SliderPanel::limitsChanged, m_curveCanvas, &CurveCanvas::setSceneLimits);
    connect(m_sliderPanel, &SliderPanel::previewChanged, m_curveCanvas, &CurveCanvas::setSliderPreview);
    connect(m_curveCanvas, &CurveCanvas::resetRequested, this, &MainWindow::resetWorkspace);
    connect(m_sliderPanel, &SliderPanel::resetRequested, this, &MainWindow::resetWorkspace);

    auto *resetShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_0), this);
    connect(resetShortcut, &QShortcut::activated, this, &MainWindow::restoreDefaultLayout);
    connect(m_mainSplitter, &RestorableSplitter::handleDoubleClicked, this, &MainWindow::restoreDefaultLayout);
    connect(m_leftSplitter, &RestorableSplitter::handleDoubleClicked, this, &MainWindow::restoreDefaultLayout);

    restoreLayout();
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveLayout();
    QMainWindow::closeEvent(event);
}

void MainWindow::restoreLayout()
{
    QSettings settings;
    const auto mainState = settings.value("layout/mainSplitter").toByteArray();
    const auto leftState = settings.value("layout/leftSplitter").toByteArray();

    if (!mainState.isEmpty()) {
        m_mainSplitter->restoreState(mainState);
    } else {
        m_mainSplitter->setSizes({900, 380});
    }

    if (!leftState.isEmpty()) {
        m_leftSplitter->restoreState(leftState);
    } else {
        m_leftSplitter->setSizes({490, 270});
    }
}

void MainWindow::saveLayout() const
{
    QSettings settings;
    settings.setValue("layout/mainSplitter", m_mainSplitter->saveState());
    settings.setValue("layout/leftSplitter", m_leftSplitter->saveState());
}

void MainWindow::restoreDefaultLayout()
{
    m_mainSplitter->setSizes({900, 380});
    m_leftSplitter->setSizes({490, 270});
    statusBar()->showMessage(tr("Default layout restored"));
}

void MainWindow::resetWorkspace()
{
    m_sliderPanel->resetPanel();
    m_curveCanvas->resetCanvas();
    statusBar()->showMessage(tr("Workspace reset"));
}
