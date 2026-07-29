#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void loadTunnelData();
    void saveTunnelData();
    void initTray();

private slots:
    void onAddTunnel();

private:
    Ui::MainWindow* ui;
};
