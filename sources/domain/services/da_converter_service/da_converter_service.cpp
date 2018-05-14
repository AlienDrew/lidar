#include "da_converter_service.h"

#include "channel.h"
#include "settings_provider.h"

using namespace domain;

class DAConverterService::Impl
{
public:
    dto::ChannelPtrList channelList;
};

DAConverterService::DAConverterService(QObject *parent) : QObject(parent),
    d(new Impl)
{
    int channelCount = settingsProvider->value(settings::dac::channelCount).toInt();
    for (int i = 0; i<channelCount; ++i)
    {
        dto::ChannelPtr ch(new dto::Channel);
        ch->setChannelId(i);
        d->channelList.append(ch);
    }
}

dto::ChannelPtr DAConverterService::load(int chId)
{
    return d->channelList.at(chId);
}

void DAConverterService::updateDAC(int chId, int value)
{
    dto::ChannelPtr channel = d->channelList.at(chId);
    if (channel.isNull())
    {
        qFatal("error in %s: no such channel", Q_FUNC_INFO);
        return;
    }
    channel->setValue(value);
    d->channelList[chId] = channel;
    emit chUpdated(channel);
}

DAConverterService::~DAConverterService()
{

}
