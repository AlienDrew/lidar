#include "measurements.h"

#include "results.h"
#include "service_registry.h"
#include "temperature_service.h"
#include "adc_service.h"
#include "signal_analyser.h"
#include "unit_conversion.h"

#include <QDebug>

#define PHASE_MULT 10

using namespace domain;

class Measurements::Impl
{
public:
    dto::Results refResult;
    dto::Results mesResult;
    utils::SignalAnalyser* signalAnalyser;
    qreal distance;
};

Measurements::Measurements(QObject *parent) : QObject(parent),
    d(new Impl)
{
    d->signalAnalyser = new utils::SignalAnalyser();
    connect(serviceRegistry->adcService(), &ADCService::adcDataUpdated, this, [=]()
    {
        d->refResult = d->signalAnalyser->goertzelAnalyse(serviceRegistry->adcService()->ch1Data());
        d->mesResult = d->signalAnalyser->goertzelAnalyse(serviceRegistry->adcService()->ch2Data());

        qreal tmpPhase = d->mesResult.phaseList().at(0)-d->refResult.phaseList().at(0);
        if (tmpPhase<0)
            tmpPhase = 360*PHASE_MULT+tmpPhase;
        tmpPhase = tmpPhase/PHASE_MULT;
        d->distance = utils::UnitConversion::phaseToDistance(tmpPhase);
        qDebug()<<tmpPhase;
        qDebug()<<d->distance;
        emit phaseUpdated(tmpPhase);
        emit distanceUpdated(this->distance());
    });
}

Measurements::~Measurements()
{
    delete d->signalAnalyser;
}

qreal Measurements::temperature()
{

}

QList<qreal> Measurements::phaseList()
{

}

qreal Measurements::distance()
{
    return d->distance;
}
