#pragma once

#include <QMainWindow>

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

private:
    void onAddTunnel();
    void loadTunnelData();
    void saveTunnelData();

private:
    Ui::MainWindow* ui;
};
