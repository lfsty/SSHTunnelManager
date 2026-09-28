#include "LogManager.h"

#include <QFile>
#include <QMutex>
#include <QMutexLocker>

#include <QDebug>

LogManager* LogManager::getInstance()
{
    static LogManager logManager;
    return &logManager;
}

void LogManager::log(const QString& log)
{
    static QMutex _mutex;
    QMutexLocker _locker(&_mutex);

    if (m_file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        m_file->write(log.toUtf8());
        m_file->write("\n");
        m_file->close();
    }
}

LogManager::LogManager()
{
    m_file = new QFile("log.txt");
}

LogManager::~LogManager()
{
    delete m_file;
}
