#include "freq_generator_service.h"

#include "channel.h"
#include "settings_provider.h"

#include <QDebug>

using namespace domain;

class FreqGeneratorService::Impl
{
public:
    dto::ChannelPtrList channelList;
};

FreqGeneratorService::FreqGeneratorService(QObject* parent) : QObject(parent),
    d(new Impl)
{
    int channelCount = settingsProvider->value(settings::FreqGeneratorSettings::channelCount).toInt();
    for (int i = 0; i<channelCount; ++i)
    {
        dto::ChannelPtr ch(new dto::Channel);
        ch->setChannelId(i);
        d->channelList.append(ch);
    }
}

FreqGeneratorService::~FreqGeneratorService()
{

}

void FreqGeneratorService::update(int chId, double freq)
{
    dto::ChannelPtr channel = d->channelList.at(chId);
    if (channel.isNull())
    {
        qFatal("error in %s: no such channel", Q_FUNC_INFO);
        return;
    }
    channel->setFreq(freq);
    d->channelList[chId] = channel;
    emit chUpdated(chId);
}

dto::ChannelPtr FreqGeneratorService::load(int chId)
{
    return d->channelList.at(chId);
}

void FreqGeneratorService::update(dto::ChannelPtr generatorChannel)
{
    if (d->channelList.contains(generatorChannel))
    {
        d->channelList[generatorChannel->channelId()] = generatorChannel;
        emit chUpdated(generatorChannel->channelId());
    }
    else
        qFatal("error in %s: no such channel", Q_FUNC_INFO);
}

//qreal FreqGeneratorService::convertFreq(qreal freq, dto::Channel::Units unit)
//{
//    //TODO: make frequency conversion
//}
