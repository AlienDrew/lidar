#include "da_converter_service.h"

#include "channel.h"
#include "settings_provider.h"

using namespace domain;

class DAConverterService::Impl
{
};

DAConverterService::DAConverterService(QObject* parent) :
    BasePeripheralService(settingsProvider->value(settings::dac::channelCount).toInt(), parent),
    d(new Impl)
{

}

void DAConverterService::updateDAC(int chId, int value)
{
    if (chId<settingsProvider->value(settings::dac::channelCount).toInt() && value<=settingsProvider->value(settings::dac::maxVal).toInt())
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
