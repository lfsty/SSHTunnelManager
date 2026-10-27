#include "Tunnel.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>

#include "LogManager.h"
#include "SshOutput.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace
{
constexpr const char* JUMPSERVER_USER_KEY = "jumpserverUserName";
constexpr const char* JUMPSERVER_IP_KEY   = "jumpserverIP";
constexpr const char* JUMPSERVER_PORT_KEY = "jumpserverPort";
constexpr const char* FORWARD_DATA_KEY    = "forwardData";

constexpr int STOP_TIMEOUT_MS = 3000;
}  // namespace

Tunnel::Tunnel(QObject* parent) : QObject(parent)
{
}

Tunnel::~Tunnel()
{
    if (m_process)
    {
        m_process->disconnect(this);
        m_process->kill();
        m_process->waitForFinished(STOP_TIMEOUT_MS);
    }
}

QString Tunnel::userName() const
{
    return m_userName;
}

void Tunnel::setUserName(const QString& userName)
{
    const QString value = userName.trimmed();
    if (m_userName == value)
    {
        return;
    }

    m_userName = value;
    emit configurationChanged();
}

QString Tunnel::serverIp() const
{
    return m_serverIp;
}

void Tunnel::setServerIp(const QString& serverIp)
{
    const QString value = serverIp.trimmed();
    if (m_serverIp == value)
    {
        return;
    }

    m_serverIp = value;
    emit configurationChanged();
}

QString Tunnel::serverPort() const
{
    return m_serverPort;
}

void Tunnel::setServerPort(const QString& serverPort)
{
    const QString value = serverPort.trimmed();
    if (m_serverPort == value)
    {
        return;
    }

    m_serverPort = value;
    emit configurationChanged();
}

QVariantList Tunnel::forwards() const
{
    return ForwardData::toVariantList(m_forwards);
}

void Tunnel::setForwards(const QVariantList& forwards)
{
    QVector<ForwardData> data = ForwardData::fromVariantList(forwards);
    if (data == m_forwards)
    {
        return;
    }

    m_forwards = data;
    emit configurationChanged();
}

bool Tunnel::isConfigured() const
{
    return validationError().isEmpty();
}

QString Tunnel::title() const
{
    if (m_userName.isEmpty() && m_serverIp.isEmpty() && m_serverPort.isEmpty())
    {
        return {};
    }

    return QString("%1@%2:%3").arg(m_userName, m_serverIp, m_serverPort);
}

bool Tunnel::isRunning() const
{
    return m_running;
}

QString Tunnel::lastError() const
{
    return m_lastError;
}

void Tunnel::applyConfiguration(const QString& userName, const QString& serverIp, const QString& serverPort, const QVariantList& forwards)
{
    if (m_running)
    {
        return;
    }

    const QVector<ForwardData> forwardData = ForwardData::fromVariantList(forwards);

    const QString newUserName   = userName.trimmed();
    const QString newServerIp   = serverIp.trimmed();
    const QString newServerPort = serverPort.trimmed();

    const bool changed = m_userName != newUserName || m_serverIp != newServerIp || m_serverPort != newServerPort || m_forwards != forwardData;
    if (!changed)
    {
        return;
    }

    m_userName   = newUserName;
    m_serverIp   = newServerIp;
    m_serverPort = newServerPort;
    m_forwards   = forwardData;

    clearError();
    emit configurationChanged();
}

void Tunnel::start()
{
    if (m_running)
    {
        return;
    }

    const QString error = validationError();
    if (!error.isEmpty())
    {
        setLastError(error);
        logMessage(error);
        return;
    }

    if (m_process && m_process->state() != QProcess::NotRunning)
    {
        return;
    }

    ensureProcess();

    QStringList arguments;
    arguments << "-N";
    for (const ForwardData& forward : m_forwards)
    {
        arguments << "-L" << QString("%1:%2:%3").arg(forward.localPort, forward.destIP, forward.destPort);
    }
    arguments << QString("%1@%2").arg(m_userName, m_serverIp) << "-p" << m_serverPort;

    clearError();
    m_stopping = false;
    m_process->start("ssh", arguments);
    setRunning(true);
}

void Tunnel::stop()
{
    if (!m_process || m_process->state() == QProcess::NotRunning)
    {
        setRunning(false);
        return;
    }

    m_stopping = true;
    m_process->kill();
    m_process->waitForFinished(STOP_TIMEOUT_MS);
    setRunning(false);
}

void Tunnel::toggle()
{
    if (m_running)
    {
        stop();
    }
    else
    {
        start();
    }
}

void Tunnel::clearError()
{
    setLastError(QString());
}

QJsonObject Tunnel::toJson() const
{
    QJsonObject json;
    json[JUMPSERVER_USER_KEY] = m_userName;
    json[JUMPSERVER_IP_KEY]   = m_serverIp;
    json[JUMPSERVER_PORT_KEY] = m_serverPort;

    QJsonArray forwardArray;
    for (const ForwardData& forward : m_forwards)
    {
        forwardArray.append(forward.toJson());
    }
    json[FORWARD_DATA_KEY] = forwardArray;

    return json;
}

void Tunnel::loadFromJson(const QJsonObject& json)
{
    m_userName   = json.value(JUMPSERVER_USER_KEY).toString();
    m_serverIp   = json.value(JUMPSERVER_IP_KEY).toString();
    m_serverPort = json.value(JUMPSERVER_PORT_KEY).toString();

    m_forwards.clear();
    const QJsonArray forwardArray = json.value(FORWARD_DATA_KEY).toArray();
    for (const QJsonValue& value : forwardArray)
    {
        m_forwards.append(ForwardData::fromJson(value.toObject()));
    }

    emit configurationChanged();
}

void Tunnel::ensureProcess()
{
    if (m_process)
    {
        return;
    }

    m_process = new QProcess(this);

#ifdef Q_OS_WIN
    m_process->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments* args)
                                                 {
                                                     args->flags |= CREATE_NO_WINDOW;
                                                 });
#endif

    connect(m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error)
            {
                if (error != QProcess::FailedToStart)
                {
                    return;
                }

                setRunning(false);
                setLastError(tr("无法启动 ssh 命令，请确认已安装 OpenSSH 客户端并加入 PATH"));
                logMessage(m_lastError);
            });

    connect(m_process, &QProcess::readyReadStandardError, this, [this]()
            {
                const QString message = SshOutput::decode(m_process->readAllStandardError()).trimmed();
                if (message.isEmpty())
                {
                    return;
                }

                logMessage(message);
                if (!m_stopping)
                {
                    setLastError(message);
                }
            });

    connect(m_process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [this](int exitCode, QProcess::ExitStatus exitStatus)
            {
                const QString message = SshOutput::decode(m_process->readAllStandardError()).trimmed();
                if (!message.isEmpty())
                {
                    logMessage(message);
                }

                const bool failed = !m_stopping && (exitStatus != QProcess::NormalExit || exitCode != 0);
                if (failed)
                {
                    if (!message.isEmpty())
                    {
                        setLastError(message);
                    }
                    else if (m_lastError.isEmpty())
                    {
                        setLastError(tr("ssh 进程已退出（退出码 %1）").arg(exitCode));
                    }
                }

                m_stopping = false;
                setRunning(false);
            });
}

void Tunnel::setRunning(bool running)
{
    if (m_running == running)
    {
        return;
    }

    m_running = running;
    emit runningChanged();
}

void Tunnel::setLastError(const QString& error)
{
    if (m_lastError == error)
    {
        return;
    }

    m_lastError = error;
    emit lastErrorChanged();
}

void Tunnel::logMessage(const QString& message)
{
    LogManager::instance()->log(message);
}

QString Tunnel::validationError() const
{
    if (m_userName.isEmpty())
    {
        return tr("请先配置跳板机用户名");
    }

    if (m_serverIp.isEmpty())
    {
        return tr("请先配置跳板机地址");
    }

    if (!ForwardData::isValidPort(m_serverPort))
    {
        return tr("跳板机端口无效");
    }

    if (m_forwards.isEmpty())
    {
        return tr("至少需要一条端口转发规则");
    }

    for (const ForwardData& forward : m_forwards)
    {
        if (!forward.isValid())
        {
            return tr("端口转发规则不完整");
        }
    }

    return {};
}
