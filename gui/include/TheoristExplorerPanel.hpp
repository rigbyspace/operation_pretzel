#pragma once

#include <QWidget>
#include <QList>
#include <QStringList>

class QTreeWidget;
class QTextEdit;
class QPushButton;

class TheoristExplorerPanel : public QWidget {
    Q_OBJECT
public:
    explicit TheoristExplorerPanel(QWidget *parent = nullptr);

signals:
    void loadTheoryRequested();
    void exportMarkdownRequested();
    void exportSuggestionsRequested();

public slots:
    void populateHypotheses(const QList<QStringList> &rows);
    void setSummary(const QString &text);
    void clear();

private:
    QTreeWidget *m_tree;
    QTextEdit *m_summary;
    QPushButton *m_loadButton;
    QPushButton *m_exportMarkdown;
    QPushButton *m_exportSuggestions;
};

