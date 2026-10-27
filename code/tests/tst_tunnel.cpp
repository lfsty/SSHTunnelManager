#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtTest>

#include "ForwardData.h"
#include "SshOutput.h"
#include "Tunnel.h"
#include "TunnelListModel.h"

class TunnelTest : public QObject
{
    Q_OBJECT

private slots:
    void forwardDataValidation();
    void jsonRoundTrip();
    void legacyDataCompatibility();
    void listModelAddRemove();
    void configurationState();
    void sshOutputDecoding();
};

void TunnelTest::forwardDataValidation()
{
    ForwardData forward;
    QVERIFY(forward.isEmpty());
    QVERIFY(!forward.isValid());

    forward.localPort = "8080";
    forward.destIP    = "10.0.0.1";
    forward.destPort  = "80";
    QVERIFY(!forward.isEmpty());
    QVERIFY(forward.isValid());

    forward.destPort = "70000";
    QVERIFY(!forward.isValid());

    QVERIFY(!ForwardData::isValidPort("0"));
    QVERIFY(!ForwardData::isValidPort("65536"));
    QVERIFY(!ForwardData::isValidPort("abc"));
    QVERIFY(ForwardData::isValidPort("1"));
    QVERIFY(ForwardData::isValidPort("65535"));
}

void TunnelTest::jsonRoundTrip()
{
    const QVariantList forwards = {
        QVariantMap{ { "localPort", "8080" }, { "destIP", "10.0.0.1" }, { "destPort", "80" } },
        QVariantMap{ { "localPort", "8443" }, { "destIP", "git.example.com" }, { "destPort", "443" } },
    };

    Tunnel source;
    source.applyConfiguration("root", "1.2.3.4", "22", forwards);
    QVERIFY(source.isConfigured());

    const QJsonObject json = source.toJson();
    QCOMPARE(json.value("jumpserverUserName").toString(), QString("root"));
    QCOMPARE(json.value("jumpserverIP").toString(), QString("1.2.3.4"));
    QCOMPARE(json.value("jumpserverPort").toString(), QString("22"));
    QCOMPARE(json.value("forwardData").toArray().size(), 2);

    Tunnel restored;
    restored.loadFromJson(json);
    QCOMPARE(restored.userName(), source.userName());
    QCOMPARE(restored.serverIp(), source.serverIp());
    QCOMPARE(restored.serverPort(), source.serverPort());
    QCOMPARE(restored.forwards(), source.forwards());
    QVERIFY(restored.isConfigured());
}

void TunnelTest::legacyDataCompatibility()
{
    const QByteArray legacy = R"({
        "savedTime": "Mon Sep 28 09:38:23 2026",
        "sshTunnelData": [
            {
                "jumpserverUserName": "ciel",
                "jumpserverIP": "jump.example.com",
                "jumpserverPort": "2222",
                "forwardData": [
                    { "localPort": "15432", "destIP": "db.internal", "destPort": "5432" }
                ]
            }
        ]
    })";

    const QJsonObject root = QJsonDocument::fromJson(legacy).object();

    TunnelListModel model;
    model.loadFromJson(root.value("sshTunnelData").toArray());

    QCOMPARE(model.rowCount(), 1);
    Tunnel* tunnel = model.tunnelAt(0);
    QVERIFY(tunnel);
    QCOMPARE(tunnel->userName(), QString("ciel"));
    QCOMPARE(tunnel->serverIp(), QString("jump.example.com"));
    QCOMPARE(tunnel->serverPort(), QString("2222"));
    QVERIFY(tunnel->isConfigured());

    const QVariantMap forward = tunnel->forwards().constFirst().toMap();
    QCOMPARE(forward.value("localPort").toString(), QString("15432"));
    QCOMPARE(forward.value("destIP").toString(), QString("db.internal"));
    QCOMPARE(forward.value("destPort").toString(), QString("5432"));

    // 保存后仍应保持旧版字段名，便于回滚
    QCOMPARE(tunnel->toJson(), root.value("sshTunnelData").toArray().at(0).toObject());
}

void TunnelTest::listModelAddRemove()
{
    TunnelListModel model;
    QSignalSpy countSpy(&model, &TunnelListModel::countChanged);

    Tunnel* first = model.addTunnel();
    QVERIFY(first);
    Tunnel* second = model.addTunnel();
    QVERIFY(second);
    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(countSpy.count(), 2);

    const QModelIndex index = model.index(1, 0);
    QVERIFY(index.isValid());
    QCOMPARE(index.data(TunnelListModel::TunnelRole).value<Tunnel*>(), second);
    QCOMPARE(model.indexOf(second), 1);

    model.removeTunnel(0);
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.indexOf(first), -1);
    QCOMPARE(model.indexOf(second), 0);

    model.removeTunnel(-1);
    model.removeTunnel(99);
    QCOMPARE(model.rowCount(), 1);
}

void TunnelTest::configurationState()
{
    Tunnel tunnel;
    QVERIFY(!tunnel.isConfigured());
    QVERIFY(tunnel.title().isEmpty());

    tunnel.setUserName("ciel");
    QVERIFY(!tunnel.isConfigured());

    const QVariantList forwards = {
        QVariantMap{ { "localPort", "8080" }, { "destIP", "10.0.0.1" }, { "destPort", "80" } },
    };
    tunnel.applyConfiguration("ciel", "jump.example.com", "22", forwards);
    QVERIFY(tunnel.isConfigured());
    QCOMPARE(tunnel.title(), QString("ciel@jump.example.com:22"));

    tunnel.applyConfiguration("ciel", "jump.example.com", "not-a-port", forwards);
    QVERIFY(!tunnel.isConfigured());

    tunnel.applyConfiguration("ciel", "jump.example.com", "22", {});
    QVERIFY(!tunnel.isConfigured());
}

void TunnelTest::sshOutputDecoding()
{
    const QByteArray plain = QByteArrayLiteral("Permission denied (publickey).");
    QCOMPARE(SshOutput::unescapeOctal(plain), plain);
    QCOMPARE(SshOutput::decode(plain), QStringLiteral("Permission denied (publickey)."));

    // Windows OpenSSH 会把 GBK 字节输出成 \ooo 转义
    const QByteArray escaped       = QByteArrayLiteral("ssh: Could not resolve hostname asdf.top: \\262\\273\\326\\252\\265\\300\\325\\342\\321\\371\\265\\304\\326\\367\\273\\372\\241\\243");
    const QByteArray expectedBytes = QByteArrayLiteral("ssh: Could not resolve hostname asdf.top: ") + QByteArray::fromHex("b2bbd6aab5c0d5e2d1f9b5c4d6f7bbfaa1a3");
    QCOMPARE(SshOutput::unescapeOctal(escaped), expectedBytes);

    const QString decoded = SshOutput::decode(escaped);
    QVERIFY(decoded.startsWith(QStringLiteral("ssh: Could not resolve hostname asdf.top: ")));
    QVERIFY(!decoded.contains(QLatin1Char('\\')));
}

QTEST_GUILESS_MAIN(TunnelTest)

#include "tst_tunnel.moc"
