#pragma once

#include <QDialog>
#include <QSet>

namespace Ui
{
class TunnelSetupDialog;
}

struct TunnelServerData;
struct ForwardData;

class TunnelSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TunnelSetupDialog(QWidget* parent = nullptr);
    ~TunnelSetupDialog();

public:
    const TunnelServerData getTunnelServerData();
    void initTunnelServerData(const TunnelServerData& serverData);

    const QVector<ForwardData> getForwardData();
    void initForwardData(const QVector<ForwardData>& forwardData);

private:
    void onAddForward();

private:
    Ui::TunnelSetupDialog* ui;
};
