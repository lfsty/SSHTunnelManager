#include "TunnelManager.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QUrl>

#include "LogManager.h"
#include "Tunnel.h"
#include "TunnelListModel.h"

namespace
{
constexpr const char* STORAGE_FILE_NAME = "tunnel.json";
constexpr const char* SAVED_TIME_KEY    = "savedTime";
constexpr const char* TUNNEL_LIST_KEY   = "sshTunnelData";
}  // namespace

TunnelManager::TunnelManager(QObject* parent) : QObject(parent),
                                                m_tunnels(new TunnelListModel(this)),
                                                m_storagePath(QDir::current().filePath(STORAGE_FILE_NAME))
{
    connect(m_tunnels, &TunnelListModel::countChanged, this, &TunnelManager::runningCountChanged);
    connect(m_tunnels, &TunnelListModel::tunnelRunningChanged, this, &TunnelManager::runningCountChanged);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &TunnelManager::save);
}

TunnelManager::~TunnelManager()
{
    stopAll();
}

TunnelListModel* TunnelManager::tunnels() const
{
    return m_tunnels;
}

QString TunnelManager::storagePath() const
{
    return m_storagePath;
}

QString TunnelManager::logPath() const
{
    return LogManager::instance()->filePath();
}

int TunnelManager::runningCount() const
{
    int count = 0;
    for (const Tunnel* tunnel : m_tunnels->tunnels())
    {
        if (tunnel->isRunning())
        {
            ++count;
        }
    }
    return count;
}

Tunnel* TunnelManager::addTunnel()
{
    Tunnel* tunnel = m_tunnels->addTunnel();
    save();
    return tunnel;
}

void TunnelManager::removeTunnel(int row)
{
    m_tunnels->removeTunnel(row);
    save();
}

void TunnelManager::load()
{
    const QString path = m_storagePath;
    if (!QFile::exists(path))
    {
        emit loaded();
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit errorOccurred(tr("无法读取配置文件: %1").arg(path));
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError || !document.isObject())
    {
        emit errorOccurred(tr("配置文件格式错误: %1").arg(parseError.errorString()));
        return;
    }

    m_tunnels->loadFromJson(document.object().value(TUNNEL_LIST_KEY).toArray());

    emit loaded();
}

void TunnelManager::save() const
{
    const QDir dir = QFileInfo(m_storagePath).absoluteDir();
    if (!dir.exists() && !QDir().mkpath(dir.absolutePath()))
    {
        LogManager::instance()->log(tr("无法创建配置目录: %1").arg(dir.absolutePath()));
        return;
    }

    QJsonObject root;
    root[SAVED_TIME_KEY]  = QDateTime::currentDateTime().toString();
    root[TUNNEL_LIST_KEY] = m_tunnels->toJson();

    QFile file(m_storagePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        LogManager::instance()->log(tr("无法写入配置文件: %1").arg(m_storagePath));
        return;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();
}

void TunnelManager::stopAll()
{
    for (Tunnel* tunnel : m_tunnels->tunnels())
    {
        tunnel->stop();
    }
}

void TunnelManager::openLogFile() const
{
    const QString path = logPath();

    QFile file(path);
    if (!file.exists())
    {
        if (file.open(QIODevice::WriteOnly))
        {
            file.close();
        }
    }

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path)))
    {
        LogManager::instance()->log(tr("无法打开日志文件: %1").arg(path));
    }
}
