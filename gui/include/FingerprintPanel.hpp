#pragma once

#include <QWidget>
#include <QPixmap>

class QLabel;
class QPushButton;
class QTextEdit;

class FingerprintPanel : public QWidget {
    Q_OBJECT
public:
    explicit FingerprintPanel(QWidget *parent = nullptr);

signals:
    void exportSvgRequested();
    void exportPngRequested();

public slots:
    void setFingerprintSummary(const QString &summary);
    void setFingerprintImage(const QPixmap &pixmap);
    void clear();

private:
    QLabel *m_imageLabel;
    QTextEdit *m_summary;
    QPushButton *m_exportSvg;
    QPushButton *m_exportPng;
};

