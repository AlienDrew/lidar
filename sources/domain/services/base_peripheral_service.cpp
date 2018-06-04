#include "base_peripheral_service.h"

#include "channel.h"

using namespace domain;

BasePeripheralService::BasePeripheralService(int channelCount, QObject* parent) : QObject(parent)
{
    for (int i = 0; i<channelCount; ++i)
    {
        dto::ChannelPtr ch(new dto::Channel);
        ch->setChannelId(i);
        channelList.append(ch);
    }
}

BasePeripheralService::~BasePeripheralService()
{
    channelList.clear();
}

dto::ChannelPtr BasePeripheralService::load(int chId)
{
    return channelList.value(chId, nullptr);
}

void BasePeripheralService::updateChannel(int chId, quint32 val)
{
    dto::ChannelPtr channel = this->load(chId);
    if (channel.isNull())
    {
        qFatal("error in %s: no such channel", Q_FUNC_INFO);
        return;
    }
    channel->setValue(val);
    channelList[chId] = channel;
    emit chUpdated(channel);
}

void BasePeripheralService::updateChannel(dto::ChannelPtr channel)
{
    if (!channelList.contains(channel))
    {
        qFatal("error in %s: no such channel", Q_FUNC_INFO);
        return;
    }
    channelList[channel->channelId()] = channel;
    emit chUpdated(channel);
}
