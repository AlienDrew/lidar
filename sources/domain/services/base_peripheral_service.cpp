#include "base_peripheral_service.h"

#include "channel.h"

using namespace domain;

class BasePeripheralService::Impl
{
public:
    dto::ChannelPtr dumpChannel;
};

BasePeripheralService::BasePeripheralService(int channelCount, QObject* parent) : QObject(parent),
    d(new Impl)
{
    for (int i = 0; i<channelCount; ++i)
    {
        dto::ChannelPtr ch(new dto::Channel);
        ch->setChannelId(i);
        channelList.append(ch);
    }
    d->dumpChannel = dto::ChannelPtr(new dto::Channel);
}

BasePeripheralService::~BasePeripheralService()
{
    channelList.clear();
}

dto::ChannelPtr BasePeripheralService::load(int chId)
{
    return channelList.value(chId, nullptr);
}

dto::ChannelPtr BasePeripheralService::loadDumpChannel() const
{
    if (!d->dumpChannel.isNull())
        return d->dumpChannel;
    return nullptr;
}

void BasePeripheralService::rollbackChannel()
{
    //this->blockSignals(true);
    dto::ChannelPtr channel = this->loadDumpChannel();
    channel->setStatus(dto::Channel::Rejected);
    this->updateChannel(channel, false);
    //this->blockSignals(false);
    emit chRollback();
}

void BasePeripheralService::updateChannel(int chId, quint32 val, bool on, bool toMCU, dto::Channel::ChannelStatus status)
{
    if (chId<0)
        return;
    dto::ChannelPtr channel = this->load(chId);
    if (channel.isNull())
    {
        qFatal("error in %s: no such channel", Q_FUNC_INFO);
        return;
    }
    d->dumpChannel->setChannelId(channel->channelId());
    d->dumpChannel->setOn(channel->isOn());
    d->dumpChannel->setValue(channel->value());

    channel->setValue(val);
    channel->setOn(on);
    channel->setStatus(status);
    channelList[chId] = channel;
    emit chUpdated(channel, toMCU);
}

void BasePeripheralService::updateChannel(dto::ChannelPtr channel, bool toMCU)
{
//    if (!channelList.contains(channel))
//    {
//        qFatal("error in %s: no such channel", Q_FUNC_INFO);
//        return;
//    }
//    d->dumpChannel->setChannelId(channel->channelId());
//    d->dumpChannel->setOn(channel->isOn());
//    d->dumpChannel->setValue(channel->value());

//    channelList[channel->channelId()] = channel;
//    emit chUpdated(channel, toMCU);
    this->updateChannel(channel->channelId(), channel->value(), channel->isOn(), toMCU, channel->status());
}
