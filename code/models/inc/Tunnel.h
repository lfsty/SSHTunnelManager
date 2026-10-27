#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVector>

#include "ForwardData.h"

class QJsonObject;
class QProcess;

// 单个 ssh 隧道: 跳板机信息 + 多条本地端口转发规则
class Tunnel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY configurationChanged)
    Q_PROPERTY(QString serverIp READ serverIp WRITE setServerIp NOTIFY configurationChanged)
    Q_PROPERTY(QString serverPort READ serverPort WRITE setServerPort NOTIFY configurationChanged)
    Q_PROPERTY(QVariantList forwards READ forwards NOTIFY configurationChanged)
    Q_PROPERTY(bool configured READ isConfigured NOTIFY configurationChanged)
    Q_PROPERTY(QString title READ title NOTIFY configurationChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

public:
    explicit Tunnel(QObject* parent = nullptr);
    ~Tunnel() override;

    QString userName() const;
    void setUserName(const QString& userName);

    QString serverIp() const;
    void setServerIp(const QString& serverIp);

    QString serverPort() const;
    void setServerPort(const QString& serverPort);

    QVariantList forwards() const;
    void setForwards(const QVariantList& forwards);

    bool isConfigured() const;
    QString title() const;

    bool isRunning() const;
    QString lastError() const;

    Q_INVOKABLE void applyConfiguration(const QString& userName, const QString& serverIp, const QString& serverPort, const QVariantList& forwards);
    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void toggle();
    Q_INVOKABLE void clearError();

    QJsonObject toJson() const;
    void loadFromJson(const QJsonObject& json);

signals:
    void configurationChanged();
    void runningChanged();
    void lastErrorChanged();

private:
    void ensureProcess();
    void setRunning(bool running);
    void setLastError(const QString& error);
    void logMessage(const QString& message);
    QString validationError() const;

    QString m_userName;
    QString m_serverIp;
    QString m_serverPort;
    QVector<ForwardData> m_forwards;

    QProcess* m_process = nullptr;
    bool m_running      = false;
    bool m_stopping     = false;
    QString m_lastError;
};
