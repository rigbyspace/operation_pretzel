#pragma once

#include <QString>
#include <QStringList>
#include <QJsonObject>

struct TRTSConfig {
    enum class PsiMode {
        MStep,
        RhoOnly,
        MStepRho,
        InhibitRho
    };

    enum class KoppaMode {
        Dump,
        Pop,
        Accumulate
    };

    enum class EngineMode {
        Add,
        Multi,
        Slide,
        DeltaAdd
    };

    enum class EngineTrackMode {
        Add,
        Multi,
        Slide
    };

    enum class KoppaTrigger {
        OnPsi,
        OnMuAfterPsi,
        OnAllMu
    };

    enum class Mt10Behavior {
        ForcedEmissionOnly,
        ForcedPsi
    };

    enum class RatioTriggerMode {
        None,
        Golden,
        Sqrt2,
        Plastic
    };

    enum class PrimeTarget {
        Memory,
        NewUpsilon
    };

    enum class SignFlipMode {
        None,
        Always,
        Alternate
    };

    PsiMode psiMode = PsiMode::InhibitRho;
    KoppaMode koppaMode = KoppaMode::Pop;
    EngineMode engineMode = EngineMode::Add;
    EngineTrackMode upsilonTrack = EngineTrackMode::Add;
    EngineTrackMode betaTrack = EngineTrackMode::Add;
    KoppaTrigger koppaTrigger = KoppaTrigger::OnAllMu;
    Mt10Behavior mt10Behavior = Mt10Behavior::ForcedPsi;
    RatioTriggerMode ratioTriggerMode = RatioTriggerMode::None;
    PrimeTarget primeTarget = PrimeTarget::NewUpsilon;
    SignFlipMode signFlipMode = SignFlipMode::None;

    bool dualTrackSymmetry = false;
    bool triplePsi = false;
    bool multiLevelKoppa = false;
    bool asymmetricCascade = false;
    bool conditionalTriplePsi = false;
    bool koppaGatedEngine = false;
    bool deltaCrossPropagation = false;
    bool deltaKoppaOffset = false;
    bool ratioThresholdPsi = false;
    bool stackDepthModes = false;
    bool epsilonPhiTriangle = false;
    bool modularWrap = false;
    bool psiStrengthParameter = false;
    bool ratioSnapshotLogging = false;
    bool feedbackOscillator = false;

    QString upsilonSeed = "3/5";
    QString betaSeed = "5/7";
    QString koppaSeed = "0/1";
    quint32 tickCount = 5;
    quint32 microTickIntervalMs = 150;
    quint32 koppaWrapThreshold = 0;

    QString configPath;

    static QString toString(PsiMode mode);
    static QString toString(KoppaMode mode);
    static QString toString(EngineMode mode);
    static QString toString(EngineTrackMode mode);
    static QString toString(KoppaTrigger trigger);
    static QString toString(Mt10Behavior behavior);
    static QString toString(RatioTriggerMode mode);
    static QString toString(PrimeTarget target);
    static QString toString(SignFlipMode mode);

    QJsonObject toJson() const;
    static TRTSConfig fromJson(const QJsonObject &object);
};

QStringList psiModeLabels();
QStringList koppaModeLabels();
QStringList engineModeLabels();
QStringList engineTrackLabels();
QStringList koppaTriggerLabels();
QStringList mt10BehaviorLabels();
QStringList ratioTriggerLabels();
QStringList primeTargetLabels();
QStringList signFlipLabels();

