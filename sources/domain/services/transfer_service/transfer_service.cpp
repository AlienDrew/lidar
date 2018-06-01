#include "transfer_service.h"

#include "channel.h"
#include "command.h"
#include "communication_interface.h"
#include "mcu_emulator_communication.h"
#include "usb.h"
#include "data_parser.h"
#include "settings_provider.h"

#include <QDataStream>
#include <QVector>
#include <QDebug>
#include <QIODevice>

using namespace domain;

class TransferService::Impl
{
public:
    communication::MCUEmulatorCommunication* emulator = nullptr;
    communication::USB* usb = nullptr;
    utils::DataParser* dataParser = nullptr;
};

TransferService::TransferService() : d(new Impl)
{
    d->usb = new communication::USB();
    d->emulator = new communication::MCUEmulatorCommunication();
    connect(d->usb, &communication::USB::deviceDisconnected, this, &TransferService::closeDevice); //hotPlug disconnect
    connect(d->usb, &communication::USB::deviceDisconnected, this, &TransferService::hotPlugDeviceLeft); //hotPlug disconnect
    //d->usb->initialize();
    //d->usb->open();
    d->dataParser = new utils::DataParser(this);
}

TransferService::~TransferService()
{
    d->usb->close();
    d->emulator->close();
    delete d->usb;
    delete d->emulator;
}

bool TransferService::openDevice()
{
    d->usb->initialize();
    d->usb->open();
    if (!d->usb->isOpened())
        d->emulator->open();
    dto::Command cmd;
    cmd.setType(dto::Command::usb_connected);
    transferCommand(cmd);
    listenData();
    return d->usb->isOpened() || d->emulator->isOpen();
}

void TransferService::closeDevice()
{
    dto::Command cmd;
    cmd.setType(dto::Command::usb_disconnected);
    transferCommand(cmd);
    d->usb->close();
    d->emulator->close();
}

bool TransferService::deviceOpened() const
{
    if (d->usb->isOpened())
        return d->usb->isOpened();
    if (d->emulator->isOpen())
        return d->emulator->isOpen();
    return false;
}

bool TransferService::transferCommand(const dto::Command& command)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream<<(uint8_t)command.type();
    if (command.hasArguments())
    {
        for (const QVariant& argument : command.arguments())
        {
            if (argument.isValid() && argument.canConvert<dto::Channel*>())
            {
                stream<<(uint8_t)argument.value<dto::Channel*>()->channelId();
                uint32_t f = argument.value<dto::Channel*>()->value();
                data.append(reinterpret_cast<const char*>(&f), sizeof(f)); //4 bytes
            }
            else
                stream<<(uint8_t)argument.toUInt();
        }
    }
    if (d->usb->isOpened())
        return d->usb->bulkWriteTransfer(data);
    if (d->emulator->isOpen())
        return d->emulator->write(data);
    return false;
}

bool TransferService::transferChannel(const dto::Command& command, dto::ChannelPtr channel)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_10);
    stream<<(uint8_t)command.type();
    stream<<(uint8_t)channel->channelId();
    uint32_t f = channel->value();
    data.append(reinterpret_cast<const char*>(&f), sizeof(f)); //4 bytes
    //d->communicationInterface->write(data);
    if (d->usb->isOpened())
        return d->usb->bulkWriteTransfer(data);
    if (d->emulator->isOpen())
        return d->emulator->write(data);
    return false;
}

//void TransferService::getAdcData(QIODevice* device)
//{
//    if(d->usb->isOpened())
//    {
//        d->usb->asyncBulkReadTransfer();
//        connect(d->usb, &communication::USB::readyRead, this, [=](QByteArray data)
//        {
//            if (device->isOpen())
//                device->write(data);
//        });
//    }
//    else if (d->emulator->isOpen())
//    {
//        connect(d->emulator, &communication::CommunicationInterface::readyRead, this, [=](QByteArray data)
//        {
//            if (device->isOpen())
//                device->write(data);
//        });
//        d->emulator->read();
//    }
//}

void TransferService::setADCXYIODevice(QIODevice* device)
{
    d->dataParser->setXYIODevice(device);
}

void TransferService::listenData()
{
    if (d->usb->isOpened())
    {
        connect(d->usb, &communication::USB::readyRead, d->dataParser, &utils::DataParser::parse);
        d->usb->asyncBulkReadTransfer();
    }
    else if (d->emulator->isOpen())
    {
        connect(d->emulator, &communication::CommunicationInterface::readyRead, d->dataParser, &utils::DataParser::parse);
        d->emulator->read();
    }
}

void TransferService::cancelListen()
{
    d->usb->cancelAsynchBulkReadTransfer();
    d->emulator->cancelRead();
}
