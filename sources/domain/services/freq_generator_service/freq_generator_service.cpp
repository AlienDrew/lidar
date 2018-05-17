#include "freq_generator_service.h"

#include "channel.h"
#include "settings_provider.h"

#include <QDebug>

using namespace domain;

class FreqGeneratorService::Impl
{
public:
};

FreqGeneratorService::FreqGeneratorService(QObject* parent) :
    BasePeripheralService(settingsProvider->value(settings::freq_generator::channelCount).toInt(), parent),
    d(new Impl)
{
}

FreqGeneratorService::~FreqGeneratorService()
{

}

void FreqGeneratorService::updateGenerator(int chId, quint32 freq)
{
    if (chId<settingsProvider->value(settings::freq_generator::channelCount).toInt() && freq<=settingsProvider->value(settings::freq_generator::maxFrequency).toUInt())
        BasePeripheralService::updateChannel(chId, freq);
}


void FreqGeneratorService::updateGenerator(dto::ChannelPtr generatorChannel)
{
    if (generatorChannel->channelId()<settingsProvider->value(settings::freq_generator::channelCount).toInt() && generatorChannel->value()<=settingsProvider->value(settings::freq_generator::maxFrequency).toUInt())
        BasePeripheralService::updateChannel(generatorChannel);
}
