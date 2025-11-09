#include "FingerprintPanel.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

FingerprintPanel::FingerprintPanel(QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    m_imageLabel = new QLabel(this);
    m_imageLabel->setMinimumHeight(160);
    m_imageLabel->setAlignment(Qt::AlignCenter);

    m_summary = new QTextEdit(this);
    m_summary->setReadOnly(true);

    auto *buttonRow = new QHBoxLayout();
    m_exportSvg = new QPushButton(tr("Export SVG"), this);
    m_exportPng = new QPushButton(tr("Export PNG"), this);
    buttonRow->addWidget(m_exportSvg);
    buttonRow->addWidget(m_exportPng);
    buttonRow->addStretch();

    layout->addWidget(m_imageLabel);
    layout->addLayout(buttonRow);
    layout->addWidget(m_summary);

    connect(m_exportSvg, &QPushButton::clicked, this, &FingerprintPanel::exportSvgRequested);
    connect(m_exportPng, &QPushButton::clicked, this, &FingerprintPanel::exportPngRequested);
}

void FingerprintPanel::setFingerprintSummary(const QString &summary) {
    m_summary->setPlainText(summary);
}

void FingerprintPanel::setFingerprintImage(const QPixmap &pixmap) {
    m_imageLabel->setPixmap(pixmap.scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void FingerprintPanel::clear() {
    m_imageLabel->clear();
    m_summary->clear();
}

