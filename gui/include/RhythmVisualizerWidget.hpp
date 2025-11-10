// gui/include/RhythmVisualizerWidget.hpp
#pragma once

#include <QWidget>
#include <QColor>
#include <deque>

// Define a simple event struct here to avoid extra dependencies
struct RhythmEvent {
    int      tick;      // overall tick count
    int      microTick; // sub‐tick index
    QString  psiType;   // label or phase
    QColor   color;     // draw color
};

class RhythmVisualizerWidget : public QWidget {
    Q_OBJECT

public:
    explicit RhythmVisualizerWidget(QWidget *parent = nullptr);
    ~RhythmVisualizerWidget() override = default;

    void appendEvent(const RhythmEvent &event);
    void clearEvents();
    void setVisibleTicks(int ticks) { m_visibleTicks = ticks; }

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    // Receives live updates from MainWindow::engineUpdate
    void onEngineUpdate(size_t tick,
                        int microtick,
                        char /*phase*/,
                        bool /*rho*/,
                        bool /*psi*/,
                        bool /*mu_zero*/,
                        bool /*forced*/);

private:
    void ensureVisibleTick(int currentTick);

    std::deque<RhythmEvent> m_events;
    int                     m_visibleTicks = 100;
};

