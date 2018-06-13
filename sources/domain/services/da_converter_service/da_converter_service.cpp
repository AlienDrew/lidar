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

void DAConverterService::updateDAC(int chId, quint32 value, bool isOn, bool toMCU)
{
    if (chId<settingsProvider->value(settings::dac::channelCount).toInt() && value<=settingsProvider->value(settings::dac::maxVal).toUInt())
        BasePeripheralService::updateChannel(chId, value, isOn, toMCU);
}

void DAConverterService::updateDAC(dto::ChannelPtr DACChannel, bool toMCU)
{
    this->updateChannel(DACChannel, toMCU);
}

DAConverterService::~DAConverterService()
{

}
