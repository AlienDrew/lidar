#include "photodetector.h"

#include "service_registry.h"
#include "da_converter_service.h"
#include "dcdc_switch_service.h"
#include "digital_potentiometer_service.h"
#include "transfer_service.h"
#include "command.h"
#include "unit_conversion.h"
#include "settings_provider.h"

#include <QDebug>

#define BIAS_ID 0
#define GAIN_ID 0

using namespace domain;

class Photodetector::Impl
{
public:
    DAConverterService* daConverterService;
    DigitalPotentiometerService* digitalPotentiometerService;
};

Photodetector::Photodetector(QObject *parent) : QObject(parent),
    d(new Impl)
{
    d->daConverterService = serviceRegistry->daConverterService();
    d->digitalPotentiometerService = serviceRegistry->digitalPotentiometerSerivce();

    connect(d->daConverterService, &DAConverterService::chUpdated, this, [this](dto::ChannelPtr channel)
    {
        if (channel->channelId() == BIAS_ID)
            emit biasUpdated(this->bias(), channel->isOn(), channel->status());
    });
    connect(d->digitalPotentiometerService, &DigitalPotentiometerService::chUpdated, [this](dto::ChannelPtr channel)
    {
        if (channel->channelId() == GAIN_ID)
            emit gainUpdated(this->gain(), channel->isOn(), channel->status());
    });
    connect(serviceRegistry->dcdcSwitchService(), &DCDCSwitchService::chUpdated, [this] (dto::ChannelPtr channel)
    {
        emit dcdcSwitchUpdated(channel->isOn(), channel->status());
    });
}

void Photodetector::setBias(qreal value, bool toMCU)
{
    d->daConverterService->updateDAC(BIAS_ID, utils::UnitConversion::BiasVoltsToDAC(value),
                                     true, toMCU);
}

void Photodetector::setGain(int value, bool toMCU)
{
    if (value >= settingsProvider->value(settings::digital_potentiometer::minK).toInt()
            && value <= settingsProvider->value(settings::digital_potentiometer::maxK).toInt())
    {
        d->digitalPotentiometerService->updatePotentiometer(GAIN_ID, utils::UnitConversion::kToDAC(value),
                                                            true, toMCU);
    }
    else
        qDebug()<<"gain value is out of range!";
}

qreal Photodetector::bias() const
{
    return utils::UnitConversion::DACToBiasVolts(d->daConverterService->load(BIAS_ID)->value());
}

int Photodetector::gain() const
{
    return utils::UnitConversion::DACToK(d->digitalPotentiometerService->load(GAIN_ID)->value());
}

void Photodetector::toggleDCDCOnOff(bool val, bool toMCU)
{
    //d->daConverterService->updateDAC(BIAS_ID, d->daConverterService->load(BIAS_ID)->value(), val, toMCU);
    serviceRegistry->dcdcSwitchService()->updateSwitch(val, toMCU);
}

Photodetector::~Photodetector()
{

}
