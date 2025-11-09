#pragma once

#include <QMainWindow>
#include <QProcess>

#include "TRTSConfig.hpp"

class AnalyzerPanel;
class EngineConfigPanel;
class ExecutionPanel;
class FingerprintPanel;
class OutputTableWidget;
class PhaseMapExplorer;
class RhythmVisualizerWidget;
class StackViewerWidget;
class TheoristExplorerPanel;
class TRTSCoreProcess;

class QAction;
class QLabel;
class QTabWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void handleStartRun();
    void handleStopRun();
    void handleReset();
    void handleEngineOutput(const QString &line);
    void handleEngineError(const QString &line);
    void handleRunFinished(int exitCode, QProcess::ExitStatus status);
    void handleLoadConfigRequested();
    void loadConfigFromFile();

private:
    void buildUi();
    void connectSignals();
    void parseEngineLine(const QString &line);
    void appendLogEntry(const QStringList &columns);
    void logStatus(const QString &message);

    TRTSCoreProcess *m_process;
    EngineConfigPanel *m_engineConfig;
    ExecutionPanel *m_execution;
    RhythmVisualizerWidget *m_rhythm;
    StackViewerWidget *m_stack;
    AnalyzerPanel *m_analyzer;
    FingerprintPanel *m_fingerprint;
    PhaseMapExplorer *m_phaseMap;
    TheoristExplorerPanel *m_theorist;
    OutputTableWidget *m_outputTable;

    QLabel *m_statusLabel;
    QAction *m_actionStart;
    QAction *m_actionStop;
    QAction *m_actionReset;
    QAction *m_actionClear;
    QAction *m_actionLoadConfig;
    QTabWidget *m_tabs;

    QStringList m_headers;
};

