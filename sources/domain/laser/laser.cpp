#include "laser.h"

#include "service_registry.h"
#include "da_converter_service.h"
#include "transfer_service.h"
#include "command.h"
#include "unit_conversion.h"
#include "settings_provider.h"

#include <QDebug>

#define LASER_ID 1

using namespace domain;

class Laser::Impl
{
public:
    DAConverterService* daConverterService;
};

Laser::Laser(QObject *parent) : QObject(parent),
    d(new Impl)
{
    d->daConverterService = serviceRegistry->daConverterService();
    connect(d->daConverterService, &DAConverterService::chRollback, this, [this]()
    {
    });

    connect(d->daConverterService, &DAConverterService::chUpdated, this, [this](dto::ChannelPtr channel)
    {
        if (channel->channelId() == LASER_ID)
            emit laserUpdate(this->value(), channel->status());
    });
}

void Laser::setValue(qreal value, bool toMCU)
{
    //d = (256*2,667*Plaz)/3.3
    if (value>=0.0 && value<=settingsProvider->value(settings::dac::vRef).toReal())
    {
        d->daConverterService->updateDAC(LASER_ID, utils::UnitConversion::voltsToDAC(value), toMCU);
    }
    else
        qDebug()<<"laser value out of range!";
}

qreal Laser::value() const
{
    return utils::UnitConversion::DACToVolts(d->daConverterService->load(LASER_ID)->value());
}

Laser::~Laser()
{

}
