#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

namespace settings
{
    namespace dac
    {
        const QString maxVal = "max_val";
        const QString vRef = "v_ref";
        const QString channelCount = "channel_count";
    }
    namespace freq_generator
    {
        const QString channelCount = "channel_count";
        const QString maxFrequency = "max_frequency";
    }
    namespace communication
    {
        namespace usb
        {
            const QString VID = "vid";
            const QString PID = "pid";
            const QString packetSize = "pack_size";
        }
    }
}

#endif // SETTINGS_H
