#include "TRTSCoreProcess.hpp"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QTextStream>
#include <QStringList>

namespace {
constexpr auto kDefaultExecutable = "./trts_engine";
constexpr auto kConfigExtension = ".trtscfg";
}

TRTSCoreProcess::TRTSCoreProcess(QObject *parent)
    : QObject(parent) {}

TRTSCoreProcess::~TRTSCoreProcess() {
    stopRun();
}

bool TRTSCoreProcess::startRun(const TRTSConfig &config) {
    if (isRunning()) {
        emit statusMessage(tr("Engine already running"));
        return false;
    }

    const QString executable = resolveEngineExecutable();
    if (executable.isEmpty()) {
        emit engineError(tr("Unable to locate TRTS engine executable"));
        return false;
    }

    const QString configFile = writeConfigToFile(config);
    if (configFile.isEmpty()) {
        emit engineError(tr("Failed to serialize configuration"));
        return false;
    }

    m_process = std::make_unique<QProcess>(this);
    m_config = config;

    connect(m_process.get(), &QProcess::readyReadStandardOutput, this, &TRTSCoreProcess::handleReadyRead);
    connect(m_process.get(), &QProcess::readyReadStandardError, this, &TRTSCoreProcess::handleReadyReadError);
    connect(m_process.get(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &TRTSCoreProcess::handleProcessFinished);

    QStringList arguments;
    arguments << "--config" << configFile;

    emit statusMessage(tr("Launching TRTS engine with %1").arg(QDir::toNativeSeparators(configFile)));
    m_process->setProgram(executable);
    m_process->setArguments(arguments);
    m_process->start();

    if (!m_process->waitForStarted(5000)) {
        emit engineError(tr("Failed to launch TRTS engine"));
        m_process.reset();
        return false;
    }

    emit statusMessage(tr("TRTS run started"));
    return true;
}

void TRTSCoreProcess::stopRun() {
    if (!isRunning()) {
        return;
    }

    emit statusMessage(tr("Stopping TRTS engine"));
    m_process->kill();
    m_process->waitForFinished(2000);
    m_process.reset();
}

bool TRTSCoreProcess::isRunning() const {
    return m_process && m_process->state() != QProcess::NotRunning;
}

void TRTSCoreProcess::handleReadyRead() {
    if (!m_process) {
        return;
    }

    while (m_process->canReadLine()) {
        const QString line = QString::fromUtf8(m_process->readLine()).trimmed();
        if (!line.isEmpty()) {
            emit engineOutput(line);
        }
    }
}

void TRTSCoreProcess::handleReadyReadError() {
    if (!m_process) {
        return;
    }

    while (m_process->canReadLine()) {
        const QString line = QString::fromUtf8(m_process->readLine()).trimmed();
        if (!line.isEmpty()) {
            emit engineError(line);
        }
    }
}

void TRTSCoreProcess::handleProcessFinished(int exitCode, QProcess::ExitStatus status) {
    emit runFinished(exitCode, status);
    if (status != QProcess::NormalExit) {
        emit engineError(tr("Engine terminated abnormally"));
    } else {
        emit statusMessage(tr("TRTS run finished with code %1").arg(exitCode));
    }
    m_process.reset();
}

QString TRTSCoreProcess::resolveEngineExecutable() const {
    const QString overridePath = qEnvironmentVariable("TRTS_ENGINE_EXECUTABLE");
    if (!overridePath.isEmpty() && QFileInfo::exists(overridePath)) {
        return overridePath;
    }

    const QString appDirCandidate = QDir(QCoreApplication::applicationDirPath()).filePath(kDefaultExecutable);
    if (QFileInfo::exists(appDirCandidate)) {
        return appDirCandidate;
    }

    const QString repoCandidate = QDir::current().filePath(kDefaultExecutable);
    if (QFileInfo::exists(repoCandidate)) {
        return repoCandidate;
    }

    const QString buildCandidate = QDir::current().filePath(QStringLiteral("build/trts_engine"));
    if (QFileInfo::exists(buildCandidate)) {
        return buildCandidate;
    }

    return QString();
}

QString TRTSCoreProcess::writeConfigToFile(const TRTSConfig &config) const {
    QDir dir(QDir::current().filePath("runs"));
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    const QString timestamp = QDateTime::currentDateTimeUtc().toString("yyyyMMdd_hhmmsszzz");
    const QString fileName = QString("run_%1%2").arg(timestamp, kConfigExtension);
    const QString fullPath = dir.filePath(fileName);

    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return QString();
    }

    const QJsonDocument doc(config.toJson());
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return fullPath;
}

