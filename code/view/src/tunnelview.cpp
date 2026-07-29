#include "tunnelview.h"
#include "ui_tunnelview.h"

#include <QJsonObject>
#include <QMessageBox>

#include <tunnel.h>
#include "tunnelsetupdialog.h"

#include <QDebug>

TunnelView::TunnelView(QWidget* parent) : QWidget(parent),
                                          ui(new Ui::TunnelView),
                                          m_tunnel(new Tunnel)
{
    ui->setupUi(this);

    connect(ui->deleteButton, &QPushButton::clicked, this, &TunnelView::deleteLater);
    updateViewInfo();

    ui->managerButton->setText(tr("Setup"));
    connect(ui->managerButton, &QPushButton::clicked, this, &TunnelView::onSetupTunnel);

    connect(ui->startTunnelButton, &QPushButton::clicked, m_tunnel, &Tunnel::requestToggleTunnelStatus);
    connect(m_tunnel, &Tunnel::tunnelStatusChanged, this, &TunnelView::onTunnelStatusChanged);
    connect(m_tunnel, &Tunnel::tunnelError, this, [=](const QString& errMsg)
            {
                QMessageBox::critical(this, tr("Tunnel Error"), errMsg);
            });

    onTunnelStatusChanged(false);
}

TunnelView::~TunnelView()
{
    delete ui;
    delete m_tunnel;
}

const QJsonObject TunnelView::save()
{
    return m_tunnel->save();
}

void TunnelView::load(const QJsonObject& tunnelDataObj)
{
    m_tunnel->load(tunnelDataObj);
    updateViewInfo();
}

void TunnelView::updateViewInfo()
{
    const TunnelServerData& serverData = m_tunnel->getTunnelServerData();
    ui->userNameLabel->setText(serverData.userName);
    ui->serverIPLabel->setText(serverData.ip);
    ui->serverPortLabel->setText(serverData.port);
}

void TunnelView::onSetupTunnel()
{
    TunnelSetupDialog setupDialog;

    setupDialog.initTunnelServerData(m_tunnel->getTunnelServerData());
    setupDialog.initForwardData(m_tunnel->getForwardData());

    setupDialog.setModal(true);
    setupDialog.show();
    if (setupDialog.exec() != QDialog::Accepted)
    {
        return;
    }

    m_tunnel->setTunnelServerData(setupDialog.getTunnelServerData());
    m_tunnel->setForwardData(setupDialog.getForwardData());
    updateViewInfo();
}

void TunnelView::onTunnelStatusChanged(bool running)
{
    ui->deleteButton->setEnabled(!running);
    ui->managerButton->setEnabled(!running);

    ui->startTunnelButton->setStyleSheet(QString("QPushButton {background-color: %1;}").arg(running ? "green" : "red"));
}
