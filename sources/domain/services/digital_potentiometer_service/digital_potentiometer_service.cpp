#include "digital_potentiometer_service.h"

#include "channel.h"
#include "settings_provider.h"

using namespace domain;

DigitalPotentiometerService::DigitalPotentiometerService(QObject* parent) :
    BasePeripheralService(settingsProvider->value(settings::digital_potentiometer::channelCount).toInt(), parent)
{

}

void DigitalPotentiometerService::updatePotentiometer(int chId, quint32 value, bool isOn, bool toMCU)
{
    if (chId<settingsProvider->value(settings::digital_potentiometer::channelCount).toInt() &&
            value<=settingsProvider->value(settings::digital_potentiometer::maxVal).toUInt())
        BasePeripheralService::updateChannel(chId, value, isOn, toMCU);
}

void DigitalPotentiometerService::updatePotentiometer(dto::ChannelPtr channel, bool toMCU)
{
    this->updateChannel(channel, toMCU);
}

DigitalPotentiometerService::~DigitalPotentiometerService()
{

}
