#include "OutputTableWidget.hpp"

#include <QHeaderView>
#include <QList>
#include <QPushButton>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

OutputTableWidget::OutputTableWidget(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    m_table = new QTableView(this);
    m_model = new QStandardItemModel(this);
    m_model->setHorizontalHeaderLabels({tr("Tick"), tr("MT"), tr("υ"), tr("β"), tr("κ"), tr("ψ"), tr("ρ"),
                                        tr("μ"), tr("Events")});
    m_table->setModel(m_model);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_exportButton = new QPushButton(tr("Export CSV"), this);

    layout->addWidget(m_table);
    layout->addWidget(m_exportButton);

    connect(m_exportButton, &QPushButton::clicked, this, &OutputTableWidget::exportCsvRequested);
}

void OutputTableWidget::appendRow(const QStringList &row) {
    QList<QStandardItem *> items;
    items.reserve(row.size());
    for (const auto &value : row) {
        items.append(new QStandardItem(value));
    }
    m_model->appendRow(items);
}

void OutputTableWidget::clear() {
    m_model->removeRows(0, m_model->rowCount());
}

