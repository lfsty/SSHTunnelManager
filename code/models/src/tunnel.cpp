#include "tunnel.h"

#include <QJsonArray>
#include <QJsonObject>

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

void Tunnel::requestStartTunnel()
{
    // TODO: refactor this
    m_tunnelIsRunning = !m_tunnelIsRunning;
    emit tunnelStatusChanged(m_tunnelIsRunning);
}
