#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QLocalServer>
#include <QLocalSocket>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#include "LogManager.h"
#include "Tunnel.h"
#include "TunnelListModel.h"
#include "TunnelManager.h"

namespace
{
constexpr const char* INSTANCE_SERVER_NAME = "com.SSHTunnelManager.Ciel";
constexpr const char* SHOW_WINDOW_COMMAND  = "SHOW_WND";

#ifdef QT_NO_DEBUG
constexpr bool CLOSE_TO_TRAY = true;
#else
constexpr bool CLOSE_TO_TRAY = false;
#endif

void registerQmlTypes()
{
    qmlRegisterUncreatableType<Tunnel>("SSHTunnelManager.Core", 1, 0, "Tunnel", QStringLiteral("Tunnel objects are created by TunnelManager"));
    qmlRegisterUncreatableType<TunnelListModel>("SSHTunnelManager.Core", 1, 0, "TunnelListModel", QStringLiteral("TunnelListModel is provided by TunnelManager"));
    qmlRegisterType<TunnelManager>("SSHTunnelManager.Core", 1, 0, "TunnelManager");
}

// 已有实例在运行时，通知它显示主窗口后立即退出
bool notifyRunningInstance()
{
    QLocalSocket socket;
    socket.connectToServer(INSTANCE_SERVER_NAME);
    if (!socket.waitForConnected(50))
    {
        return false;
    }

    socket.write(SHOW_WINDOW_COMMAND);
    socket.waitForBytesWritten(50);
    socket.disconnectFromServer();
    return true;
}

void handleInstanceActivation(QLocalServer& server, QObject* rootWindow)
{
    QObject::connect(&server, &QLocalServer::newConnection, rootWindow, [&server, rootWindow]()
                     {
                         while (QLocalSocket* client = server.nextPendingConnection())
                         {
                             QObject::connect(client, &QLocalSocket::readyRead, client, [client, rootWindow]()
                                              {
                                                  if (client->readAll().startsWith(SHOW_WINDOW_COMMAND))
                                                  {
                                                      QMetaObject::invokeMethod(rootWindow, "showWindow", Qt::QueuedConnection);
                                                  }
                                                  client->deleteLater();
                                              });
                             QObject::connect(client, &QLocalSocket::disconnected, client, &QObject::deleteLater);
                         }
                     });
}
}  // namespace

int main(int argc, char* argv[])
{
    // 所有数据都留在工作目录内，禁用 Qt 的全局磁盘缓存
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    qputenv("QSG_RHI_DISABLE_SHADER_DISK_CACHE", "1");

    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("Ciel"));
    QApplication::setApplicationName(QStringLiteral("SSHTunnelManager"));
    QApplication::setApplicationVersion(QStringLiteral("0.3.0"));
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/img/icon.png")));

    LogManager::installMessageHandler();
    registerQmlTypes();

    // Basic 样式允许完全自定义控件外观
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    if (notifyRunningInstance())
    {
        return EXIT_SUCCESS;
    }

    QLocalServer instanceServer;
    QLocalServer::removeServer(INSTANCE_SERVER_NAME);
    if (!instanceServer.listen(INSTANCE_SERVER_NAME))
    {
        qWarning() << "无法监听本地 socket，单实例功能不可用:" << instanceServer.errorString();
    }

    QQmlApplicationEngine engine;
    engine.setInitialProperties({ { QStringLiteral("closeToTray"), CLOSE_TO_TRAY } });

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        []()
        {
            QCoreApplication::exit(EXIT_FAILURE);
        },
        Qt::QueuedConnection);

    engine.loadFromModule("SSHTunnelManager", "Main");
    if (engine.rootObjects().isEmpty())
    {
        return EXIT_FAILURE;
    }

    handleInstanceActivation(instanceServer, engine.rootObjects().constFirst());

    return app.exec();
}
