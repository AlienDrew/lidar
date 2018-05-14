#include "unit_conversion.h"

#include "settings_provider.h"

#include <QMap>
#include <QtMath>

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

int UnitConversion::voltsToDAC(qreal volts)
{
    return qRound(volts*settingsProvider->value(settings::dac::maxVal).toInt()/settingsProvider->value(settings::dac::vRef).toReal());
}
