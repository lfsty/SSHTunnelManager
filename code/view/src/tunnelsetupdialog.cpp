#include "tunnelsetupdialog.h"
#include "ui_tunnelsetupdialog.h"

#include <tunnel.h>
#include "forwardsetup.h"

TunnelSetupDialog::TunnelSetupDialog(QWidget* parent) : QDialog(parent),
                                                        ui(new Ui::TunnelSetupDialog)
{
    ui->setupUi(this);

    connect(ui->addForwardButton, &QPushButton::clicked, this, &TunnelSetupDialog::onAddForward);
}

TunnelSetupDialog::~TunnelSetupDialog()
{
    delete ui;
}

const TunnelServerData TunnelSetupDialog::getTunnelServerData()
{
    TunnelServerData serverData;

    serverData.userName = ui->userNameEdit->text();
    serverData.ip       = ui->serverIPEdit->text();
    serverData.port     = ui->serverPortEdit->text();

    return serverData;
}

void TunnelSetupDialog::initTunnelServerData(const TunnelServerData& serverData)
{
    ui->userNameEdit->setText(serverData.userName);
    ui->serverIPEdit->setText(serverData.ip);
    ui->serverPortEdit->setText(serverData.port);
}

const QVector<ForwardData> TunnelSetupDialog::getForwardData()
{
    QVector<ForwardData> forwardDataSet;

    for (int i = 0; i < ui->forwardSetupLayout->count(); i++)
    {
        QLayoutItem* item = ui->forwardSetupLayout->itemAt(i);
        if (!item)
        {
            continue;
        }

        if (QWidget* widget = item->widget())
        {
            ForwardSetup* forwardSetupView = qobject_cast<ForwardSetup*>(widget);
            if (forwardSetupView)
            {
                forwardDataSet.push_back(forwardSetupView->getForwardData());
            }
        }
    }

    return forwardDataSet;
}

void TunnelSetupDialog::initForwardData(const QVector<ForwardData>& forwardData)
{
    for (const auto& data : forwardData)
    {
        ForwardSetup* forwardSetupView = new ForwardSetup(this);
        forwardSetupView->initForwardData(data);
        ui->forwardSetupLayout->addWidget(forwardSetupView);
    }
}

void TunnelSetupDialog::onAddForward()
{
    ui->forwardSetupLayout->addWidget(new ForwardSetup(this));
}
