#pragma once

#include <QString>
#include <QVariantList>
#include <QVector>

class QJsonObject;

// 一条 ssh 端口转发规则: 本地端口 -> 目标地址:目标端口
struct ForwardData
{
    QString localPort;
    QString destIP;
    QString destPort;

    bool isEmpty() const;
    bool isValid() const;
    bool operator==(const ForwardData& other) const;
    bool operator!=(const ForwardData& other) const;

    QJsonObject toJson() const;
    static ForwardData fromJson(const QJsonObject& json);

    QVariantMap toVariantMap() const;
    static ForwardData fromVariantMap(const QVariantMap& map);

    static QVariantList toVariantList(const QVector<ForwardData>& forwards);
    static QVector<ForwardData> fromVariantList(const QVariantList& forwards);

    static bool isValidPort(const QString& port);
};
