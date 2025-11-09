#pragma once

#include <QWidget>
#include <QStringList>

class QTableView;
class QStandardItemModel;
class QPushButton;

class OutputTableWidget : public QWidget {
    Q_OBJECT
public:
    explicit OutputTableWidget(QWidget *parent = nullptr);

signals:
    void exportCsvRequested();

public slots:
    void appendRow(const QStringList &row);
    void clear();

private:
    QTableView *m_table;
    QStandardItemModel *m_model;
    QPushButton *m_exportButton;
};

