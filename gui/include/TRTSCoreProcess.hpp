#pragma once

#include "TRTSConfig.hpp"

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <memory>

class TRTSCoreProcess : public QObject {
    Q_OBJECT
public:
    explicit TRTSCoreProcess(QObject *parent = nullptr);
    ~TRTSCoreProcess() override;

    bool startRun(const TRTSConfig &config);
    void stopRun();
    bool isRunning() const;

    TRTSConfig currentConfig() const { return m_config; }

signals:
    void engineOutput(const QString &line);
    void engineError(const QString &line);
    void runFinished(int exitCode, QProcess::ExitStatus status);
    void statusMessage(const QString &message);

private slots:
    void handleReadyRead();
    void handleReadyReadError();
    void handleProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
    QString resolveEngineExecutable() const;
    QString writeConfigToFile(const TRTSConfig &config) const;

    std::unique_ptr<QProcess> m_process;
    TRTSConfig m_config;
};

