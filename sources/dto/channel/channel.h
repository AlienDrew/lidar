#ifndef FREQ_GENERATOR_H
#define FREQ_GENERATOR_H

#include <QtGlobal>

namespace dto
{
    class Channel
    {
    public:
        enum FrequencyUnits
        {
            Hz,
            KHz,
            MHz
        };

        void setChannelId(int channelId);
        int channelId() const;
        void setValue(quint32 val);
        quint32 value() const;
        bool operator == (const Channel& channel);
    private:
        int m_channelId;
        quint32 m_value = 0;
    };
}

#endif // FREQ_GENERATOR_H
