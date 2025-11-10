// gui/include/StackViewerWidget.hpp
#pragma once

#include <QWidget>
#include <QTreeWidget>
#include <QList>
#include <QStringList>

class StackViewerWidget : public QWidget {
    Q_OBJECT

public:
    explicit StackViewerWidget(QWidget *parent = nullptr);
    ~StackViewerWidget() override = default;

    void setStackFrames(const QList<QStringList> &frames);
    void clear();

private slots:
    // Slot to receive live engine updates
    void onEngineUpdate(size_t tick,
                        int microtick,
                        char phase,
                        bool rho,
                        bool psi,
                        bool mu_zero,
                        bool forced);

private:
    QTreeWidget *m_tree{};
};

