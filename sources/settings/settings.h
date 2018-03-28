#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

namespace settings
{
    namespace DACoverterSettings
    {
        const QString bitDepth = "bit_depth";
    }
    namespace FreqGeneratorSettings
    {
        const QString channelCount = "channel_count";
        const QString maxFrequency = "max_frequency";
    }
}

#endif // SETTINGS_H
