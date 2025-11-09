#include "RhythmVisualizerWidget.hpp"

#include <QPainter>
#include <QPaintEvent>

RhythmVisualizerWidget::RhythmVisualizerWidget(QWidget *parent)
    : QWidget(parent) {
    setMinimumHeight(180);
    setAutoFillBackground(true);
}

void RhythmVisualizerWidget::appendEvent(const RhythmEvent &event) {
    m_events.push_back(event);
    ensureVisibleTick(event.tick);
    update();
}

void RhythmVisualizerWidget::clearEvents() {
    m_events.clear();
    update();
}

void RhythmVisualizerWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRect rect = event->rect().adjusted(10, 10, -10, -10);
    if (rect.width() <= 0 || rect.height() <= 0) {
        return;
    }

    painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
    const int tickStep = std::max(1, m_visibleTicks / 10);
    for (int tick = 0; tick <= m_visibleTicks; tick += tickStep) {
        const qreal x = rect.left() + (static_cast<qreal>(tick) / m_visibleTicks) * rect.width();
        painter.drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }

    if (m_events.empty()) {
        painter.drawText(rect, Qt::AlignCenter, tr("Awaiting ψ events"));
        return;
    }

    painter.setPen(Qt::NoPen);

    const int minTick = m_events.front().tick;
    const int maxTick = std::max(minTick + m_visibleTicks, m_events.back().tick + 1);
    const qreal tickSpan = std::max(1, maxTick - minTick);

    for (const auto &evt : m_events) {
        const qreal tickPosition = (evt.tick - minTick) / tickSpan;
        const qreal microOffset = evt.microTick / 11.0;
        const qreal x = rect.left() + (tickPosition + microOffset / m_visibleTicks) * rect.width();
        const qreal y = rect.bottom() - (evt.microTick / 11.0) * rect.height();
        painter.setBrush(evt.color);  
        painter.drawEllipse(QPointF(x, y), 4.0, 4.0);
    }
}

void RhythmVisualizerWidget::ensureVisibleTick(int tick) {
    if (m_events.empty()) {
        return;
    }

    const int firstTick = m_events.front().tick;
    while (!m_events.empty() && tick - firstTick > m_visibleTicks) {
        m_events.pop_front();
    }
}

