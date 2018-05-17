#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

namespace settings
{
    namespace dac
    {
        const QString maxVal = "DAC/max_val";
        const QString vRef = "DAC/v_ref";
        const QString channelCount = "DAC/channel_count";
    }
    namespace freq_generator
    {
        const QString channelCount = "GEN/channel_count";
        const QString minFrequency = "GEN/min_frequency";
        const QString maxFrequency = "GEN/max_frequency";
    }
    namespace digital_potentiometer
    {
        const QString maxVal = "POT/max_val";
        const QString vRef = "POT/v_ref";
        const QString channelCount = "POT/channel_count";
    }
    namespace communication
    {
        namespace usb
        {
            const QString VID = "USB/vid";
            const QString PID = "USB/pid";
            const QString packetSize = "USB/pack_size";
        }
    }
}

#endif // SETTINGS_H
