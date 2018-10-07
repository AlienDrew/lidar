#include "signal_analyser.h"

#include "unit_conversion.h"
#include "results.h"
#include "settings_provider.h"

#include <QtMath>
#include <QVector>


using namespace utils;

class SignalAnalyser::Impl
{
public:
    dto::Results results;

    qreal sinCoef;
    qreal cosCoef;
    qreal gCoef;
    int k_koef;
    qreal omega;
    int pointsCount;
};

SignalAnalyser::SignalAnalyser() : d(new Impl)
{
    goertzelInit();
}

SignalAnalyser::~SignalAnalyser()
{

}

void SignalAnalyser::goertzelInit()
{
    d->pointsCount = 750;
    d->k_koef = qRound(settingsProvider->value(settings::adc::signalFreq).toReal()*(qreal)d->pointsCount/settingsProvider->value(settings::adc::samplingFreq).toReal());
    d->omega = (2.0*M_PI*d->k_koef)/d->pointsCount;
    d->sinCoef = qSin(d->omega);
    d->cosCoef = qCos(d->omega);
    d->gCoef = 2.0*d->cosCoef;
}

dto::Results SignalAnalyser::goertzelAnalyse(const QVector<int>& ch)
{
    dto::Results results;
    qreal q0 = 0, q1 = 0, q2 = 0;
    qreal real;
    qreal imag;
    qreal scalingFactor = (qreal) d->pointsCount/2.0;
    for (int i = 0; i<d->pointsCount; ++i)
    {
        q0 = d->gCoef*q1-q2+(qreal)ch[i];
        q2 = q1;
        q1 = q0;
    }
    real = (q1-q2*d->cosCoef)/scalingFactor;
    imag = (q2*d->sinCoef)/scalingFactor;

    results.addPhase(utils::UnitConversion::complexToPhase(real, imag));
    results.addAmplitude(utils::UnitConversion::complexToAmplitude(real, imag));
    return results;
}
