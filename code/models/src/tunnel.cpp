#include "tunnel.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include <QDebug>

namespace
{
constexpr const char* JUMPSERVER_USER_KEY = "jumpserverUserName";
constexpr const char* JUMPSERVER_IP_KEY   = "jumpserverIP";
constexpr const char* JUMPSERVER_PORT_KEY = "jumpserverPort";

constexpr const char* FORWARD_DATA_LIST_KEY  = "forwardData";
constexpr const char* FORWARD_LOCAL_PORT_KEY = "localPort";
constexpr const char* FORWARD_DEST_IP_KEY    = "destIP";
constexpr const char* FORWARD_DEST_PORT_KEY  = "destPort";
}  // namespace

Tunnel::Tunnel()
{
}

Tunnel::~Tunnel()
{
    if (m_process)
    {
        m_process->disconnect();
        m_process->kill();
        m_process->waitForFinished();

        delete m_process;
        m_process = nullptr;
    }
}

QJsonObject Tunnel::save()
{
    QJsonObject rootObj;

    rootObj[JUMPSERVER_USER_KEY] = m_serverData.userName;
    rootObj[JUMPSERVER_IP_KEY]   = m_serverData.ip;
    rootObj[JUMPSERVER_PORT_KEY] = m_serverData.port;

    if (!m_forwardData.empty())
    {
        QJsonArray forwardDataArray;

        for (const auto& forwardData : m_forwardData)
        {
            QJsonObject forwardDataObj;
            forwardDataObj[FORWARD_LOCAL_PORT_KEY] = forwardData.localPort;
            forwardDataObj[FORWARD_DEST_IP_KEY]    = forwardData.destIP;
            forwardDataObj[FORWARD_DEST_PORT_KEY]  = forwardData.destPort;
            forwardDataArray.append(forwardDataObj);
        }

        rootObj[FORWARD_DATA_LIST_KEY] = forwardDataArray;
    }

    return rootObj;
}

void Tunnel::load(const QJsonObject& jsonDataObj)
{
    if (jsonDataObj.contains(JUMPSERVER_USER_KEY))
    {
        m_serverData.userName = jsonDataObj[JUMPSERVER_USER_KEY].toString();
    }

    if (jsonDataObj.contains(JUMPSERVER_IP_KEY))
    {
        m_serverData.ip = jsonDataObj[JUMPSERVER_IP_KEY].toString();
    }

    if (jsonDataObj.contains(JUMPSERVER_PORT_KEY))
    {
        m_serverData.port = jsonDataObj[JUMPSERVER_PORT_KEY].toString();
    }

    if (jsonDataObj.contains(FORWARD_DATA_LIST_KEY))
    {
        QJsonArray forwardDataArray = jsonDataObj[FORWARD_DATA_LIST_KEY].toArray();
        for (const auto& forwardDataRef : forwardDataArray)
        {
            QJsonObject forwardDataObj = forwardDataRef.toObject();

            ForwardData forwardData;
            forwardData.localPort = forwardDataObj[FORWARD_LOCAL_PORT_KEY].toString();
            forwardData.destIP    = forwardDataObj[FORWARD_DEST_IP_KEY].toString();
            forwardData.destPort  = forwardDataObj[FORWARD_DEST_PORT_KEY].toString();
            m_forwardData.append(forwardData);
        }
    }
}

void Tunnel::requestToggleTunnelStatus()
{
    if (m_serverData.ip.isEmpty() || m_serverData.port.isEmpty() || m_serverData.userName.isEmpty() || m_forwardData.empty())
    {
        return;
    }

    if (m_process == nullptr)
    {
        m_process = new QProcess(this);
#ifdef Q_OS_WIN
        m_process->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments* args)
                                                     {
                                                         args->flags |= CREATE_NO_WINDOW;  // 关键：完全隐藏黑窗口
                                                     });
#endif

        connect(m_process, &QProcess::readyReadStandardError, this, [=]()
                {
                    emit tunnelStatusChanged(false);
                    emit tunnelError(m_process->readAllStandardError());
                    m_process->kill();
                });

        connect(m_process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus exitStatus)
                {
                    emit tunnelStatusChanged(false);
                });
    }

    QStringList arguments;
    arguments << "-N";
    for (const ForwardData& forward : m_forwardData)
    {
        arguments << "-L" << QString("%1:%2:%3").arg(forward.localPort).arg(forward.destIP).arg(forward.destPort);
    }
    arguments << QString("%1@%2").arg(m_serverData.userName).arg(m_serverData.ip)
              << "-p" << m_serverData.port;

    if (m_process->state() == QProcess::NotRunning)
    {
        m_process->start("ssh", arguments);
        emit tunnelStatusChanged(true);
    }
    else
    {
        m_process->kill();
    }
}
