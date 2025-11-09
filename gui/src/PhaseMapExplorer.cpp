#include "PhaseMapExplorer.hpp"

#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

PhaseMapExplorer::PhaseMapExplorer(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    m_infoLabel = new QLabel(tr("No phase map loaded"), this);
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({tr("Region"), tr("Classification"), tr("Support %"), tr("Hash")});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_loadButton = new QPushButton(tr("Load phase map"), this);

    layout->addWidget(m_infoLabel);
    layout->addWidget(m_table);
    layout->addWidget(m_loadButton);

    connect(m_loadButton, &QPushButton::clicked, this, &PhaseMapExplorer::loadPhaseMapRequested);
    connect(m_table, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
        Q_UNUSED(column);
        const QString hash = m_table->item(row, 3) ? m_table->item(row, 3)->text() : QString();
        if (!hash.isEmpty()) {
            emit rerunRequested(hash);
        }
    });
}

void PhaseMapExplorer::setPhaseMapInfo(const QString &info) {
    m_infoLabel->setText(info);
}

void PhaseMapExplorer::populateMap(const QVector<QStringList> &rows) {
    m_table->setRowCount(0);
    for (int i = 0; i < rows.size(); ++i) {
        m_table->insertRow(i);
        const auto &row = rows.at(i);
        for (int column = 0; column < row.size() && column < m_table->columnCount(); ++column) {
            m_table->setItem(i, column, new QTableWidgetItem(row.at(column)));
        }
    }
}

void PhaseMapExplorer::clear() {
    m_table->setRowCount(0);
    m_infoLabel->setText(tr("No phase map loaded"));
}

