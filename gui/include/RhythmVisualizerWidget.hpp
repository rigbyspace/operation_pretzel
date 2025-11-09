#pragma once

#include <QWidget>
#include <QColor>
#include <QString>
#include <deque>

struct RhythmEvent {
    int tick = 0;
    int microTick = 0;
    QString psiType;
    QColor color;
};

class RhythmVisualizerWidget : public QWidget {
    Q_OBJECT
public:
    explicit RhythmVisualizerWidget(QWidget *parent = nullptr);

    void appendEvent(const RhythmEvent &event);
    void clearEvents();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void ensureVisibleTick(int tick);

    std::deque<RhythmEvent> m_events;
    int m_visibleTicks = 50;
};

