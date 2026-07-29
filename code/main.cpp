#include <QApplication>

#ifdef RELEASE
#include <QLocalServer>
#include <QLocalSocket>
#endif

#include <mainwindow.h>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/img/icon.png"));

#ifdef RELEASE
    QString serverName = "com.SSHTunnelManager.Ciel";

    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(50))
    {
        socket.write("SHOW_WND");
        socket.waitForBytesWritten(50);
        return 0;
    }

    QLocalServer server;
    // 如果上次程序异常崩溃可能残留未释放的文件，先清理
    QLocalServer::removeServer(serverName);
    if (!server.listen(serverName))
    {
        return -1;
    }
#endif

    MainWindow w;
    w.show();

#ifndef DEBUG
    QObject::connect(&server, &QLocalServer::newConnection, [&]()
                     {
                         QLocalSocket* clientSocket = server.nextPendingConnection();
                         QObject::connect(clientSocket, &QLocalSocket::readyRead, [&, clientSocket]()
                                          {
                                              if (clientSocket->readAll() == "SHOW_WND")
                                              {
                                                  w.showNormal();
                                                  w.raise();
                                                  w.activateWindow();
                                              }
                                              clientSocket->deleteLater();
                                          });
                     });
#endif

    return a.exec();
}