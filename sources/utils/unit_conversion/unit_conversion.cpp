#include "unit_conversion.h"

#include "settings_provider.h"
#include "service_registry.h"

#include <QMap>
#include <QtMath>

#define PHASE_MULT 10
#define MAX_ANGLE 360

using namespace utils;

namespace frequency
{
    QMap<dto::Channel::FrequencyUnits, qreal> toHzMap =
    {
        {dto::Channel::Hz, 1},
        {dto::Channel::KHz, 1000},
        {dto::Channel::MHz, 1000000}
    };
    QMap<dto::Channel::FrequencyUnits, qreal> toKHzMap =
    {
        {dto::Channel::Hz, 0.001},
        {dto::Channel::KHz, 1},
        {dto::Channel::MHz, 1000}
    };

    QMap<dto::Channel::FrequencyUnits, qreal> toMHzMap =
    {
        {dto::Channel::Hz, 0.000001},
        {dto::Channel::KHz, 0.001},
        {dto::Channel::MHz, 1}
    };

    QMap<dto::Channel::FrequencyUnits, QMap<dto::Channel::FrequencyUnits, qreal> > conversionMap =
    {
        {dto::Channel::Hz, toHzMap},
        {dto::Channel::KHz, toKHzMap},
        {dto::Channel::MHz, toMHzMap}
    };
}

UnitConversion::UnitConversion()
{

}

qreal UnitConversion::frequencyConvert(qreal freq, dto::Channel::FrequencyUnits from, dto::Channel::FrequencyUnits to)
{
    return freq *= frequency::conversionMap.value(to).value(from);
}

quint32 UnitConversion::voltsToDAC(qreal volts)
{
    if (volts>=settingsProvider->value(settings::dac::vRef).toReal())
        return 255;
    return qRound(volts*settingsProvider->value(settings::dac::maxVal).toInt()/settingsProvider->value(settings::dac::vRef).toReal());
}

qreal UnitConversion::DACToVolts(quint32 dac)
{
    if (dac>settingsProvider->value(settings::dac::maxVal).toUInt())
        return 3.3;
    return settingsProvider->value(settings::dac::vRef).toReal()/settingsProvider->value(settings::dac::maxVal).toReal()*dac;
}

qreal UnitConversion::DACToBiasVolts(quint32 dac)
{
    //yeey magic numbers ^_^
    return ( -20.4115*DACToVolts(dac) )+110.924;
}

quint32 UnitConversion::BiasVoltsToDAC(qreal biasVolts)
{
    //yeey magic numbers ^_^
    return voltsToDAC((biasVolts-110.924)/(-20.4115));
}

quint32 UnitConversion::kToDAC(int k)
{
    return qRound(2.56*k-5.632);
}

int UnitConversion::DACToK(quint32 dac)
{
    return qRound((dac+5.632)/2.56);
}

qreal UnitConversion::complexToPhase(qreal real, qreal imag)
{
    qreal phase = qAtan2(imag, real);
    phase = phase*(qreal)(PHASE_MULT*MAX_ANGLE/2)/M_PI;
    return phase;
}

qreal UnitConversion::complexToAmplitude(qreal real, qreal imag)
{
    qreal ampitude = imag*imag+real*real;
    return qSqrt(ampitude);
}

qreal UnitConversion::phaseToDistance(qreal phase)
{
//    int c = 299792458;
//    int distMult = 32;
//    int wavelength = c/50060000*distMult;
//    qreal result;
//    if (phase<180*PHASE_MULT)
//     result = wavelength * 0 + ((int32_t)phase * wavelength) / (MAX_ANGLE * PHASE_MULT);
//    else
//        result = wavelength * -1 + ((int32_t)phase * wavelength) / (MAX_ANGLE * PHASE_MULT);
//    result = result/(distMult*2)+2;
    //interpolation
    qreal distance = (-8.31362*phase/PHASE_MULT)+3934.3406;

    return distance;
}
