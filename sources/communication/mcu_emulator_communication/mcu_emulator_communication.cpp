#include "mcu_emulator_communication.h"

#include "mcu_emulator.h"
#include "command.h"

#include <QTimer>
#include <QDebug>

using namespace communication;

class MCUEmulatorCommunication::Impl
{
public:
    domain::MCUEmulator* mcuEmulator;
    QTimer* ADCTimer;
    QTimer* tempTimer;

    bool canRead = false;
};

MCUEmulatorCommunication::MCUEmulatorCommunication() :
    d(new Impl)
{
    d->mcuEmulator = new domain::MCUEmulator(this);
    d->ADCTimer = new QTimer(this);
    d->tempTimer = new QTimer(this);

    connect(d->mcuEmulator, &domain::MCUEmulator::readyRead, this, &MCUEmulatorCommunication::readyRead);

    //temperature
    connect(d->tempTimer, &QTimer::timeout, d->mcuEmulator, &domain::MCUEmulator::sendTemperature);

    //ADCData
    connect(d->ADCTimer, &QTimer::timeout, d->mcuEmulator, &domain::MCUEmulator::sendADCData);
    connect(d->mcuEmulator, &domain::MCUEmulator::finishedRead, d->ADCTimer, &QTimer::stop);
}

MCUEmulatorCommunication::~MCUEmulatorCommunication()
{

}

bool MCUEmulatorCommunication::open()
{
    qDebug()<<"opening emulator..";
    m_isOpened = true;
    return m_isOpened;
}

bool MCUEmulatorCommunication::isOpen()
{
    return m_isOpened;
}

void MCUEmulatorCommunication::close()
{
    d->ADCTimer->stop();
    d->tempTimer->stop();
    d->canRead = false;
    m_isOpened = false;
}

void MCUEmulatorCommunication::read()
{
    if(!m_isOpened)
        return;
    d->canRead = true;
    d->tempTimer->setInterval(1000);
    d->tempTimer->start();
}

void MCUEmulatorCommunication::cancelRead()
{
    d->canRead = false;
    d->tempTimer->stop();
}

bool MCUEmulatorCommunication::write(QByteArray data)
{
    if (!m_isOpened)
        return false;
    uint8_t command = data.at(0);
    switch (command)
    {
    case dto::Command::gen:
        qDebug()<<data;
        break;
    case dto::Command::startMeasure:
        d->ADCTimer->setInterval(1);
        d->ADCTimer->start();
        break;
    case dto::Command::usb_connected:
        d->mcuEmulator->sendPeriphStatus();
        break;
    default:
        break;
    }
    return true;
}
