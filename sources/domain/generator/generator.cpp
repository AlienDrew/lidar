#include "generator.h"

#include "service_registry.h"
#include "freq_generator_service.h"
#include "transfer_service.h"
#include "command.h"
#include "settings_provider.h"

#include <QVariant>
#include <QDebug>

using namespace domain;

class Generator::Impl
{
public:
    FreqGeneratorService* freqGenService;
};

Generator::Generator(QObject* parent) : QObject(parent),
    d(new Impl)
{
    d->freqGenService = serviceRegistry->freqGeneratorService();

    connect(d->freqGenService, &domain::BasePeripheralService::chUpdated, this, [this](dto::ChannelPtr channel)
    {
        emit generatorUpdate(channel->channelId(), channel->value(), channel->isOn(), channel->status());
    });
}

void Generator::toggleOnOff(int chId, bool isOn, bool toMCU)
{
    d->freqGenService->updateGenerator(chId, d->freqGenService->load(chId)->value(), isOn, toMCU);
}

void Generator::setValue(int chId, quint32 freq, bool toMCU)
{
    if (freq >= settingsProvider->value(settings::freq_generator::minFrequency).toUInt()
            && freq <= settingsProvider->value(settings::freq_generator::maxFrequency).toUInt())
    {
        d->freqGenService->updateGenerator(chId, freq, d->freqGenService->load(chId)->isOn(), toMCU);
    }
    else
        qDebug()<<"frequency value out of range!";
}

quint32 Generator::value(int chId) const
{
    return d->freqGenService->load(chId)->value();
}

Generator::~Generator()
{

}
