#pragma once

#include <QObject>
#include <QString>

class Tunnel;
class TunnelListModel;

// QML 的唯一入口: 管理隧道列表、持久化与日志
class TunnelManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(TunnelListModel* tunnels READ tunnels CONSTANT)
    Q_PROPERTY(QString storagePath READ storagePath CONSTANT)
    Q_PROPERTY(QString logPath READ logPath CONSTANT)
    Q_PROPERTY(int runningCount READ runningCount NOTIFY runningCountChanged)

public:
    explicit TunnelManager(QObject* parent = nullptr);
    ~TunnelManager() override;

    TunnelListModel* tunnels() const;
    QString storagePath() const;
    QString logPath() const;
    int runningCount() const;

    Q_INVOKABLE Tunnel* addTunnel();
    Q_INVOKABLE void removeTunnel(int row);
    Q_INVOKABLE void load();
    Q_INVOKABLE void save() const;
    Q_INVOKABLE void stopAll();
    Q_INVOKABLE void openLogFile() const;

signals:
    void runningCountChanged();
    void errorOccurred(const QString& message);
    void loaded();

private:
    TunnelListModel* m_tunnels = nullptr;
    QString m_storagePath;
};
