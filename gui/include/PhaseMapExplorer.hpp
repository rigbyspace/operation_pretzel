#pragma once

#include <QWidget>
#include <QVector>
#include <QStringList>

class QTableWidget;
class QLabel;
class QPushButton;

class PhaseMapExplorer : public QWidget {
    Q_OBJECT
public:
    explicit PhaseMapExplorer(QWidget *parent = nullptr);

signals:
    void loadPhaseMapRequested();
    void rerunRequested(const QString &configurationHash);

public slots:
    void setPhaseMapInfo(const QString &info);
    void populateMap(const QVector<QStringList> &rows);
    void clear();

private:
    QTableWidget *m_table;
    QLabel *m_infoLabel;
    QPushButton *m_loadButton;
};

