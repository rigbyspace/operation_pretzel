#include "MainWindow.hpp"

#include "AnalyzerPanel.hpp"
#include "EngineConfigPanel.hpp"
#include "ExecutionPanel.hpp"
#include "FingerprintPanel.hpp"
#include "OutputTableWidget.hpp"
#include "PhaseMapExplorer.hpp"
#include "RhythmVisualizerWidget.hpp"
#include "StackViewerWidget.hpp"
#include "TRTSCoreProcess.hpp"
#include "TheoristExplorerPanel.hpp"

#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextStream>
#include <QToolBar>
#include <QColor>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_process(new TRTSCoreProcess(this)) {
    buildUi();
    connectSignals();
    setWindowTitle(tr("TRTS Lab Interface"));
}

MainWindow::~MainWindow() = default;

void MainWindow::buildUi() {
    m_tabs = new QTabWidget(this);
    setCentralWidget(m_tabs);

    m_engineConfig = new EngineConfigPanel(this);
    m_execution = new ExecutionPanel(this);
    m_rhythm = new RhythmVisualizerWidget(this);
    m_stack = new StackViewerWidget(this);
    m_analyzer = new AnalyzerPanel(this);
    m_fingerprint = new FingerprintPanel(this);
    m_phaseMap = new PhaseMapExplorer(this);
    m_theorist = new TheoristExplorerPanel(this);
    m_outputTable = new OutputTableWidget(this);

    m_tabs->addTab(m_engineConfig, tr("Engine configuration"));
    m_tabs->addTab(m_execution, tr("Execution"));
    m_tabs->addTab(m_rhythm, tr("ψ rhythm"));
    m_tabs->addTab(m_stack, tr("Stack"));
    m_tabs->addTab(m_analyzer, tr("Analyzer"));
    m_tabs->addTab(m_fingerprint, tr("Fingerprint"));
    m_tabs->addTab(m_phaseMap, tr("Phase map"));
    m_tabs->addTab(m_theorist, tr("Theorist"));
    m_tabs->addTab(m_outputTable, tr("Output table"));

    auto *toolBar = addToolBar(tr("Controls"));
    m_actionStart = toolBar->addAction(tr("Start"));
    m_actionStop = toolBar->addAction(tr("Stop"));
    m_actionReset = toolBar->addAction(tr("Reset"));
    toolBar->addSeparator();
    m_actionClear = toolBar->addAction(tr("Clear state"));
    toolBar->addSeparator();
    m_actionLoadConfig = toolBar->addAction(tr("Load config"));

    statusBar();
    m_statusLabel = new QLabel(tr("Idle"), this);
    statusBar()->addPermanentWidget(m_statusLabel, 1);
}

void MainWindow::connectSignals() {
    connect(m_actionStart, &QAction::triggered, this, &MainWindow::handleStartRun);
    connect(m_actionStop, &QAction::triggered, this, &MainWindow::handleStopRun);
    connect(m_actionReset, &QAction::triggered, this, &MainWindow::handleReset);
    connect(m_actionClear, &QAction::triggered, this, [this]() {
        m_execution->resetState();
        m_rhythm->clearEvents();
        m_stack->clear();
        m_analyzer->clearResults();
        m_fingerprint->clear();
        m_phaseMap->clear();
        m_theorist->clear();
        m_outputTable->clear();
        logStatus(tr("Cleared in-memory state"));
    });
    connect(m_actionLoadConfig, &QAction::triggered, this, &MainWindow::loadConfigFromFile);

    connect(m_engineConfig, &EngineConfigPanel::configurationChanged, this, [this](const TRTSConfig &config) {
        Q_UNUSED(config);
        logStatus(tr("Configuration updated"));
    });
    connect(m_engineConfig, &EngineConfigPanel::requestLoadConfig, this, &MainWindow::handleLoadConfigRequested);

    connect(m_execution, &ExecutionPanel::runRequested, this, &MainWindow::handleStartRun);
    connect(m_execution, &ExecutionPanel::stopRequested, this, &MainWindow::handleStopRun);
    connect(m_execution, &ExecutionPanel::resetRequested, this, &MainWindow::handleReset);
    connect(m_execution, &ExecutionPanel::stepRequested, this, [this]() {
        logStatus(tr("Step requested (not yet implemented)"));
    });

    connect(m_process, &TRTSCoreProcess::engineOutput, this, &MainWindow::handleEngineOutput);
    connect(m_process, &TRTSCoreProcess::engineError, this, &MainWindow::handleEngineError);
    connect(m_process, &TRTSCoreProcess::runFinished, this, &MainWindow::handleRunFinished);
    connect(m_process, &TRTSCoreProcess::statusMessage, this, &MainWindow::logStatus);
}

void MainWindow::handleStartRun() {
    const TRTSConfig config = m_engineConfig->configuration();
    if (!m_process->startRun(config)) {
        return;
    }
    m_execution->resetState();
    m_outputTable->clear();
    m_rhythm->clearEvents();
    m_stack->clear();
}

void MainWindow::handleStopRun() {
    m_process->stopRun();
}

void MainWindow::handleReset() {
    if (m_process->isRunning()) {
        m_process->stopRun();
    }
    m_execution->resetState();
    m_outputTable->clear();
    m_rhythm->clearEvents();
    m_stack->clear();
    logStatus(tr("Engine reset"));
}

void MainWindow::handleEngineOutput(const QString &line) {
    parseEngineLine(line);
}

void MainWindow::handleEngineError(const QString &line) {
    logStatus(tr("Engine error: %1").arg(line));
}

void MainWindow::handleRunFinished(int exitCode, QProcess::ExitStatus status) {
    Q_UNUSED(status);
    logStatus(tr("Engine finished with exit code %1").arg(exitCode));
}

void MainWindow::handleLoadConfigRequested() {
    loadConfigFromFile();
}

void MainWindow::loadConfigFromFile() {
    const QString path = QFileDialog::getOpenFileName(this, tr("Select TRTS config"), QString(), tr("TRTS config (*.trtscfg)"));
    if (path.isEmpty()) {
        return;
    }
    m_engineConfig->loadConfigurationFromFile(path);
    logStatus(tr("Loaded configuration from %1").arg(QFileInfo(path).fileName()));
}

void MainWindow::parseEngineLine(const QString &line) {
    // Expected format example (subject to engine implementation):
    // TICK;MT;UPSILON;BETA;KOPPA;PSI;RHO;STACK;COLOR;PSI_TYPE
    const QStringList columns = line.split(';');
    if (columns.size() < 8) {
        appendLogEntry({line});
        return;
    }

    const QString tick = columns.at(0);
    const QString microTick = columns.at(1);
    const QString upsilon = columns.at(2);
    const QString beta = columns.at(3);
    const QString koppa = columns.at(4);
    const QString psiState = columns.at(5);
    const QString rhoState = columns.at(6);
    const QString stackDepth = columns.at(7);
    QString psiMode = columns.size() > 9 ? columns.at(9) : QString();

    m_execution->updateState(tick, upsilon, beta, koppa, psiState.contains("FIRE"), stackDepth.toInt(), rhoState,
                             psiMode);
    m_execution->appendLogRow(columns.mid(0, std::min(columns.size(), 8)));
    m_outputTable->appendRow(columns.mid(0, std::min(columns.size(), 9)));

    if (columns.size() >= 10) {
        RhythmEvent event;
        event.tick = tick.toInt();
        event.microTick = microTick.toInt();
        event.psiType = columns.at(9);
        event.color = QColor(columns.size() > 10 ? columns.at(10) : "#ff8800");
        m_rhythm->appendEvent(event);
    }
}

void MainWindow::appendLogEntry(const QStringList &columns) {
    m_outputTable->appendRow(columns);
}

void MainWindow::logStatus(const QString &message) {
    m_statusLabel->setText(message);
}

