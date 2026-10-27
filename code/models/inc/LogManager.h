#pragma once

#include <QMutex>
#include <QObject>
#include <QString>

// 线程安全的日志文件，同时用于承接 Qt 自身的警告输出
class LogManager : public QObject
{
    Q_OBJECT

public:
    static LogManager* instance();

    QString filePath() const;
    void log(const QString& message);

    static void installMessageHandler();

private:
    explicit LogManager(QObject* parent = nullptr);

    static QString resolveFilePath();

    QString m_filePath;
    mutable QMutex m_mutex;
};
