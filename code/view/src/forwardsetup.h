#pragma once

#include <QWidget>

namespace Ui
{
class ForwardSetup;
}

struct ForwardData;

class ForwardSetup : public QWidget
{
    Q_OBJECT

public:
    explicit ForwardSetup(QWidget* parent = nullptr);
    ~ForwardSetup();

public:
    void initForwardData(const ForwardData& forwardData);
    const ForwardData getForwardData();

private:
    Ui::ForwardSetup* ui;
};
