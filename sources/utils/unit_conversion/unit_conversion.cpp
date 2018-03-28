#include "unit_conversion.h"

#include <QMap>

using namespace utils;

namespace frequency
{
    QMap<dto::Channel::Units, qreal> toHzMap =
    {
        {dto::Channel::Hz, 1},
        {dto::Channel::KHz, 1000},
        {dto::Channel::MHz, 1000000}
    };
    QMap<dto::Channel::Units, qreal> toKHzMap =
    {
        {dto::Channel::Hz, 0.001},
        {dto::Channel::KHz, 1},
        {dto::Channel::MHz, 1000}
    };

    QMap<dto::Channel::Units, qreal> toMHzMap =
    {
        {dto::Channel::Hz, 0.000001},
        {dto::Channel::KHz, 0.001},
        {dto::Channel::MHz, 1}
    };

    QMap<dto::Channel::Units, QMap<dto::Channel::Units, qreal> > conversionMap =
    {
        {dto::Channel::Hz, toHzMap},
        {dto::Channel::KHz, toKHzMap},
        {dto::Channel::MHz, toMHzMap}
    };
}

UnitConversion::UnitConversion()
{

}

qreal UnitConversion::frequencyConvert(qreal freq, dto::Channel::Units from, dto::Channel::Units to)
{
    return freq *= frequency::conversionMap.value(to).value(from);
}
