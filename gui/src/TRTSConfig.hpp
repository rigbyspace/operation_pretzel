// This header defines the high‑level TRTS configuration used by the Qt GUI.
// It mirrors the upstream TRTSConfig struct found in the original project but
// extends it with an additional `fibonacciGate` flag.  The GUI uses this
// structure to gather user input, serialize/deserialize configurations, and
// eventually convert the fields into the lower‑level C `Config` structure used
// by the simulation engine.

#pragma once

#include <QString>
#include <QStringList>
#include <QJsonObject>

// The TRTSConfig struct encapsulates all user‑adjustable settings for the
// simulation.  Each enum corresponds to a C enum in config.h.  The boolean
// flags correspond to individual features that can be toggled on or off.
struct TRTSConfig {
    // High‑level mode selections
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

    // Default values for all fields.  These mirror the upstream defaults.
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

    // Feature flags
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
    // New feature: enable the Fibonacci gate in the engine.  This maps to
    // Config::enable_fibonacci_gate in the simulation layer.
    bool fibonacciGate = false;

    // Seeds and timing parameters
    QString upsilonSeed = "3/5";
    QString betaSeed = "5/7";
    QString koppaSeed = "0/1";
    quint32 tickCount = 5;
    quint32 microTickIntervalMs = 150;
    quint32 koppaWrapThreshold = 0;

    // Optional path to a saved configuration file
    QString configPath;

    // Helpers for mapping enum values to user‑friendly labels
    static QString toString(PsiMode mode);
    static QString toString(KoppaMode mode);
    static QString toString(EngineMode mode);
    static QString toString(EngineTrackMode mode);
    static QString toString(KoppaTrigger trigger);
    static QString toString(Mt10Behavior behavior);
    static QString toString(RatioTriggerMode mode);
    static QString toString(PrimeTarget target);
    static QString toString(SignFlipMode mode);

    // Serialize this configuration into a JSON object.  Only non‑default fields
    // are emitted when appropriate (e.g. configPath).  Boolean flags are
    // emitted unconditionally to avoid confusion when toggling features on/off.
    QJsonObject toJson() const;
    // Construct a configuration from a JSON object.  Missing fields fall back
    // to the defaults defined above.
    static TRTSConfig fromJson(const QJsonObject &object);
};

// Retrieve lists of labels for each enumeration for use in combo boxes.
QStringList psiModeLabels();
QStringList koppaModeLabels();
QStringList engineModeLabels();
QStringList engineTrackLabels();
QStringList koppaTriggerLabels();
QStringList mt10BehaviorLabels();
QStringList ratioTriggerLabels();
QStringList primeTargetLabels();
QStringList signFlipLabels();
