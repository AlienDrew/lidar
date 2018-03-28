#ifndef UNIT_CONVERSION_H
#define UNIT_CONVERSION_H

#include <QtGlobal>

#include "channel.h"

namespace utils
{
    class UnitConversion
    {
//        enum FrequencyUnits
//        {
//            Hz,
//            KHz,
//            MHz
//        };
    public:
        explicit UnitConversion();
        static qreal frequencyConvert(qreal freq, dto::Channel::Units from, dto::Channel::Units to);
    };
}

#endif // UNIT_CONVERSION_H
