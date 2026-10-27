#include "TunnelListModel.h"

#include <QJsonArray>
#include <QJsonObject>

#include "Tunnel.h"

TunnelListModel::TunnelListModel(QObject* parent) : QAbstractListModel(parent)
{
}

TunnelListModel::~TunnelListModel() = default;

int TunnelListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return static_cast<int>(m_tunnels.size());
}

QVariant TunnelListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_tunnels.size())
    {
        return {};
    }

    if (role != TunnelRole)
    {
        return {};
    }

    return QVariant::fromValue<Tunnel*>(m_tunnels.at(index.row()));
}

QHash<int, QByteArray> TunnelListModel::roleNames() const
{
    return { { TunnelRole, "tunnel" } };
}

Tunnel* TunnelListModel::addTunnel()
{
    const int row = static_cast<int>(m_tunnels.size());

    beginInsertRows(QModelIndex(), row, row);
    Tunnel* tunnel = new Tunnel(this);
    m_tunnels.append(tunnel);
    trackTunnel(tunnel);
    endInsertRows();

    emit countChanged();
    return tunnel;
}

void TunnelListModel::removeTunnel(int row)
{
    if (row < 0 || row >= m_tunnels.size())
    {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    Tunnel* tunnel = m_tunnels.takeAt(row);
    endRemoveRows();

    tunnel->stop();
    delete tunnel;

    emit countChanged();
}

Tunnel* TunnelListModel::tunnelAt(int row) const
{
    if (row < 0 || row >= m_tunnels.size())
    {
        return nullptr;
    }

    return m_tunnels.at(row);
}

int TunnelListModel::indexOf(Tunnel* tunnel) const
{
    return static_cast<int>(m_tunnels.indexOf(tunnel));
}

QList<Tunnel*> TunnelListModel::tunnels() const
{
    return m_tunnels;
}

QJsonArray TunnelListModel::toJson() const
{
    QJsonArray array;
    for (const Tunnel* tunnel : m_tunnels)
    {
        array.append(tunnel->toJson());
    }
    return array;
}

void TunnelListModel::loadFromJson(const QJsonArray& json)
{
    beginResetModel();

    for (Tunnel* tunnel : m_tunnels)
    {
        tunnel->stop();
        delete tunnel;
    }
    m_tunnels.clear();

    for (const QJsonValue& value : json)
    {
        Tunnel* tunnel = new Tunnel(this);
        tunnel->loadFromJson(value.toObject());
        trackTunnel(tunnel);
        m_tunnels.append(tunnel);
    }

    endResetModel();
    emit countChanged();
}

void TunnelListModel::clear()
{
    if (m_tunnels.isEmpty())
    {
        return;
    }

    beginResetModel();
    for (Tunnel* tunnel : m_tunnels)
    {
        tunnel->stop();
        delete tunnel;
    }
    m_tunnels.clear();
    endResetModel();

    emit countChanged();
}

void TunnelListModel::trackTunnel(Tunnel* tunnel)
{
    connect(tunnel, &Tunnel::runningChanged, this, &TunnelListModel::tunnelRunningChanged);
}
