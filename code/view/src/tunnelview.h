#pragma once

#include <QString>
#include <QWidget>

namespace Ui
{
class TunnelView;
}

class Tunnel;
class QJsonObject;

class TunnelView : public QWidget
{
    Q_OBJECT

public:
    explicit TunnelView(QWidget* parent = nullptr);
    ~TunnelView();

    const QJsonObject save();
    void load(const QJsonObject& tunnelDataObj);

private:
    void updateViewInfo();
    void onSetupTunnel();
    void onTunnelStatusChanged(bool running);

private:
    Ui::TunnelView* ui;

    Tunnel* m_tunnel;
};
