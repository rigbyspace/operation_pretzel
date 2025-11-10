// StackViewerWidget.cpp
#include "StackViewerWidget.hpp"
#include "MainWindow.hpp"

#include <QHeaderView>
#include <QTreeWidget>
#include <QVBoxLayout>

StackViewerWidget::StackViewerWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    m_tree = new QTreeWidget(this);
    m_tree->setHeaderLabels({tr("Frame"), tr("υ"), tr("β"), tr("κ"), tr("ψ")});
    m_tree->header()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(m_tree);

    // Hook streaming updates:
    if (auto *mw = qobject_cast<MainWindow*>(parent)) {
        connect(mw, &MainWindow::engineUpdate,
                this, &StackViewerWidget::onEngineUpdate);
    }
}

void StackViewerWidget::setStackFrames(const QList<QStringList> &frames)
{
    m_tree->clear();
    int idx = 1;
    for (auto &frame : frames) {
        auto *item = new QTreeWidgetItem(m_tree);
        item->setText(0, tr("Frame %1").arg(idx++));
        for (int c = 0; c < frame.size() && c+1 < m_tree->columnCount(); ++c) {
            item->setText(c+1, frame.at(c));
        }
    }
    m_tree->expandAll();
}

void StackViewerWidget::clear()
{
    m_tree->clear();
}

// Slot for streaming updates:
void StackViewerWidget::onEngineUpdate(size_t /*tick*/,
                                       int /*microtick*/,
                                       char /*phase*/,
                                       bool /*rho*/,
                                       bool /*psi*/,
                                       bool /*mu_zero*/,
                                       bool /*forced*/)
{
    // rebuild stack frames from state if available; for now, send empty
    setStackFrames({});
}

