#pragma once

#include "stdio.h"
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

    TRTSConfig configuration() const;
    void setConfiguration(const TRTSConfig &config);

signals:
    void configurationChanged(const TRTSConfig &config);
    void requestLoadConfig();

public slots:
    void loadConfigurationFromFile(const QString &path);

private slots:
    void emitConfigurationChanged();

private:
    void buildUi();
    void populateFromConfig(const TRTSConfig &config);
    void applyComboBoxSelection(QComboBox *combo, int index);

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

    QLineEdit *m_upsilonSeed;
    QLineEdit *m_betaSeed;
    QLineEdit *m_koppaSeed;

    QSpinBox *m_tickCount;
    QSpinBox *m_microTickMs;
    QSpinBox *m_koppaWrap;

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
    QCheckBox* m_fibonacciGate;

    QPushButton *m_loadConfigButton;
};

