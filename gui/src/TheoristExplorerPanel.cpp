#include "TheoristExplorerPanel.hpp"

#include <QHeaderView>
#include <QPushButton>
#include <QTextEdit>
#include <QTreeWidget>
#include <QVBoxLayout>

TheoristExplorerPanel::TheoristExplorerPanel(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    m_tree = new QTreeWidget(this);
    m_tree->setHeaderLabels({tr("Hypothesis"), tr("Support %"), tr("Contradictions"), tr("Next test")});
    m_tree->header()->setSectionResizeMode(QHeaderView::Stretch);

    auto *buttonRow = new QHBoxLayout();
    m_loadButton = new QPushButton(tr("Load Theorist"), this);
    m_exportMarkdown = new QPushButton(tr("Export theories.md"), this);
    m_exportSuggestions = new QPushButton(tr("Export suggestions.csv"), this);
    buttonRow->addWidget(m_loadButton);
    buttonRow->addWidget(m_exportMarkdown);
    buttonRow->addWidget(m_exportSuggestions);
    buttonRow->addStretch();

    m_summary = new QTextEdit(this);
    m_summary->setReadOnly(true);

    layout->addWidget(m_tree);
    layout->addLayout(buttonRow);
    layout->addWidget(m_summary);

    connect(m_loadButton, &QPushButton::clicked, this, &TheoristExplorerPanel::loadTheoryRequested);
    connect(m_exportMarkdown, &QPushButton::clicked, this, &TheoristExplorerPanel::exportMarkdownRequested);
    connect(m_exportSuggestions, &QPushButton::clicked, this, &TheoristExplorerPanel::exportSuggestionsRequested);
}

void TheoristExplorerPanel::populateHypotheses(const QList<QStringList> &rows) {
    m_tree->clear();
    for (const auto &row : rows) {
        auto *item = new QTreeWidgetItem(m_tree);
        for (int column = 0; column < row.size() && column < m_tree->columnCount(); ++column) {
            item->setText(column, row.at(column));
        }
    }
    m_tree->expandAll();
}

void TheoristExplorerPanel::setSummary(const QString &text) {
    m_summary->setPlainText(text);
}

void TheoristExplorerPanel::clear() {
    m_tree->clear();
    m_summary->clear();
}

