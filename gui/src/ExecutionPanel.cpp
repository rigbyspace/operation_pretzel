// ExecutionPanel.cpp
// Implements the ExecutionPanel, which provides engine controls and state
// display.  This version adds a pause button alongside the existing run,
// stop, step and reset controls.  When clicked, the pause button emits
// pauseRequested(); it does not pause the engine itself but allows the
// surrounding MainWindow to implement pausing logic if supported.

#include "../include/ExecutionPanel.hpp"
#include "../include/MainWindow.hpp"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QAbstractItemView>

ExecutionPanel::ExecutionPanel(QWidget *parent)
    : QWidget(parent)
{
    buildUi();
    resetState();
    // Hook live updates from MainWindow.  If the parent is nested under a
    // MainWindow, connect the engineUpdate signal so that updateFromEngine()
    // receives streaming updates.
    if (auto *mw = qobject_cast<MainWindow*>(parent ? parent->parentWidget() : nullptr)) {
        connect(mw, &MainWindow::engineUpdate,
                this, &ExecutionPanel::updateFromEngine);
    }
}

void ExecutionPanel::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    // State display row
    auto *stateRow = new QHBoxLayout();
    auto stateLabel = [stateRow](const QString &title, QLabel **labelOut) {
        auto *container   = new QWidget();
        auto *innerLayout = new QVBoxLayout(container);
        innerLayout->setContentsMargins(0, 0, 0, 0);
        auto *titleLabel = new QLabel(title, container);
        auto *valueLabel = new QLabel(container);
        titleLabel->setProperty("class", "title");
        valueLabel->setProperty("class", "value");
        innerLayout->addWidget(titleLabel);
        innerLayout->addWidget(valueLabel);
        container->setLayout(innerLayout);
        stateRow->addWidget(container);
        *labelOut = valueLabel;
    };
    stateLabel(tr("Tick"),       &m_tickLabel);
    stateLabel(tr("υ"),          &m_upsilonLabel);
    stateLabel(tr("β"),          &m_betaLabel);
    stateLabel(tr("κ"),          &m_koppaLabel);
    stateLabel(tr("ρ"),          &m_rhoLabel);
    stateLabel(tr("ψ"),          &m_psiLabel);
    stateLabel(tr("Stack depth"), &m_stackLabel);
    // Button row
    auto *buttonRow = new QHBoxLayout();
    m_stepButton   = new QPushButton(tr("Step"), this);
    m_runButton    = new QPushButton(tr("Run"),  this);
    m_stopButton   = new QPushButton(tr("Stop"), this);
    m_resetButton  = new QPushButton(tr("Reset"),this);
    m_pauseButton  = new QPushButton(tr("Pause"),this);
    buttonRow->addWidget(m_stepButton);
    buttonRow->addWidget(m_runButton);
    buttonRow->addWidget(m_pauseButton);
    buttonRow->addWidget(m_stopButton);
    buttonRow->addWidget(m_resetButton);
    buttonRow->addStretch();
    // Connect buttons to signals
    connect(m_stepButton,  &QPushButton::clicked, this, &ExecutionPanel::stepRequested);
    connect(m_runButton,   &QPushButton::clicked, this, &ExecutionPanel::runRequested);
    connect(m_stopButton,  &QPushButton::clicked, this, &ExecutionPanel::stopRequested);
    connect(m_resetButton, &QPushButton::clicked, this, &ExecutionPanel::resetRequested);
    connect(m_pauseButton, &QPushButton::clicked, this, &ExecutionPanel::pauseRequested);
    // Log table
    m_logTable = new QTableWidget(this);
    m_logTable->setColumnCount(8);
    m_logTable->setHorizontalHeaderLabels({tr("Tick"), tr("MT"), tr("υ"), tr("β"), tr("κ"), tr("ψ"), tr("ρ"), tr("Stack")});
    m_logTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_logTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Assemble layout
    layout->addLayout(stateRow);
    layout->addLayout(buttonRow);
    layout->addWidget(m_logTable);
}

void ExecutionPanel::updateState(const QString &tick,
                                 const QString &upsilon,
                                 const QString &beta,
                                 const QString &koppa,
                                 bool psiFired,
                                 int stackDepth,
                                 const QString &rhoSource,
                                 const QString &psiMode)
{
    m_tickLabel->setText(tick);
    m_upsilonLabel->setText(upsilon);
    m_betaLabel->setText(beta);
    m_koppaLabel->setText(koppa);
    m_rhoLabel->setText(rhoSource);
    m_psiLabel->setText(psiFired ? tr("ψ fired (%1)").arg(psiMode)
                                 : tr("ψ idle (%1)").arg(psiMode));
    m_stackLabel->setText(QString::number(stackDepth));
}

void ExecutionPanel::appendLogRow(const QStringList &row)
{
    const int rowIndex = m_logTable->rowCount();
    m_logTable->insertRow(rowIndex);
    for (int i = 0; i < row.size(); ++i) {
        m_logTable->setItem(rowIndex, i, new QTableWidgetItem(row.at(i)));
    }
    m_logTable->scrollToBottom();
}

void ExecutionPanel::resetState()
{
    m_tickLabel->setText("0");
    m_upsilonLabel->clear();
    m_betaLabel->clear();
    m_koppaLabel->clear();
    m_rhoLabel->setText(tr("ρ idle"));
    m_psiLabel->setText(tr("ψ idle"));
    m_stackLabel->setText("0");
    m_logTable->setRowCount(0);
}

// Slot for streaming updates: update the state and log a row.  Note that the
// engine does not currently emit psiMode or stack size, so those fields are
// left empty or zero in the log row.  If future versions of the engine emit
// more detailed information, this slot should be updated accordingly.
void ExecutionPanel::updateFromEngine(size_t tick,
                                      int microtick,
                                      char /*phase*/,
                                      bool rho,
                                      bool psi,
                                      bool /*mu_zero*/,
                                      bool /*forced*/)
{
    updateState(QString::number(tick), QString(), QString(), QString(),
                psi, 0, rho ? QStringLiteral("ρ=1") : QStringLiteral("ρ=0"), QString());
    appendLogRow({
        QString::number(tick),
        QString::number(microtick),
        "", "", "",
        psi ? "1" : "0",
        rho ? "1" : "0",
        "0"
    });
}
