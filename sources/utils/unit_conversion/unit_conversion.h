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
        static quint32 voltsToDAC(qreal volts);
        static qreal DACToVolts(quint32 dac);
        static qreal DACToBiasVolts(quint32 dac);
        static quint32 BiasVoltsToDAC(qreal biasVolts);
        static quint32 kToDAC(int k);
        static int DACToK(quint32 dac);
    };
}

#endif // UNIT_CONVERSION_H
