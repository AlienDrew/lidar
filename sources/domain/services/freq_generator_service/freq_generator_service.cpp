#include "freq_generator_service.h"

#include "channel.h"
#include "settings_provider.h"

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

void FreqGeneratorService::updateGenerator(int chId, quint32 freq, bool on, bool toMCU)
{
    if (chId>=0 && chId<settingsProvider->value(settings::freq_generator::channelCount).toInt() &&
            freq<=settingsProvider->value(settings::freq_generator::maxFrequency).toUInt())
    {
        BasePeripheralService::updateChannel(chId, freq, on, toMCU);
    }
}


void FreqGeneratorService::updateGenerator(dto::ChannelPtr generatorChannel, bool toMCU)
{
    this->updateChannel(generatorChannel, toMCU);
}
