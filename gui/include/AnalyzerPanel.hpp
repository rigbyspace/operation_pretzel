#pragma once

#include <QWidget>

class QPushButton;
class QTextEdit;
class QLabel;

class AnalyzerPanel : public QWidget {
    Q_OBJECT
public:
    explicit AnalyzerPanel(QWidget *parent = nullptr);

signals:
    void analyzeRequested();

public slots:
    void setResultSummary(const QString &summary);
    void setClassification(const QString &classification);
    void appendLog(const QString &line);
    void clearResults();

private:
    QPushButton *m_analyzeButton;
    QLabel *m_classificationLabel;
    QTextEdit *m_output;
};

