// ExecutionPanel.hpp
// Provides a panel for controlling execution of the TRTS engine and displaying
// real‑time state.  This version extends the original by adding a "Pause"
// button and signal.  The pause signal does not currently suspend the engine
// but allows MainWindow to react to the user's intent.

#pragma once

#include <QWidget>
#include <QStringList>

class QLabel;
class QPushButton;
class QTableWidget;

class ExecutionPanel : public QWidget {
    Q_OBJECT
public:
    explicit ExecutionPanel(QWidget *parent = nullptr);

    // Update the displayed state of the engine.  These helpers are called
    // either by MainWindow directly or via the guiObserver callback.
    void updateState(const QString &tick,
                     const QString &upsilon,
                     const QString &beta,
                     const QString &koppa,
                     bool psi,
                     int stackSize,
                     const QString &rho,
                     const QString &psiMode);
    // Append a row to the log table.
    void appendLogRow(const QStringList &row);
    // Reset the displayed state and clear the log.
    void resetState();

signals:
    void runRequested();
    void stopRequested();
    void resetRequested();
    void stepRequested();
    // New signal emitted when the user requests a pause.  There is no
    // underlying pause implementation yet; this simply notifies MainWindow.
    void pauseRequested();

private slots:
    // Slot to receive streaming updates from the engine via MainWindow.
    void updateFromEngine(size_t tick,
                          int microtick,
                          char phase,
                          bool rho,
                          bool psi,
                          bool mu_zero,
                          bool forced);

private:
    void buildUi();

    QLabel *m_tickLabel;
    QLabel *m_upsilonLabel;
    QLabel *m_betaLabel;
    QLabel *m_koppaLabel;
    QLabel *m_rhoLabel;
    QLabel *m_psiLabel;
    QLabel *m_stackLabel;

    QPushButton *m_stepButton;
    QPushButton *m_runButton;
    QPushButton *m_stopButton;
    QPushButton *m_resetButton;
    // New pause button
    QPushButton *m_pauseButton;

    QTableWidget *m_logTable;
};
