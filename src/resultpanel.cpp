#include "resultpanel.h"

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

ResultPanel::ResultPanel(QWidget *parent)
    : QWidget(parent)
{
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    auto *content = new QWidget(scrollArea);
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    auto *title = new QLabel(tr("Result Display Area"), content);
    title->setObjectName("sectionTitle");

    auto *fitGroup = new QGroupBox(tr("Fit Placeholder"), content);
    auto *fitLayout = new QFormLayout(fitGroup);
    auto *curveType = new QComboBox(fitGroup);
    curveType->addItems({tr("Not selected"), tr("Polynomial"), tr("SmoothStep"), tr("Logistic"), tr("Exponential"), tr("Auto PySR")});
    fitLayout->addRow(tr("Curve Type"), curveType);
    fitLayout->addRow(tr("Formula"), new QLabel(tr("None"), fitGroup));
    fitLayout->addRow(tr("RMSE"), new QLabel(tr("--"), fitGroup));
    fitLayout->addRow(tr("Complexity"), new QLabel(tr("--"), fitGroup));

    auto *previewGroup = new QGroupBox(tr("Motion Preview Placeholder"), content);
    auto *previewLayout = new QVBoxLayout(previewGroup);
    auto *previewText = new QLabel(tr("Preview will be enabled in V2."), previewGroup);
    previewText->setWordWrap(true);
    auto *playButton = new QPushButton(tr("Play"), previewGroup);
    auto *resetButton = new QPushButton(tr("Reset"), previewGroup);
    previewLayout->addWidget(previewText);
    previewLayout->addWidget(playButton);
    previewLayout->addWidget(resetButton);

    layout->addWidget(title);
    layout->addWidget(fitGroup);
    layout->addWidget(previewGroup);
    layout->addStretch(1);

    scrollArea->setWidget(content);
    outerLayout->addWidget(scrollArea);
}
