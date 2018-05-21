#include "digital_potentiometer_service.h"

#include "channel.h"
#include "settings_provider.h"
#include <QDebug>

using namespace domain;

DigitalPotentiometerService::DigitalPotentiometerService(QObject* parent) :
    BasePeripheralService(settingsProvider->value(settings::digital_potentiometer::channelCount).toInt(), parent)
{

}

void DigitalPotentiometerService::updatePotentiometer(int chId, quint32 value)
{
    if (chId<settingsProvider->value(settings::digital_potentiometer::channelCount).toInt() &&
            value<=settingsProvider->value(settings::digital_potentiometer::maxVal).toUInt())
        BasePeripheralService::updateChannel(chId, value);
}

void DigitalPotentiometerService::updatePotentiometer(dto::ChannelPtr channel)
{
    if (channel->channelId()<settingsProvider->value(settings::digital_potentiometer::channelCount).toInt() &&
            channel->value()<=settingsProvider->value(settings::digital_potentiometer::maxVal).toUInt())
        BasePeripheralService::updateChannel(channel);
}

DigitalPotentiometerService::~DigitalPotentiometerService()
{

}
