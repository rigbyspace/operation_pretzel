// ExecutionPanel.hpp
#pragma once
#include "stdio.h"
#include <QWidget>
#include <QStringList>

class QLabel;
class QPushButton;
class QTableWidget;

class ExecutionPanel : public QWidget {
    Q_OBJECT
public:
    explicit ExecutionPanel(QWidget *parent = nullptr);

    void updateState(const QString &tick,
                     const QString &upsilon,
                     const QString &beta,
                     const QString &koppa,
                     bool psi,
                     int stackSize,
                     const QString &rho,
                     const QString &psiMode);

    void appendLogRow(const QStringList &row);
    void resetState();

signals:
    void runRequested();
    void stopRequested();
    void resetRequested();
    void stepRequested();
    void pauseRequested();

private slots:
    // Added to receive streamed updates:
    void updateFromEngine(size_t tick,
                          int microtick,
                          char phase,
                          bool rho,
                          bool psi,
                          bool mu_zero,
                          bool forced);

private:
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
    QPushButton *m_pauseButton;

    QTableWidget *m_logTable;

    void buildUi();
};

