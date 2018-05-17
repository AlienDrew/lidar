#include "transfer_service.h"

#include "channel.h"
#include "usb.h"
#include "data_parser.h"

#include <QDataStream>
#include <QVector>
#include <QDebug>
#include <QIODevice>

using namespace domain;

class TransferService::Impl
{
public:
    communication::USB* usb = nullptr;
    utils::DataParser* dataParser = nullptr;
};

TransferService::TransferService() : d(new Impl)
{
    d->usb = new communication::USB();
    connect(d->usb, &communication::USB::deviceDisconnected, this, &TransferService::closeDevice); //hotPlug disconnect
    connect(d->usb, &communication::USB::deviceDisconnected, this, &TransferService::hotPlugDeviceLeft); //hotPlug disconnect
    //d->usb->initialize();
    //d->usb->open();
    d->dataParser = new utils::DataParser(this);
}

TransferService::~TransferService()
{
    d->usb->close();
    delete d->usb;
}

bool TransferService::openDevice()
{
    d->usb->initialize();
    d->usb->open();
    transferCommand(domain::TransferService::usb_connected);
    listenData();
    return d->usb->isOpened();
}

void TransferService::closeDevice()
{
    transferCommand(domain::TransferService::usb_disconnected);
    d->usb->close();
}

bool TransferService::deviceOpened() const
{
    return d->usb->isOpened();
}

bool TransferService::transferCommand(TransferService::Command cmd)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream<<(uint8_t)cmd;
    return d->usb->bulkWriteTransfer(data);
}

bool TransferService::transferCommand(TransferService::Command cmd, uint8_t value)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream<<(uint8_t)cmd;
    stream<<(uint8_t)value;
    return d->usb->bulkWriteTransfer(data);
}

bool TransferService::transferChannel(Command cmd, dto::ChannelPtr channel)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream<<(uint8_t)cmd;
    stream<<(uint8_t)channel->channelId();
    uint32_t f = channel->value();
    data.append(reinterpret_cast<const char*>(&f), sizeof(f)); //4 bytes
    //d->communicationInterface->write(data);
    return d->usb->bulkWriteTransfer(data);
}

void TransferService::getAdcData(QIODevice* device)
{
    d->usb->asyncBulkReadTransfer();
    connect(d->usb, &communication::USB::readyRead, this, [=](QByteArray data)
    {
        device->write(data);
    });
}

void TransferService::listenData()
{
    d->usb->asyncBulkReadTransfer();
    connect(d->usb, &communication::USB::readyRead, d->dataParser, &utils::DataParser::parse);
}
