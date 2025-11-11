// EngineConfigPanel.hpp
// This class provides a QWidget containing form controls for building a
// TRTSConfig from user input.  It has been extended with a checkbox for
// enabling the Fibonacci gate option.  When the user changes any control,
// the configurationChanged() signal is emitted with the new TRTSConfig.

#pragma once

#include "TRTSConfig.hpp"

#include <QWidget>

class QComboBox;
class QCheckBox;
class QLineEdit;
class QPushButton;
class QSpinBox;

class EngineConfigPanel : public QWidget {
    Q_OBJECT
public:
    explicit EngineConfigPanel(QWidget *parent = nullptr);

    // Return the current configuration assembled from the UI controls.
    TRTSConfig configuration() const;
    // Populate all controls based on an existing configuration.
    void setConfiguration(const TRTSConfig &config);

signals:
    // Emitted whenever any part of the configuration changes.  The updated
    // configuration is passed along for convenience.
    void configurationChanged(const TRTSConfig &config);
    // Emitted when the user clicks the "Load config" button.
    void requestLoadConfig();

public slots:
    // Load configuration from the specified file path.  This slot will be
    // invoked by MainWindow when the user selects a .trtscfg file.
    void loadConfigurationFromFile(const QString &path);

private slots:
    // Internal helper to emit configurationChanged() when a control changes.
    void emitConfigurationChanged();

private:
    void buildUi();
    void populateFromConfig(const TRTSConfig &config);
    void applyComboBoxSelection(QComboBox *combo, int index);

    // Combo boxes for enums
    QComboBox *m_psiMode;
    QComboBox *m_engineMode;
    QComboBox *m_koppaMode;
    QComboBox *m_koppaTrigger;
    QComboBox *m_mt10Behavior;
    QComboBox *m_ratioTrigger;
    QComboBox *m_primeTarget;
    QComboBox *m_signFlip;
    QComboBox *m_upsilonTrack;
    QComboBox *m_betaTrack;

    // Line edits for seeds
    QLineEdit *m_upsilonSeed;
    QLineEdit *m_betaSeed;
    QLineEdit *m_koppaSeed;

    // Spin boxes for timing
    QSpinBox *m_tickCount;
    QSpinBox *m_microTickMs;
    QSpinBox *m_koppaWrap;

    // Toggle checkboxes for feature flags
    QCheckBox *m_dualTrackSymmetry;
    QCheckBox *m_triplePsi;
    QCheckBox *m_multiLevelKoppa;
    QCheckBox *m_asymmetricCascade;
    QCheckBox *m_conditionalTriplePsi;
    QCheckBox *m_koppaGatedEngine;
    QCheckBox *m_deltaCrossPropagation;
    QCheckBox *m_deltaKoppaOffset;
    QCheckBox *m_ratioThresholdPsi;
    QCheckBox *m_stackDepthModes;
    QCheckBox *m_epsilonPhiTriangle;
    QCheckBox *m_modularWrap;
    QCheckBox *m_psiStrengthParameter;
    QCheckBox *m_ratioSnapshotLogging;
    QCheckBox *m_feedbackOscillator;
    // New toggle: Fibonacci gate
    QCheckBox *m_fibonacciGate;

    // Button to load configuration
    QPushButton *m_loadConfigButton;
};
