#include "status_window.h"
#include "ui_status_window.h"

#include "generator.h"
#include "photodetector.h"
#include "laser.h"

#include <QDebug>

using namespace presentation;

class StatusWindow::Impl
{
public:
    domain::Generator* generator;
    domain::Laser* laser;
    domain::Photodetector* photodetector;
};

StatusWindow::StatusWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusWindow), d(new Impl)
{
    ui->setupUi(this);
    d->generator = new domain::Generator(this);
    d->laser = new domain::Laser(this);
    d->photodetector = new domain::Photodetector(this);

    connect(d->generator, &domain::Generator::generatorUpdate, this, [this](int chId, quint32 value, bool isOn, dto::Channel::ChannelStatus status)
    {
        if (chId == 0)
        {
            ui->ch1Status->setText(isOn?"ON":"OFF");
            ui->ch1Label->setText(QString::number(value));
//            if (status == dto::Channel::Completed)
//                ui->ch1Label->setStyleSheet("QLabel { background-color : green; color : white; }");
//            else if (status == dto::Channel::InProgress)
//                ui->ch1Label->setStyleSheet("QLabel { background-color : yellow; color : black; }");
//            else if (status == dto::Channel::Rejected)
//                ui->ch1Label->setStyleSheet("QLabel { background-color : red; color : white; }");
        }
        else if (chId == 1)
        {
            ui->ch2Status->setText(isOn?"ON":"OFF");
            ui->ch2Label->setText(QString::number(value));
        }
    });

    connect(d->photodetector, &domain::Photodetector::biasUpdated, this, [this](qreal bias, bool isOn, dto::Channel::ChannelStatus status)
    {
        Q_UNUSED(isOn);
        ui->uBiasLabel->setText(QString::number(bias));
    });
    connect(d->photodetector, &domain::Photodetector::dcdcSwitchUpdated, this, [this](bool isOn, dto::Channel::ChannelStatus status)
    {
        ui->dcdcStatus->setText(isOn?"ON":"OFF");
    });
    connect(d->photodetector, &domain::Photodetector::gainUpdated, this, [this](int gain, bool isOn, dto::Channel::ChannelStatus status)
    {
        Q_UNUSED(isOn);
        ui->kLabel->setText(QString::number(gain));
    });

    connect(d->laser, &domain::Laser::laserUpdate, this, [this](qreal value, dto::Channel::ChannelStatus status)
    {
        ui->laserPLable->setText(QString::number(value));
    });
}

StatusWindow::~StatusWindow()
{
    delete ui;
}
