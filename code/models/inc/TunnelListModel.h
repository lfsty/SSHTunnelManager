#pragma once

#include <QAbstractListModel>
#include <QList>

class Tunnel;
class QJsonArray;

// 隧道列表模型，向 QML 暴露 Tunnel 对象
class TunnelListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum Role
    {
        TunnelRole = Qt::UserRole + 1
    };

    explicit TunnelListModel(QObject* parent = nullptr);
    ~TunnelListModel() override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE Tunnel* addTunnel();
    Q_INVOKABLE void removeTunnel(int row);
    Q_INVOKABLE Tunnel* tunnelAt(int row) const;
    Q_INVOKABLE int indexOf(Tunnel* tunnel) const;

    QList<Tunnel*> tunnels() const;

    QJsonArray toJson() const;
    void loadFromJson(const QJsonArray& json);
    void clear();

signals:
    void countChanged();
    void tunnelRunningChanged();

private:
    void trackTunnel(Tunnel* tunnel);

    QList<Tunnel*> m_tunnels;
};
