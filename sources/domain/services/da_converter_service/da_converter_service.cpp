#include "da_converter_service.h"

#include "channel.h"
#include "settings_provider.h"

#include <QDebug>

using namespace domain;

class DAConverterService::Impl
{
};

DAConverterService::DAConverterService(QObject* parent) :
    BasePeripheralService(settingsProvider->value(settings::dac::channelCount).toInt(), parent),
    d(new Impl)
{

}

void DAConverterService::updateDAC(int chId, quint32 value)
{
    qDebug()<<value;
    if (chId<settingsProvider->value(settings::dac::channelCount).toInt() && value<=settingsProvider->value(settings::dac::maxVal).toUInt())
        BasePeripheralService::updateChannel(chId, value);
}

void DAConverterService::updateDAC(dto::ChannelPtr DACChannel)
{
    if (DACChannel->channelId()<settingsProvider->value(settings::dac::channelCount).toInt() && DACChannel->value()<=settingsProvider->value(settings::dac::maxVal).toUInt())
        BasePeripheralService::updateChannel(DACChannel->channelId(), DACChannel->value());
}

DAConverterService::~DAConverterService()
{

}
