#include "StackViewerWidget.hpp"

#include <QHeaderView>
#include <QTreeWidget>
#include <QVBoxLayout>

StackViewerWidget::StackViewerWidget(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    m_tree = new QTreeWidget(this);
    m_tree->setHeaderLabels({tr("Frame"), tr("υ"), tr("β"), tr("κ"), tr("ψ")});
    m_tree->header()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(m_tree);
}

void StackViewerWidget::setStackFrames(const QList<QStringList> &frames) {
    m_tree->clear();
    int index = 0;
    for (const auto &frame : frames) {
        auto *item = new QTreeWidgetItem(m_tree);
        item->setText(0, tr("Frame %1").arg(index++));
        for (int i = 0; i < frame.size() && i + 1 < m_tree->columnCount(); ++i) {
            item->setText(i + 1, frame.at(i));
        }
    }
    m_tree->expandAll();
}

void StackViewerWidget::clear() {
    m_tree->clear();
}

