#pragma once

#include <QObject>
#include <QString>
#include <QVector>

class QJsonObject;
class QProcess;

struct TunnelServerData
{
    QString userName;
    QString ip;
    QString port;
};

struct ForwardData
{
    QString localPort;
    QString destIP;
    QString destPort;

    bool operator==(const ForwardData& other) const
    {
        return localPort == other.localPort && destIP == other.destIP && destPort == other.destPort;
    }
};

class Tunnel : public QObject
{
    Q_OBJECT

public:
    Tunnel();
    ~Tunnel();

public:
    const TunnelServerData& getTunnelServerData() { return m_serverData; }
    void setTunnelServerData(const TunnelServerData& serverData) { m_serverData = serverData; }

    const QVector<ForwardData>& getForwardData() { return m_forwardData; }
    void setForwardData(const QVector<ForwardData>& forwardData) { m_forwardData = forwardData; }

    QJsonObject save();
    void load(const QJsonObject& jsonDataObj);

    void requestToggleTunnelStatus();

signals:
    void tunnelStatusChanged(bool running);
    void tunnelError(const QString& errmsg);

private:
    TunnelServerData m_serverData;
    QVector<ForwardData> m_forwardData;
    QProcess* m_process = nullptr;
};