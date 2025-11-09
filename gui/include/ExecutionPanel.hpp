#pragma once

#include <QWidget>

#include "TRTSConfig.hpp"

class QLabel;
class QPushButton;
class QTableWidget;

class ExecutionPanel : public QWidget {
    Q_OBJECT
public:
    explicit ExecutionPanel(QWidget *parent = nullptr);

    void updateState(const QString &tick, const QString &upsilon, const QString &beta, const QString &koppa,
                     bool psiFired, int stackDepth, const QString &rhoSource, const QString &psiMode);
    void appendLogRow(const QStringList &row);
    void resetState();

signals:
    void stepRequested();
    void runRequested();
    void stopRequested();
    void resetRequested();

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

    QTableWidget *m_logTable;
};

