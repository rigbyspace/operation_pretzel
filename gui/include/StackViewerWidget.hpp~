#pragma once

#include <QWidget>
#include <QList>
#include <QStringList>

class QTreeWidget;

class StackViewerWidget : public QWidget {
    Q_OBJECT
public:
    explicit StackViewerWidget(QWidget *parent = nullptr);

    void setStackFrames(const QList<QStringList> &frames);
    void clear();

private:
    QTreeWidget *m_tree;
};

