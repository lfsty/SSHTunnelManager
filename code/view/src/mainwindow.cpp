#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QCloseEvent>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>

#include "tunnelview.h"

#include <QDebug>

namespace
{
constexpr const char* SAVED_FILE_PATH = "./tunnel.json";

constexpr const char* LAST_SAVE_TIME_KEY  = "savedTime";
constexpr const char* SSH_TUNNEL_DATA_KEY = "sshTunnelData";
}  // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->addTunnelButton->setText(tr("addTunnel"));
    connect(ui->addTunnelButton, &QPushButton::clicked, this, &MainWindow::onAddTunnel);
    loadTunnelData();
    initTray();
}

MainWindow::~MainWindow()
{
    saveTunnelData();

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    this->hide();
    event->ignore();
}

void MainWindow::onAddTunnel()
{
    ui->tunnelViewLayout->addWidget(new TunnelView(this));
}

void MainWindow::loadTunnelData()
{
    QFile file(SAVED_FILE_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject rootObj = doc.object();
    if (rootObj.empty() || !rootObj.contains(SSH_TUNNEL_DATA_KEY))
    {
        return;
    }

    QJsonArray sshTunnelData = rootObj[SSH_TUNNEL_DATA_KEY].toArray();
    for (const auto& tunnelData : sshTunnelData)
    {
        QJsonObject tunnelObj  = tunnelData.toObject();
        TunnelView* tunnelView = new TunnelView(this);
        tunnelView->load(tunnelObj);
        ui->tunnelViewLayout->addWidget(tunnelView);
    }
}

void MainWindow::saveTunnelData()
{
    QJsonObject rootObj;
    rootObj[LAST_SAVE_TIME_KEY] = QDateTime::currentDateTime().toString();

    QJsonArray sshTunnelData;

    for (int i = 0; i < ui->tunnelViewLayout->count(); i++)
    {
        QLayoutItem* item = ui->tunnelViewLayout->itemAt(i);
        if (!item)
        {
            continue;
        }

        if (QWidget* widget = item->widget())
        {
            TunnelView* tunnelView = qobject_cast<TunnelView*>(widget);
            if (tunnelView)
            {
                sshTunnelData.append(tunnelView->save());
            }
        }
    }
    rootObj[SSH_TUNNEL_DATA_KEY] = sshTunnelData;

    QJsonDocument doc(rootObj);
    QFile file(SAVED_FILE_PATH);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(doc.toJson(QJsonDocument::Indented));
    }
    file.close();
}

void MainWindow::initTray()
{
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/img/icon.png"));

    QMenu* trayMenu     = new QMenu(this);
    QAction* quitAction = new QAction(tr("quit"), this);
    trayMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayMenu);

    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    connect(trayIcon, &QSystemTrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason)
            {
                if (reason == QSystemTrayIcon::DoubleClick)
                {
                    this->showNormal();
                    this->activateWindow();
                }
            });

    trayIcon->show();
}
