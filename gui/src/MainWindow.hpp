// MainWindow.hpp
// Defines the MainWindow class which orchestrates all GUI panels and handles
// running the TRTS simulation engine.  This version fixes the segmentation
// fault by avoiding reinterpret_cast from TRTSConfig to Config and instead
// performing an explicit conversion.  It also introduces a Pause action.

#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QTabWidget>
#include <QProcess>

#include "TRTSConfig.hpp"
#include "EngineConfigPanel.hpp"
#include "ExecutionPanel.hpp"

// Forward declarations for other panels; these are not implemented here but
// referenced as pointers so that MainWindow can compile.  In the full
// application these would be provided by their respective headers.
class RhythmVisualizerWidget;
class StackViewerWidget;
class AnalyzerPanel;
class FingerprintPanel;
class PhaseMapExplorer;
class TheoristExplorerPanel;
class OutputTableWidget;
class TRTSCoreProcess;

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
    // Handle pause requested by the UI; currently updates status only.
    void handlePause();
    void handleEngineOutput(const QString &line);
    void handleEngineError(const QString &line);
    void handleRunFinished(int exitCode, QProcess::ExitStatus status);
    void appendLogEntry(const QStringList &columns);
    void logStatus(const QString &message);

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
    QAction               *m_actionPause{};
    QAction               *m_actionReset{};
    QAction               *m_actionClear{};
    QAction               *m_actionLoadConfig{};
    QLabel                *m_statusLabel{};
    // Optional external-process fallback
    TRTSCoreProcess       *m_process{};
    // Engine configuration (deprecated)
    TRTSConfig             config;
    // Observer prototype for streaming updates
    static void guiObserver(void *userData,
                            size_t tick,
                            int microtick,
                            char phase,
                            const struct TRTS_State *state,
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
