// RhythmVisualizerWidget.cpp
#include "RhythmVisualizerWidget.hpp"
#include "MainWindow.hpp"

#include <QPainter>
#include <QPaintEvent>

RhythmVisualizerWidget::RhythmVisualizerWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(180);
    setAutoFillBackground(true);

    // Hook streaming updates:
    if (auto *mw = qobject_cast<MainWindow*>(parent)) {
        connect(mw, &MainWindow::engineUpdate,
                this, &RhythmVisualizerWidget::onEngineUpdate);
    }
}

void RhythmVisualizerWidget::appendEvent(const RhythmEvent &event)
{
    m_events.push_back(event);
    ensureVisibleTick(event.tick);
    update();
}

void RhythmVisualizerWidget::clearEvents()
{
    m_events.clear();
    update();
}

// Slot for streaming updates:
void RhythmVisualizerWidget::onEngineUpdate(size_t tick,
                                            int microtick,
                                            char /*phase*/,
                                            bool /*rho*/,
                                            bool /*psi*/,
                                            bool /*mu_zero*/,
                                            bool /*forced*/)
{
    RhythmEvent e;
    e.tick      = static_cast<int>(tick);
    e.microTick = microtick;
    e.psiType   = "stream";
    e.color     = QColor("#ff8800");
    appendEvent(e);
}

void RhythmVisualizerWidget::ensureVisibleTick(int tick)
{
    if (m_events.empty()) return;
    const int first = m_events.front().tick;
    while (!m_events.empty() && tick - first > m_visibleTicks) {
        m_events.pop_front();
    }
}

void RhythmVisualizerWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRect rect = event->rect().adjusted(10, 10, -10, -10);
    if (rect.width() <=0 || rect.height() <= 0) return;

    painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
    const int step = std::max(1, m_visibleTicks / 10);
    for (int t = 0; t <= m_visibleTicks; t += step) {
        qreal x = rect.left() + (qreal(t)/m_visibleTicks) * rect.width();
        painter.drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }

    if (m_events.empty()) {
        painter.drawText(rect, Qt::AlignCenter, tr("Awaiting ψ events"));
        return;
    }

    painter.setPen(Qt::NoPen);
    const int minTick = m_events.front().tick;
    const int maxTick = std::max(minTick + m_visibleTicks, m_events.back().tick + 1);
    const qreal span = std::max(1, maxTick - minTick);

    for (const auto &evt : m_events) {
        qreal pos   = (evt.tick - minTick) / span;
        qreal x     = rect.left() + (pos + evt.microTick/11.0 / m_visibleTicks)*rect.width();
        qreal y     = rect.bottom() - (evt.microTick/11.0)*rect.height();
        painter.setBrush(evt.color);
        painter.drawEllipse(QPointF(x,y), 4.0, 4.0);
    }
}

