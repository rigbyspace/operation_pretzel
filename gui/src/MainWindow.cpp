// gui/src/MainWindow.cpp
// Implements the MainWindow class.  This version fixes a crash caused by
// reinterpreting a TRTSConfig as a Config by instead constructing a Config
// explicitly and copying each field.  It also adds a Pause control.

#include "../include/MainWindow.hpp"
#include "stdio.h"
// Step 1: Include all C headers to get the type definitions (Config, etc.).
#include "../../simulate.h"
#include "../../state.h"
#include "../../config.h"

// Step 2: Use a surgical extern "C" block for *only* the failing prototypes.
// This is the CRITICAL fix that guarantees C linkage for config_init/config_clear.
extern "C" {
    void config_init(Config *config);
    void config_clear(Config *config);
}

#include "../include/EngineConfigPanel.hpp"
#include "../include/ExecutionPanel.hpp"

#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QStatusBar>
#include <QTabWidget>
#include <QToolBar>
#include <QStringList>

// Forward declare helper for parsing rationals
static bool parseRational(const QString &text, mpq_t out);

// Constructor / destructor
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_process(new TRTSCoreProcess(this))
{
    buildUi();
    connectSignals();
    setWindowTitle(tr("TRTS Lab Interface"));
}

MainWindow::~MainWindow() = default;

// Size hints
QSize MainWindow::minimumSizeHint() const {
    return QSize(800, 600);
}

QSize MainWindow::sizeHint() const {
    return QSize(1024, 768);
}

// UI setup
void MainWindow::buildUi()
{
    m_tabs = new QTabWidget(this);
    setCentralWidget(m_tabs);

    m_engineConfig = new EngineConfigPanel(this);
    m_execution    = new ExecutionPanel(this);
    // The following panel pointers are left uninitialized; in a full
    // application they would be constructed here.  Declaring them prevents
    // compilation errors when referenced in member variables.
    m_rhythm       = nullptr;
    m_stack        = nullptr;
    m_analyzer     = nullptr;
    m_fingerprint  = nullptr;
    m_phaseMap     = nullptr;
    m_theorist     = nullptr;
    m_outputTable  = nullptr;

    m_tabs->addTab(m_engineConfig, tr("Engine configuration"));
    m_tabs->addTab(m_execution,    tr("Execution"));
    // Additional tabs for other panels would be added here when they exist.

    auto *toolBar = addToolBar(tr("Controls"));
    m_actionStart      = toolBar->addAction(tr("Start"));
    m_actionStop       = toolBar->addAction(tr("Stop"));
    m_actionPause      = toolBar->addAction(tr("Pause"));
    m_actionReset      = toolBar->addAction(tr("Reset"));
    toolBar->addSeparator();
    m_actionClear      = toolBar->addAction(tr("Clear state"));
    toolBar->addSeparator();
    m_actionLoadConfig = toolBar->addAction(tr("Load config"));

    statusBar();
    m_statusLabel = new QLabel(tr("Idle"), this);
    statusBar()->addPermanentWidget(m_statusLabel, 1);
}

// Signal wiring
void MainWindow::connectSignals()
{
    connect(m_actionStart,      &QAction::triggered, this, &MainWindow::handleStartRun);
    connect(m_actionStop,       &QAction::triggered, this, &MainWindow::handleStopRun);
    connect(m_actionPause,      &QAction::triggered, this, &MainWindow::handlePause);
    connect(m_actionReset,      &QAction::triggered, this, &MainWindow::handleReset);
    connect(m_actionClear,      &QAction::triggered, this, &MainWindow::handleClearOutput);
    connect(m_actionLoadConfig, &QAction::triggered, this, &MainWindow::handleLoadConfigRequested);

    connect(m_engineConfig, &EngineConfigPanel::configurationChanged,
            this, [this](const TRTSConfig &cfg) {
                Q_UNUSED(cfg);
                logStatus(tr("Configuration updated"));
            });
    connect(m_engineConfig, &EngineConfigPanel::requestLoadConfig,
            this, &MainWindow::handleLoadConfigRequested);

    connect(m_execution, &ExecutionPanel::runRequested,   this, &MainWindow::handleStartRun);
    connect(m_execution, &ExecutionPanel::stopRequested,  this, &MainWindow::handleStopRun);
    connect(m_execution, &ExecutionPanel::pauseRequested, this, &MainWindow::handlePause);
    connect(m_execution, &ExecutionPanel::resetRequested, this, &MainWindow::handleReset);
    connect(m_execution, &ExecutionPanel::stepRequested,  this, &MainWindow::handleStartRun);
}

// Start run: convert TRTSConfig into Config and invoke streaming simulation
void MainWindow::handleStartRun()
{
    const TRTSConfig cfg = m_engineConfig->configuration();
    // Construct and initialize a fresh Config
    Config c;
    config_init(&c);
    // Map enums
    c.psi_mode = static_cast<PsiMode>(cfg.psiMode);
    c.koppa_mode = static_cast<KoppaMode>(cfg.koppaMode);
    c.engine_mode = static_cast<EngineMode>(cfg.engineMode);
    c.engine_upsilon = static_cast<EngineTrackMode>(cfg.upsilonTrack);
    c.engine_beta = static_cast<EngineTrackMode>(cfg.betaTrack);
    c.koppa_trigger = static_cast<KoppaTrigger>(cfg.koppaTrigger);
    c.prime_target = static_cast<PrimeTarget>(cfg.primeTarget);
    c.mt10_behavior = static_cast<Mt10Behavior>(cfg.mt10Behavior);
    c.ratio_trigger_mode = static_cast<RatioTriggerMode>(cfg.ratioTriggerMode);
    c.sign_flip_mode = static_cast<SignFlipMode>(cfg.signFlipMode);
    // Tick count
    c.ticks = cfg.tickCount;
    // Parse seeds into mpq values.  parseRational returns true on success.
    if (!parseRational(cfg.upsilonSeed, c.initial_upsilon)) {
        logStatus(tr("Invalid υ seed: %1").arg(cfg.upsilonSeed));
    }
    if (!parseRational(cfg.betaSeed, c.initial_beta)) {
        logStatus(tr("Invalid β seed: %1").arg(cfg.betaSeed));
    }
    if (!parseRational(cfg.koppaSeed, c.initial_koppa)) {
        logStatus(tr("Invalid κ seed: %1").arg(cfg.koppaSeed));
    }
    // Copy boolean flags
    c.dual_track_mode             = cfg.dualTrackSymmetry;
    c.triple_psi_mode             = cfg.triplePsi;
    c.multi_level_koppa           = cfg.multiLevelKoppa;
    c.enable_asymmetric_cascade   = cfg.asymmetricCascade;
    c.enable_conditional_triple_psi = cfg.conditionalTriplePsi;
    c.enable_koppa_gated_engine   = cfg.koppaGatedEngine;
    c.enable_delta_cross_propagation = cfg.deltaCrossPropagation;
    c.enable_delta_koppa_offset   = cfg.deltaKoppaOffset;
    c.enable_ratio_threshold_psi  = cfg.ratioThresholdPsi;
    c.enable_stack_depth_modes    = cfg.stackDepthModes;
    c.enable_epsilon_phi_triangle = cfg.epsilonPhiTriangle;
    c.enable_modular_wrap         = cfg.modularWrap;
    c.enable_psi_strength_parameter = cfg.psiStrengthParameter;
    c.enable_ratio_snapshot_logging = cfg.ratioSnapshotLogging;
    c.enable_feedback_oscillator  = cfg.feedbackOscillator;
    c.enable_fibonacci_gate       = cfg.fibonacciGate;
    // enable_sign_flip is true if a non‑None sign flip mode was chosen
    c.enable_sign_flip            = (cfg.signFlipMode != TRTSConfig::SignFlipMode::None);
    // Additional parameters
    c.koppa_wrap_threshold = cfg.koppaWrapThreshold;
    // microTickIntervalMs is not part of Config; the engine uses a fixed microtick
    // interval for streaming.  We store it in ticks or pass as user data if needed.
    // Start streaming simulation
    logStatus(tr("Running…"));
    simulate_stream(&c, MainWindow::guiObserver, this);
    // Clean up rationals
    config_clear(&c);
    logStatus(tr("Finished"));
}

// Stop run (not yet implemented)
void MainWindow::handleStopRun()
{
    // There is no streaming cancellation API in the current engine.  In a full
    // implementation this would signal the engine to stop.  For now we just
    // update the status label.
    logStatus(tr("Stopped"));
}

// Handle pause: update the status but do not pause engine (stub)
void MainWindow::handlePause()
{
    logStatus(tr("Paused (not yet implemented)"));
}

// Reset run state and clear panels
void MainWindow::handleReset()
{
    m_execution->resetState();
    if (m_outputTable) m_outputTable->clear();
    if (m_rhythm)     ; // clear rhythm events if implemented
    if (m_stack)      ; // clear stack viewer if implemented
    logStatus(tr("Engine reset"));
}

// Clear all panels
void MainWindow::handleClearOutput()
{
    m_execution->resetState();
    if (m_rhythm)     ;
    if (m_stack)      ;
    if (m_analyzer)   ;
    if (m_fingerprint); 
    if (m_phaseMap)   ;
    if (m_theorist)   ;
    if (m_outputTable) m_outputTable->clear();
    logStatus(tr("Cleared in-memory state"));
}

// Load config flow
void MainWindow::handleLoadConfigRequested()
{
    loadConfigFromFile();
}

// Read config from file
void MainWindow::loadConfigFromFile()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        tr("Select TRTS config"),
        {},
        tr("TRTS config (*.trtscfg)")
    );
    if (path.isEmpty()) return;
    m_engineConfig->loadConfigurationFromFile(path);
    logStatus(tr("Loaded configuration from %1").arg(QFileInfo(path).fileName()));
}

// Parse lines from external process (unused by streaming mode)
void MainWindow::parseEngineLine(const QString &line)
{
    const QStringList cols = line.split(';');
    if (cols.size() < 8) {
        appendLogEntry({line});
        return;
    }
    // existing parsing + panel updates... (stub)
}

// Handle legacy external‑process output
void MainWindow::handleEngineOutput(const QString &line)
{
    parseEngineLine(line);
}

// Handle legacy external‑process errors
void MainWindow::handleEngineError(const QString &line)
{
    logStatus(tr("Engine error: %1").arg(line));
}

// Handle legacy external‑process finish
void MainWindow::handleRunFinished(int exitCode, QProcess::ExitStatus)
{
    logStatus(tr("Engine finished with exit code %1").arg(exitCode));
}

// Utility: append to output table
void MainWindow::appendLogEntry(const QStringList &columns)
{
    if (m_outputTable) m_outputTable->appendRow(columns);
}

// Utility: show status text
void MainWindow::logStatus(const QString &msg)
{
    if (m_statusLabel) m_statusLabel->setText(msg);
}

// Observer callback for streaming API
void MainWindow::guiObserver(void *userData,
                             size_t tick,
                             int microtick,
                             char phase,
                             const TRTS_State * /*state*/,
                             bool rho,
                             bool psi,
                             bool mu_zero,
                             bool forced)
{
    auto *self = static_cast<MainWindow*>(userData);
    emit self->engineUpdate(tick, microtick, phase, rho, psi, mu_zero, forced);
}

// Helper to parse a rational string into an mpq_t.  Accepts strings of the form
// "numerator/denominator" or decimal representations.  Returns true on
// success, false otherwise.  On failure, out is set to 0/1.
static bool parseRational(const QString &text, mpq_t out)
{
    // Initialize output to 0/1
    mpq_set_si(out, 0, 1);
    QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) {
        return false;
    }
    // Attempt to parse as fraction "a/b"
    const QStringList parts = trimmed.split('/');
    if (parts.size() == 2) {
        bool ok1 = false, ok2 = false;
        const long long num = parts.at(0).toLongLong(&ok1);
        const long long den = parts.at(1).toLongLong(&ok2);
        if (ok1 && ok2 && den != 0) {
            mpq_set_si(out, num, den);
            return true;
        }
    }
    // Attempt to parse as decimal using mpq_set_str with base 10
    QByteArray cstr = trimmed.toLatin1();
    if (mpq_set_str(out, cstr.constData(), 10) == 0) {
        return true;
    }
    // Fallback: cannot parse
    return false;
}
