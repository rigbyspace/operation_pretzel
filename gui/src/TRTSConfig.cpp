#include "TRTSConfig.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QObject>
#include <QPair>

namespace {
QStringList labelsFromPairs(const QList<QPair<int, QString>> &pairs) {
    QStringList list;
    for (const auto &pair : pairs) {
        list << pair.second;
    }
    return list;
}
}

static QString valueToLabel(int value, const QList<QPair<int, QString>> &pairs) {
    for (const auto &pair : pairs) {
        if (pair.first == value) {
            return pair.second;
        }
    }
    return QString();
}

static QList<QPair<int, QString>> psiModePairs() {
    return {
        {static_cast<int>(TRTSConfig::PsiMode::MStep), QObject::tr("mstep")},
        {static_cast<int>(TRTSConfig::PsiMode::RhoOnly), QObject::tr("rho only")},
        {static_cast<int>(TRTSConfig::PsiMode::MStepRho), QObject::tr("mstep+rho")},
        {static_cast<int>(TRTSConfig::PsiMode::InhibitRho), QObject::tr("inhibit rho")}
    };
}

static QList<QPair<int, QString>> koppaModePairs() {
    return {
        {static_cast<int>(TRTSConfig::KoppaMode::Dump), QObject::tr("dump")},
        {static_cast<int>(TRTSConfig::KoppaMode::Pop), QObject::tr("pop")},
        {static_cast<int>(TRTSConfig::KoppaMode::Accumulate), QObject::tr("accumulate")}
    };
}

static QList<QPair<int, QString>> engineModePairs() {
    return {
        {static_cast<int>(TRTSConfig::EngineMode::Add), QObject::tr("add")},
        {static_cast<int>(TRTSConfig::EngineMode::Multi), QObject::tr("multi")},
        {static_cast<int>(TRTSConfig::EngineMode::Slide), QObject::tr("slide")},
        {static_cast<int>(TRTSConfig::EngineMode::DeltaAdd), QObject::tr("delta-add")}
    };
}

static QList<QPair<int, QString>> engineTrackPairs() {
    return {
        {static_cast<int>(TRTSConfig::EngineTrackMode::Add), QObject::tr("ADD")},
        {static_cast<int>(TRTSConfig::EngineTrackMode::Multi), QObject::tr("MULTI")},
        {static_cast<int>(TRTSConfig::EngineTrackMode::Slide), QObject::tr("SLIDE")}
    };
}

static QList<QPair<int, QString>> koppaTriggerPairs() {
    return {
        {static_cast<int>(TRTSConfig::KoppaTrigger::OnPsi), QObject::tr("On ψ")},
        {static_cast<int>(TRTSConfig::KoppaTrigger::OnMuAfterPsi), QObject::tr("On μ after ψ")},
        {static_cast<int>(TRTSConfig::KoppaTrigger::OnAllMu), QObject::tr("On all μ")}
    };
}

static QList<QPair<int, QString>> mt10BehaviorPairs() {
    return {
        {static_cast<int>(TRTSConfig::Mt10Behavior::ForcedEmissionOnly), QObject::tr("Forced emission" )},
        {static_cast<int>(TRTSConfig::Mt10Behavior::ForcedPsi), QObject::tr("Forced ψ" )}
    };
}

static QList<QPair<int, QString>> ratioTriggerPairs() {
    return {
        {static_cast<int>(TRTSConfig::RatioTriggerMode::None), QObject::tr("none")},
        {static_cast<int>(TRTSConfig::RatioTriggerMode::Golden), QObject::tr("golden")},
        {static_cast<int>(TRTSConfig::RatioTriggerMode::Sqrt2), QObject::tr("sqrt(2)")},
        {static_cast<int>(TRTSConfig::RatioTriggerMode::Plastic), QObject::tr("plastic")}
    };
}

static QList<QPair<int, QString>> primeTargetPairs() {
    return {
        {static_cast<int>(TRTSConfig::PrimeTarget::Memory), QObject::tr("ε memory")},
        {static_cast<int>(TRTSConfig::PrimeTarget::NewUpsilon), QObject::tr("υ new")}
    };
}

static QList<QPair<int, QString>> signFlipPairs() {
    return {
        {static_cast<int>(TRTSConfig::SignFlipMode::None), QObject::tr("none")},
        {static_cast<int>(TRTSConfig::SignFlipMode::Always), QObject::tr("always")},
        {static_cast<int>(TRTSConfig::SignFlipMode::Alternate), QObject::tr("alternate")}
    };
}

QString TRTSConfig::toString(TRTSConfig::PsiMode mode) {
    return valueToLabel(static_cast<int>(mode), psiModePairs());
}

QString TRTSConfig::toString(TRTSConfig::KoppaMode mode) {
    return valueToLabel(static_cast<int>(mode), koppaModePairs());
}

QString TRTSConfig::toString(TRTSConfig::EngineMode mode) {
    return valueToLabel(static_cast<int>(mode), engineModePairs());
}

QString TRTSConfig::toString(TRTSConfig::EngineTrackMode mode) {
    return valueToLabel(static_cast<int>(mode), engineTrackPairs());
}

QString TRTSConfig::toString(TRTSConfig::KoppaTrigger trigger) {
    return valueToLabel(static_cast<int>(trigger), koppaTriggerPairs());
}

QString TRTSConfig::toString(TRTSConfig::Mt10Behavior behavior) {
    return valueToLabel(static_cast<int>(behavior), mt10BehaviorPairs());
}

QString TRTSConfig::toString(TRTSConfig::RatioTriggerMode mode) {
    return valueToLabel(static_cast<int>(mode), ratioTriggerPairs());
}

QString TRTSConfig::toString(TRTSConfig::PrimeTarget target) {
    return valueToLabel(static_cast<int>(target), primeTargetPairs());
}

QString TRTSConfig::toString(TRTSConfig::SignFlipMode mode) {
    return valueToLabel(static_cast<int>(mode), signFlipPairs());
}

QJsonObject TRTSConfig::toJson() const {
    QJsonObject obj;
    obj.insert("psi_mode", static_cast<int>(psiMode));
    obj.insert("koppa_mode", static_cast<int>(koppaMode));
    obj.insert("engine_mode", static_cast<int>(engineMode));
    obj.insert("upsilon_track", static_cast<int>(upsilonTrack));
    obj.insert("beta_track", static_cast<int>(betaTrack));
    obj.insert("koppa_trigger", static_cast<int>(koppaTrigger));
    obj.insert("mt10_behavior", static_cast<int>(mt10Behavior));
    obj.insert("ratio_trigger_mode", static_cast<int>(ratioTriggerMode));
    obj.insert("prime_target", static_cast<int>(primeTarget));
    obj.insert("sign_flip_mode", static_cast<int>(signFlipMode));
    obj.insert("dual_track_symmetry", dualTrackSymmetry);
    obj.insert("triple_psi", triplePsi);
    obj.insert("multi_level_koppa", multiLevelKoppa);
    obj.insert("asymmetric_cascade", asymmetricCascade);
    obj.insert("conditional_triple_psi", conditionalTriplePsi);
    obj.insert("koppa_gated_engine", koppaGatedEngine);
    obj.insert("delta_cross_propagation", deltaCrossPropagation);
    obj.insert("delta_koppa_offset", deltaKoppaOffset);
    obj.insert("ratio_threshold_psi", ratioThresholdPsi);
    obj.insert("stack_depth_modes", stackDepthModes);
    obj.insert("epsilon_phi_triangle", epsilonPhiTriangle);
    obj.insert("modular_wrap", modularWrap);
    obj.insert("psi_strength_parameter", psiStrengthParameter);
    obj.insert("ratio_snapshot_logging", ratioSnapshotLogging);
    obj.insert("feedback_oscillator", feedbackOscillator);
    obj.insert("upsilon_seed", upsilonSeed);
    obj.insert("beta_seed", betaSeed);
    obj.insert("koppa_seed", koppaSeed);
    obj.insert("tick_count", static_cast<int>(tickCount));
    obj.insert("microtick_interval_ms", static_cast<int>(microTickIntervalMs));
    obj.insert("koppa_wrap_threshold", static_cast<int>(koppaWrapThreshold));
    if (!configPath.isEmpty()) {
        obj.insert("config_path", configPath);
    }
    return obj;
}

TRTSConfig TRTSConfig::fromJson(const QJsonObject &object) {
    TRTSConfig config;
    if (object.contains("psi_mode")) {
        config.psiMode = static_cast<PsiMode>(object.value("psi_mode").toInt());
    }
    if (object.contains("koppa_mode")) {
        config.koppaMode = static_cast<KoppaMode>(object.value("koppa_mode").toInt());
    }
    if (object.contains("engine_mode")) {
        config.engineMode = static_cast<EngineMode>(object.value("engine_mode").toInt());
    }
    if (object.contains("upsilon_track")) {
        config.upsilonTrack = static_cast<EngineTrackMode>(object.value("upsilon_track").toInt());
    }
    if (object.contains("beta_track")) {
        config.betaTrack = static_cast<EngineTrackMode>(object.value("beta_track").toInt());
    }
    if (object.contains("koppa_trigger")) {
        config.koppaTrigger = static_cast<KoppaTrigger>(object.value("koppa_trigger").toInt());
    }
    if (object.contains("mt10_behavior")) {
        config.mt10Behavior = static_cast<Mt10Behavior>(object.value("mt10_behavior").toInt());
    }
    if (object.contains("ratio_trigger_mode")) {
        config.ratioTriggerMode = static_cast<RatioTriggerMode>(object.value("ratio_trigger_mode").toInt());
    }
    if (object.contains("prime_target")) {
        config.primeTarget = static_cast<PrimeTarget>(object.value("prime_target").toInt());
    }
    if (object.contains("sign_flip_mode")) {
        config.signFlipMode = static_cast<SignFlipMode>(object.value("sign_flip_mode").toInt());
    }

    config.dualTrackSymmetry = object.value("dual_track_symmetry").toBool(false);
    config.triplePsi = object.value("triple_psi").toBool(false);
    config.multiLevelKoppa = object.value("multi_level_koppa").toBool(false);
    config.asymmetricCascade = object.value("asymmetric_cascade").toBool(false);
    config.conditionalTriplePsi = object.value("conditional_triple_psi").toBool(false);
    config.koppaGatedEngine = object.value("koppa_gated_engine").toBool(false);
    config.deltaCrossPropagation = object.value("delta_cross_propagation").toBool(false);
    config.deltaKoppaOffset = object.value("delta_koppa_offset").toBool(false);
    config.ratioThresholdPsi = object.value("ratio_threshold_psi").toBool(false);
    config.stackDepthModes = object.value("stack_depth_modes").toBool(false);
    config.epsilonPhiTriangle = object.value("epsilon_phi_triangle").toBool(false);
    config.modularWrap = object.value("modular_wrap").toBool(false);
    config.psiStrengthParameter = object.value("psi_strength_parameter").toBool(false);
    config.ratioSnapshotLogging = object.value("ratio_snapshot_logging").toBool(false);
    config.feedbackOscillator = object.value("feedback_oscillator").toBool(false);

    config.upsilonSeed = object.value("upsilon_seed").toString(config.upsilonSeed);
    config.betaSeed = object.value("beta_seed").toString(config.betaSeed);
    config.koppaSeed = object.value("koppa_seed").toString(config.koppaSeed);
    config.tickCount = object.value("tick_count").toInt(static_cast<int>(config.tickCount));
    config.microTickIntervalMs = object.value("microtick_interval_ms").toInt(static_cast<int>(config.microTickIntervalMs));
    config.koppaWrapThreshold = object.value("koppa_wrap_threshold").toInt(static_cast<int>(config.koppaWrapThreshold));
    config.configPath = object.value("config_path").toString();

    return config;
}

QStringList psiModeLabels() { return labelsFromPairs(psiModePairs()); }
QStringList koppaModeLabels() { return labelsFromPairs(koppaModePairs()); }
QStringList engineModeLabels() { return labelsFromPairs(engineModePairs()); }
QStringList engineTrackLabels() { return labelsFromPairs(engineTrackPairs()); }
QStringList koppaTriggerLabels() { return labelsFromPairs(koppaTriggerPairs()); }
QStringList mt10BehaviorLabels() { return labelsFromPairs(mt10BehaviorPairs()); }
QStringList ratioTriggerLabels() { return labelsFromPairs(ratioTriggerPairs()); }
QStringList primeTargetLabels() { return labelsFromPairs(primeTargetPairs()); }
QStringList signFlipLabels() { return labelsFromPairs(signFlipPairs()); }

