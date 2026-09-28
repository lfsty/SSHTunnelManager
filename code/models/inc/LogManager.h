#pragma once

#include <QObject>

class QFile;

class LogManager : public QObject
{
    Q_OBJECT

public:
    static LogManager* getInstance();

public:
    void log(const QString& log);

private:
    LogManager();
    ~LogManager();

private:
    QFile* m_file = nullptr;
};