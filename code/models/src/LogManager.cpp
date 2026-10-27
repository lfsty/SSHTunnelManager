#include "LogManager.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace
{
constexpr const char* LOG_FILE_NAME = "log.txt";

QString formatMessage(const QString& message)
{
    return QString("[%1] %2").arg(QDateTime::currentDateTime().toString(Qt::ISODateWithMs), message);
}
}  // namespace

LogManager::LogManager(QObject* parent) : QObject(parent),
                                          m_filePath(resolveFilePath())
{
    const QDir dir = QFileInfo(m_filePath).absoluteDir();
    if (!dir.exists())
    {
        QDir().mkpath(dir.absolutePath());
    }
}

LogManager* LogManager::instance()
{
    static LogManager manager;
    return &manager;
}

QString LogManager::filePath() const
{
    return m_filePath;
}

void LogManager::log(const QString& message)
{
    if (message.isEmpty())
    {
        return;
    }

    QMutexLocker locker(&m_mutex);

    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        return;
    }

    file.write(formatMessage(message).toUtf8());
    file.write("\n");
    file.close();
}

void LogManager::installMessageHandler()
{
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& context, const QString& message)
                           {
                               Q_UNUSED(context)

                               QString level;
                               switch (type)
                               {
                               case QtDebugMsg:
                                   level = "debug";
                                   break;
                               case QtInfoMsg:
                                   level = "info";
                                   break;
                               case QtWarningMsg:
                                   level = "warning";
                                   break;
                               case QtCriticalMsg:
                                   level = "critical";
                                   break;
                               case QtFatalMsg:
                                   level = "fatal";
                                   break;
                               }

                               instance()->log(QString("[qt:%1] %2").arg(level, message));
                           });
}

QString LogManager::resolveFilePath()
{
    // 数据统一保存在程序当前工作目录
    return QDir::current().filePath(LOG_FILE_NAME);
}
