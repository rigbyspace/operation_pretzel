#include "AnalyzerPanel.hpp"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

AnalyzerPanel::AnalyzerPanel(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    m_classificationLabel = new QLabel(tr("Classification: pending"), this);
    m_analyzeButton = new QPushButton(tr("Run metallic analyzer"), this);
    m_output = new QTextEdit(this);
    m_output->setReadOnly(true);

    layout->addWidget(m_classificationLabel);
    layout->addWidget(m_analyzeButton);
    layout->addWidget(m_output);
    layout->addStretch();

    connect(m_analyzeButton, &QPushButton::clicked, this, &AnalyzerPanel::analyzeRequested);
}

void AnalyzerPanel::setResultSummary(const QString &summary) {
    m_output->setPlainText(summary);
}

void AnalyzerPanel::setClassification(const QString &classification) {
    m_classificationLabel->setText(tr("Classification: %1").arg(classification));
}

void AnalyzerPanel::appendLog(const QString &line) {
    m_output->append(line);
}

void AnalyzerPanel::clearResults() {
    m_output->clear();
    m_classificationLabel->setText(tr("Classification: pending"));
}

