#include "service_registry.h"

#include <QDebug>

#include "presentation_manager.h"
#include "freq_generator_service.h"
#include "da_converter_service.h"
#include "dcdc_switch_service.h"
#include "digital_potentiometer_service.h"
#include "temperature_service.h"
#include "transfer_service.h"

#include "command.h"
#include "channel.h"

using namespace domain;

ServiceRegistry* ServiceRegistry::self = nullptr;

class ServiceRegistry::Impl
{
public:
    TransferService transferService;
    FreqGeneratorService freqGenService;
    DAConverterService daConverterService;
    DCDCSwitchService dcdcSwitchService;
    DigitalPotentiometerService digitalPotentiometerSerivce;
    TemperatureService temperatureSerive;
};

ServiceRegistry::ServiceRegistry() : d(new Impl)
{
    ServiceRegistry::self = this;

    connect(&d->daConverterService, &domain::BasePeripheralService::chUpdated, this, [this](dto::ChannelPtr channel, bool toMCU)
    {
        if (!toMCU)
            return;
        dto::Command cmd;
        cmd.setType(dto::Command::dac);
        cmd.addArgument(QVariant::fromValue(channel.data()));
        if (!d->transferService.transferCommand(cmd))
        {
            d->daConverterService.rollbackChannel();
        }
    });

    connect(&d->dcdcSwitchService, &domain::BasePeripheralService::chUpdated, this, [this](dto::ChannelPtr channel, bool toMCU)
    {
        if (!toMCU)
            return;
        dto::Command cmd;
        cmd.setType(dto::Command::dcdc_switch);
        cmd.addArgument(QVariant::fromValue(channel.data()));
        if (!d->transferService.transferCommand(cmd))
        {
            d->daConverterService.rollbackChannel();
        }
    });

    connect(&d->freqGenService, &domain::BasePeripheralService::chUpdated, this, [this](dto::ChannelPtr channel, bool toMCU)
    {
        if (!toMCU)
            return;
        dto::Command cmd;
        cmd.setType(dto::Command::gen);
        cmd.addArgument(QVariant::fromValue(channel.data()));
        if (!d->transferService.transferCommand(cmd))
        {
            d->freqGenService.rollbackChannel();
        }
    });
    connect(&d->digitalPotentiometerSerivce, &domain::BasePeripheralService::chUpdated, this, [this](dto::ChannelPtr channel, bool toMCU)
    {
        if (!toMCU)
            return;
        dto::Command cmd;
        cmd.setType(dto::Command::digital_pot);
        cmd.addArgument(QVariant::fromValue(channel.data()));
        if(!d->transferService.transferCommand(cmd))
        {
            d->digitalPotentiometerSerivce.rollbackChannel();
        }
    });
}

ServiceRegistry* ServiceRegistry::instance()
{
    return ServiceRegistry::self;
}

ServiceRegistry::~ServiceRegistry()
{
    qDebug()<<"service registry destroyed";
}

FreqGeneratorService* ServiceRegistry::freqGeneratorService()
{
    return &d->freqGenService;
}

DAConverterService* ServiceRegistry::daConverterService()
{
    return &d->daConverterService;
}

DCDCSwitchService*ServiceRegistry::dcdcSwitchService()
{
    return &d->dcdcSwitchService;
}

DigitalPotentiometerService*ServiceRegistry::digitalPotentiometerSerivce()
{
    return &d->digitalPotentiometerSerivce;
}

TemperatureService*ServiceRegistry::temperatureService()
{
    return &d->temperatureSerive;
}

TransferService* ServiceRegistry::transferService()
{
    return &d->transferService;
}
