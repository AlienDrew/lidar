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
        static qreal frequencyConvert(qreal freq, dto::Channel::FrequencyUnits from, dto::Channel::FrequencyUnits to);
        static int voltsToDAC(qreal volts);
        static int kToDAC(int k);
    };
}

#endif // UNIT_CONVERSION_H
