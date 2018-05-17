#include "digital_potentiometer_service.h"

#include "channel.h"
#include "settings_provider.h"

using namespace domain;

DigitalPotentiometerService::DigitalPotentiometerService(QObject* parent) :
    BasePeripheralService(settingsProvider->value(settings::digital_potentiometer::channelCount).toInt(), parent)
{

}

void DigitalPotentiometerService::updatePotentiometer(int chId, int value)
{
    BasePeripheralService::updateChannel(chId, value);
}

void DigitalPotentiometerService::updatePotentiometer(dto::ChannelPtr channel)
{
    BasePeripheralService::updateChannel(channel);
}

DigitalPotentiometerService::~DigitalPotentiometerService()
{

}
