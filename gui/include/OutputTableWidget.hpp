// gui/include/OutputTableWidget.hpp
#pragma once

#include <QWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QPushButton>
#include <QStringList>

class OutputTableWidget : public QWidget {
    Q_OBJECT

public:
    explicit OutputTableWidget(QWidget *parent = nullptr);
    ~OutputTableWidget() override = default;

    void appendRow(const QStringList &row);
    void clear();

signals:
    void exportCsvRequested();

private slots:
    // Receives live updates from MainWindow::engineUpdate
    void onEngineUpdate(size_t tick,
                        int microtick,
                        char phase,
                        bool rho,
                        bool psi,
                        bool mu_zero,
                        bool forced);

private:
    QTableView        *m_table{};
    QStandardItemModel*m_model{};
    QPushButton       *m_exportButton{};
};

