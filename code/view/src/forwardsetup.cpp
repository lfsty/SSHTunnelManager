#include "forwardsetup.h"
#include "ui_forwardsetup.h"

#include <tunnel.h>

ForwardSetup::ForwardSetup(QWidget* parent) : QWidget(parent),
                                              ui(new Ui::ForwardSetup)
{
    ui->setupUi(this);

    connect(ui->deleteButton, &QPushButton::clicked, this, &ForwardSetup::deleteLater);
}

ForwardSetup::~ForwardSetup()
{
    delete ui;
}

void ForwardSetup::initForwardData(const ForwardData& forwardData)
{
    ui->localPortEdit->setText(forwardData.localPort);
    ui->destIPEdit->setText(forwardData.destIP);
    ui->destPortEdit->setText(forwardData.destPort);
}

const ForwardData ForwardSetup::getForwardData()
{
    ForwardData forwardData;

    forwardData.localPort = ui->localPortEdit->text();
    forwardData.destIP    = ui->destIPEdit->text();
    forwardData.destPort  = ui->destPortEdit->text();

    return forwardData;
}
