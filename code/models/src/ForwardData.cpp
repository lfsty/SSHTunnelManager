#include "ForwardData.h"

#include <QJsonObject>
#include <QVariantMap>

namespace
{
constexpr const char* LOCAL_PORT_KEY = "localPort";
constexpr const char* DEST_IP_KEY    = "destIP";
constexpr const char* DEST_PORT_KEY  = "destPort";

constexpr int MIN_PORT = 1;
constexpr int MAX_PORT = 65535;
}  // namespace

bool ForwardData::isEmpty() const
{
    return localPort.trimmed().isEmpty() && destIP.trimmed().isEmpty() && destPort.trimmed().isEmpty();
}

bool ForwardData::isValid() const
{
    return isValidPort(localPort.trimmed()) && !destIP.trimmed().isEmpty() && isValidPort(destPort.trimmed());
}

bool ForwardData::operator==(const ForwardData& other) const
{
    return localPort == other.localPort && destIP == other.destIP && destPort == other.destPort;
}

bool ForwardData::operator!=(const ForwardData& other) const
{
    return !(*this == other);
}

QJsonObject ForwardData::toJson() const
{
    QJsonObject json;
    json[LOCAL_PORT_KEY] = localPort;
    json[DEST_IP_KEY]    = destIP;
    json[DEST_PORT_KEY]  = destPort;
    return json;
}

ForwardData ForwardData::fromJson(const QJsonObject& json)
{
    ForwardData forward;
    forward.localPort = json[LOCAL_PORT_KEY].toString();
    forward.destIP    = json[DEST_IP_KEY].toString();
    forward.destPort  = json[DEST_PORT_KEY].toString();
    return forward;
}

QVariantMap ForwardData::toVariantMap() const
{
    QVariantMap map;
    map[LOCAL_PORT_KEY] = localPort;
    map[DEST_IP_KEY]    = destIP;
    map[DEST_PORT_KEY]  = destPort;
    return map;
}

ForwardData ForwardData::fromVariantMap(const QVariantMap& map)
{
    ForwardData forward;
    forward.localPort = map.value(LOCAL_PORT_KEY).toString();
    forward.destIP    = map.value(DEST_IP_KEY).toString();
    forward.destPort  = map.value(DEST_PORT_KEY).toString();
    return forward;
}

QVariantList ForwardData::toVariantList(const QVector<ForwardData>& forwards)
{
    QVariantList list;
    list.reserve(forwards.size());

    for (const ForwardData& forward : forwards)
    {
        list.append(forward.toVariantMap());
    }

    return list;
}

QVector<ForwardData> ForwardData::fromVariantList(const QVariantList& forwards)
{
    QVector<ForwardData> result;
    result.reserve(forwards.size());

    for (const QVariant& variant : forwards)
    {
        result.append(fromVariantMap(variant.toMap()));
    }

    return result;
}

bool ForwardData::isValidPort(const QString& port)
{
    bool ok         = false;
    const int value = port.trimmed().toInt(&ok);
    return ok && value >= MIN_PORT && value <= MAX_PORT;
}
