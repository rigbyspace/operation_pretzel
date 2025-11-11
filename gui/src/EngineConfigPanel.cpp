// EngineConfigPanel.cpp
// Provides the implementation for EngineConfigPanel, allowing users to
// configure all simulation parameters via a GUI.  This version extends the
// original upstream implementation with a checkbox for the Fibonacci gate.

#include "../include/EngineConfigPanel.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

EngineConfigPanel::EngineConfigPanel(QWidget *parent)
    : QWidget(parent) {
    buildUi();
    // Initialize controls with default configuration
    setConfiguration(TRTSConfig{});
}

TRTSConfig EngineConfigPanel::configuration() const {
    TRTSConfig config;
    // Enum selections
    config.psiMode = static_cast<TRTSConfig::PsiMode>(m_psiMode->currentIndex());
    config.engineMode = static_cast<TRTSConfig::EngineMode>(m_engineMode->currentIndex());
    config.koppaMode = static_cast<TRTSConfig::KoppaMode>(m_koppaMode->currentIndex());
    config.koppaTrigger = static_cast<TRTSConfig::KoppaTrigger>(m_koppaTrigger->currentIndex());
    config.mt10Behavior = static_cast<TRTSConfig::Mt10Behavior>(m_mt10Behavior->currentIndex());
    config.ratioTriggerMode = static_cast<TRTSConfig::RatioTriggerMode>(m_ratioTrigger->currentIndex());
    config.primeTarget = static_cast<TRTSConfig::PrimeTarget>(m_primeTarget->currentIndex());
    config.signFlipMode = static_cast<TRTSConfig::SignFlipMode>(m_signFlip->currentIndex());
    config.upsilonTrack = static_cast<TRTSConfig::EngineTrackMode>(m_upsilonTrack->currentIndex());
    config.betaTrack = static_cast<TRTSConfig::EngineTrackMode>(m_betaTrack->currentIndex());
    // Toggles
    config.dualTrackSymmetry = m_dualTrackSymmetry->isChecked();
    config.triplePsi = m_triplePsi->isChecked();
    config.multiLevelKoppa = m_multiLevelKoppa->isChecked();
    config.asymmetricCascade = m_asymmetricCascade->isChecked();
    config.conditionalTriplePsi = m_conditionalTriplePsi->isChecked();
    config.koppaGatedEngine = m_koppaGatedEngine->isChecked();
    config.deltaCrossPropagation = m_deltaCrossPropagation->isChecked();
    config.deltaKoppaOffset = m_deltaKoppaOffset->isChecked();
    config.ratioThresholdPsi = m_ratioThresholdPsi->isChecked();
    config.stackDepthModes = m_stackDepthModes->isChecked();
    config.epsilonPhiTriangle = m_epsilonPhiTriangle->isChecked();
    config.modularWrap = m_modularWrap->isChecked();
    config.psiStrengthParameter = m_psiStrengthParameter->isChecked();
    config.ratioSnapshotLogging = m_ratioSnapshotLogging->isChecked();
    config.feedbackOscillator = m_feedbackOscillator->isChecked();
    config.fibonacciGate = m_fibonacciGate->isChecked();
    // Seeds and timing
    config.upsilonSeed = m_upsilonSeed->text();
    config.betaSeed = m_betaSeed->text();
    config.koppaSeed = m_koppaSeed->text();
    config.tickCount = static_cast<quint32>(m_tickCount->value());
    config.microTickIntervalMs = static_cast<quint32>(m_microTickMs->value());
    config.koppaWrapThreshold = static_cast<quint32>(m_koppaWrap->value());
    return config;
}

void EngineConfigPanel::setConfiguration(const TRTSConfig &config) {
    populateFromConfig(config);
}

void EngineConfigPanel::loadConfigurationFromFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) {
        return;
    }
    const TRTSConfig cfg = TRTSConfig::fromJson(doc.object());
    populateFromConfig(cfg);
    emit configurationChanged(configuration());
}

void EngineConfigPanel::emitConfigurationChanged() {
    emit configurationChanged(configuration());
}

void EngineConfigPanel::buildUi() {
    auto *layout = new QVBoxLayout(this);

    // Engine track selection
    auto *tracksBox = new QGroupBox(tr("Engine tracks"));
    auto *tracksLayout = new QGridLayout(tracksBox);
    m_upsilonTrack = new QComboBox(tracksBox);
    m_upsilonTrack->addItems(engineTrackLabels());
    m_betaTrack = new QComboBox(tracksBox);
    m_betaTrack->addItems(engineTrackLabels());
    tracksLayout->addWidget(new QLabel(tr("υ track")), 0, 0);
    tracksLayout->addWidget(m_upsilonTrack, 0, 1);
    tracksLayout->addWidget(new QLabel(tr("β track")), 1, 0);
    tracksLayout->addWidget(m_betaTrack, 1, 1);
    tracksBox->setLayout(tracksLayout);

    // Core configuration comboboxes
    auto *configBox = new QGroupBox(tr("Core configuration"));
    auto *configForm = new QFormLayout(configBox);
    m_engineMode = new QComboBox(configBox);
    m_engineMode->addItems(engineModeLabels());
    configForm->addRow(tr("Engine mode"), m_engineMode);
    m_psiMode = new QComboBox(configBox);
    m_psiMode->addItems(psiModeLabels());
    configForm->addRow(tr("ψ mode"), m_psiMode);
    m_koppaMode = new QComboBox(configBox);
    m_koppaMode->addItems(koppaModeLabels());
    configForm->addRow(tr("κ mode"), m_koppaMode);
    m_koppaTrigger = new QComboBox(configBox);
    m_koppaTrigger->addItems(koppaTriggerLabels());
    configForm->addRow(tr("κ trigger"), m_koppaTrigger);
    m_mt10Behavior = new QComboBox(configBox);
    m_mt10Behavior->addItems(mt10BehaviorLabels());
    configForm->addRow(tr("MT10"), m_mt10Behavior);
    m_ratioTrigger = new QComboBox(configBox);
    m_ratioTrigger->addItems(ratioTriggerLabels());
    configForm->addRow(tr("Ratio trigger"), m_ratioTrigger);
    m_primeTarget = new QComboBox(configBox);
    m_primeTarget->addItems(primeTargetLabels());
    configForm->addRow(tr("Prime target"), m_primeTarget);
    m_signFlip = new QComboBox(configBox);
    m_signFlip->addItems(signFlipLabels());
    configForm->addRow(tr("Sign flip"), m_signFlip);
    configBox->setLayout(configForm);

    // Seed inputs
    auto *seedBox = new QGroupBox(tr("Seeds"));
    auto *seedForm = new QFormLayout(seedBox);
    m_upsilonSeed = new QLineEdit(seedBox);
    m_betaSeed = new QLineEdit(seedBox);
    m_koppaSeed = new QLineEdit(seedBox);
    seedForm->addRow(tr("υ"), m_upsilonSeed);
    seedForm->addRow(tr("β"), m_betaSeed);
    seedForm->addRow(tr("κ"), m_koppaSeed);
    seedBox->setLayout(seedForm);

    // Timing controls
    auto *timingBox = new QGroupBox(tr("Timing"));
    auto *timingForm = new QFormLayout(timingBox);
    m_tickCount = new QSpinBox(timingBox);
    m_tickCount->setRange(1, 10000);
    m_tickCount->setValue(5);
    m_microTickMs = new QSpinBox(timingBox);
    m_microTickMs->setRange(10, 10000);
    m_microTickMs->setValue(150);
    m_koppaWrap = new QSpinBox(timingBox);
    m_koppaWrap->setRange(0, 1000000);
    timingForm->addRow(tr("Ticks"), m_tickCount);
    timingForm->addRow(tr("μtick ms"), m_microTickMs);
    timingForm->addRow(tr("κ wrap"), m_koppaWrap);
    timingBox->setLayout(timingForm);

    // Advanced modes toggles
    auto *modesBox = new QGroupBox(tr("Advanced modes"));
    auto *modesLayout = new QGridLayout(modesBox);
    // Define pairs of label and pointer to toggle pointer.  When adding new
    // toggles, append them here and declare the corresponding member in
    // EngineConfigPanel.hpp.  The order controls the placement in the grid.
    const QList<QPair<QString, QCheckBox **>> toggles = {
        {tr("Dual-track symmetry"), &m_dualTrackSymmetry},
        {tr("Triple ψ"),            &m_triplePsi},
        {tr("Multi-level κ"),       &m_multiLevelKoppa},
        {tr("Asymmetric cascade"),  &m_asymmetricCascade},
        {tr("Conditional triple ψ"),&m_conditionalTriplePsi},
        {tr("κ gated engine"),      &m_koppaGatedEngine},
        {tr("Δ cross propagation"), &m_deltaCrossPropagation},
        {tr("Δ κ offset"),          &m_deltaKoppaOffset},
        {tr("Ratio threshold ψ"),   &m_ratioThresholdPsi},
        {tr("Stack depth modes"),   &m_stackDepthModes},
        {tr("ε-φ triangle"),        &m_epsilonPhiTriangle},
        {tr("Modular wrap"),        &m_modularWrap},
        {tr("ψ strength parameter"),&m_psiStrengthParameter},
        {tr("Ratio snapshot logging"), &m_ratioSnapshotLogging},
        {tr("Feedback oscillator"), &m_feedbackOscillator},
        {tr("Fibonacci gate"),      &m_fibonacciGate}
    };
    int row = 0;
    int column = 0;
    for (const auto &pair : toggles) {
        auto *box = new QCheckBox(pair.first, modesBox);
        *(pair.second) = box;
        modesLayout->addWidget(box, row, column);
        column = (column + 1) % 2;
        if (column == 0) {
            ++row;
        }
        connect(box, &QCheckBox::toggled, this, &EngineConfigPanel::emitConfigurationChanged);
    }
    modesBox->setLayout(modesLayout);

    // Load configuration button
    m_loadConfigButton = new QPushButton(tr("Load config"), this);
    connect(m_loadConfigButton, &QPushButton::clicked, this, [this]() {
        emit requestLoadConfig();
    });

    // Assemble the panel
    layout->addWidget(tracksBox);
    layout->addWidget(configBox);
    layout->addWidget(seedBox);
    layout->addWidget(timingBox);
    layout->addWidget(modesBox);
    layout->addWidget(m_loadConfigButton);
    layout->addStretch();

    // Connect combo boxes
    const QList<QComboBox *> combos = {m_psiMode,    m_engineMode,   m_koppaMode,     m_koppaTrigger,
                                       m_mt10Behavior, m_ratioTrigger, m_primeTarget,   m_signFlip,
                                       m_upsilonTrack, m_betaTrack};
    for (QComboBox *combo : combos) {
        connect(combo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, &EngineConfigPanel::emitConfigurationChanged);
    }
    // Connect line edits
    const QList<QLineEdit *> edits = {m_upsilonSeed, m_betaSeed, m_koppaSeed};
    for (QLineEdit *edit : edits) {
        connect(edit, &QLineEdit::editingFinished, this, &EngineConfigPanel::emitConfigurationChanged);
    }
    // Connect spin boxes
    const QList<QSpinBox *> spins = {m_tickCount, m_microTickMs, m_koppaWrap};
    for (QSpinBox *spin : spins) {
        connect(spin, QOverload<int>::of(&QSpinBox::valueChanged), this, &EngineConfigPanel::emitConfigurationChanged);
    }
}

void EngineConfigPanel::populateFromConfig(const TRTSConfig &config) {
    applyComboBoxSelection(m_psiMode, static_cast<int>(config.psiMode));
    applyComboBoxSelection(m_engineMode, static_cast<int>(config.engineMode));
    applyComboBoxSelection(m_koppaMode, static_cast<int>(config.koppaMode));
    applyComboBoxSelection(m_koppaTrigger, static_cast<int>(config.koppaTrigger));
    applyComboBoxSelection(m_mt10Behavior, static_cast<int>(config.mt10Behavior));
    applyComboBoxSelection(m_ratioTrigger, static_cast<int>(config.ratioTriggerMode));
    applyComboBoxSelection(m_primeTarget, static_cast<int>(config.primeTarget));
    applyComboBoxSelection(m_signFlip, static_cast<int>(config.signFlipMode));
    applyComboBoxSelection(m_upsilonTrack, static_cast<int>(config.upsilonTrack));
    applyComboBoxSelection(m_betaTrack, static_cast<int>(config.betaTrack));
    m_dualTrackSymmetry->setChecked(config.dualTrackSymmetry);
    m_triplePsi->setChecked(config.triplePsi);
    m_multiLevelKoppa->setChecked(config.multiLevelKoppa);
    m_asymmetricCascade->setChecked(config.asymmetricCascade);
    m_conditionalTriplePsi->setChecked(config.conditionalTriplePsi);
    m_koppaGatedEngine->setChecked(config.koppaGatedEngine);
    m_deltaCrossPropagation->setChecked(config.deltaCrossPropagation);
    m_deltaKoppaOffset->setChecked(config.deltaKoppaOffset);
    m_ratioThresholdPsi->setChecked(config.ratioThresholdPsi);
    m_stackDepthModes->setChecked(config.stackDepthModes);
    m_epsilonPhiTriangle->setChecked(config.epsilonPhiTriangle);
    m_modularWrap->setChecked(config.modularWrap);
    m_psiStrengthParameter->setChecked(config.psiStrengthParameter);
    m_ratioSnapshotLogging->setChecked(config.ratioSnapshotLogging);
    m_feedbackOscillator->setChecked(config.feedbackOscillator);
    m_fibonacciGate->setChecked(config.fibonacciGate);
    m_upsilonSeed->setText(config.upsilonSeed);
    m_betaSeed->setText(config.betaSeed);
    m_koppaSeed->setText(config.koppaSeed);
    m_tickCount->setValue(config.tickCount);
    m_microTickMs->setValue(config.microTickIntervalMs);
    m_koppaWrap->setValue(config.koppaWrapThreshold);
}

void EngineConfigPanel::applyComboBoxSelection(QComboBox *combo, int index) {
    if (!combo) {
        return;
    }
    if (index >= 0 && index < combo->count()) {
        combo->setCurrentIndex(index);
    }
}
