// gui/include/MainWindow.hpp
#pragma once

#include "stdio.h"
#include <QMainWindow>
#include <QLabel>
#include <QTabWidget>
#include <QProcess>
#include "RhythmVisualizerWidget.hpp"   // <— now included before member use
// Core engine headers
#include "../../state.h"      // TRTS_State
#include "../../simulate.h"   // simulate_stream, Config
#include "../../config.h"
// GUI panels and widgets:

#include "TRTSConfig.hpp"
#include "EngineConfigPanel.hpp"
#include "ExecutionPanel.hpp"

#include "StackViewerWidget.hpp"
#include "AnalyzerPanel.hpp"
#include "FingerprintPanel.hpp"
#include "PhaseMapExplorer.hpp"
#include "TheoristExplorerPanel.hpp"
#include "OutputTableWidget.hpp"
#include "TRTSCoreProcess.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

private slots:
    void handleStartRun();
    void handleStopRun();
    void handleReset();
    void handleClearOutput();
    void handleLoadConfigRequested();
    void handleEngineOutput(const QString &line);
    void handleEngineError(const QString &line);
    void handleRunFinished(int exitCode, QProcess::ExitStatus status);
    void appendLogEntry(const QStringList &columns);
    void logStatus(const QString &message);
    void handlePause();

private:
    void buildUi();
    void connectSignals();
    void loadConfigFromFile();
    void parseEngineLine(const QString &line);

    // Tab widget container
    QTabWidget            *m_tabs{};

    // Panels
    EngineConfigPanel     *m_engineConfig{};
    ExecutionPanel        *m_execution{};
    RhythmVisualizerWidget *m_rhythm{};
    StackViewerWidget     *m_stack{};
    AnalyzerPanel         *m_analyzer{};
    FingerprintPanel      *m_fingerprint{};
    PhaseMapExplorer      *m_phaseMap{};
    TheoristExplorerPanel *m_theorist{};
    OutputTableWidget     *m_outputTable{};

    // Actions & status
    QAction               *m_actionStart{};
    QAction               *m_actionStop{};
    QAction               *m_actionReset{};
    QAction               *m_actionClear{};
    QAction               *m_actionLoadConfig{};
    QAction               * m_actionPause;
    QLabel                *m_statusLabel{};

    // Optional external-process fallback
    TRTSCoreProcess       *m_process{};

    // Engine configuration
    TRTSConfig             config;

    // ─────────────────────────────────────────────────────────────────────────
    // Observer prototype for streaming updates
    static void guiObserver(void *userData,
                            size_t tick,
                            int microtick,
                            char phase,
                            const TRTS_State *state,
                            bool rho,
                            bool psi,
                            bool mu_zero,
                            bool forced);

signals:
    // Emitted once per microtick by guiObserver
    void engineUpdate(size_t tick,
                      int microtick,
                      char phase,
                      bool rho,
                      bool psi,
                      bool mu_zero,
                      bool forced);
};

