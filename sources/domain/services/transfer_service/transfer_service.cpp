#include "transfer_service.h"

#include "channel.h"
#include "usb.h"

#include <QDataStream>
#include <QVector>
#include <QDebug>
#include <QIODevice>

using namespace domain;

class TransferService::Impl
{
public:
    communication::USB* usb = nullptr;
};

TransferService::TransferService() : d(new Impl)
{
    d->usb = new communication::USB();
    //d->usb->initialize();
    //d->usb->open();
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
    return d->usb->isOpened();
}

void TransferService::closeDevice()
{
    d->usb->close();
}

bool TransferService::deviceOpened() const
{
    return d->usb->isOpened();
}

void TransferService::transferCommand(TransferService::Command cmd)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream<<(uint8_t)cmd;
    d->usb->bulkWriteTransfer(data);
}

void TransferService::transferChannel(Command cmd, dto::ChannelPtr channel)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream<<(uint8_t)cmd;
    stream<<(uint8_t)channel->channelId();
    uint32_t f = channel->value();
    data.append(reinterpret_cast<const char*>(&f), sizeof(f)); //4 bytes
    //d->communicationInterface->write(data);
    d->usb->bulkWriteTransfer(data);
}

void TransferService::getAdcData(QIODevice* device)
{
    d->usb->asyncBulkReadTransfer();
    connect(d->usb, &communication::USB::readyRead, this, [=](QByteArray data)
    {
        device->write(data);
    });
}
