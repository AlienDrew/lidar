#ifndef FREQ_GENERATOR_H
#define FREQ_GENERATOR_H

#include <QtGlobal>

namespace dto
{
    class Channel
    {
    public:
        enum Units
        {
            Hz,
            KHz,
            MHz
        };

        void setChannelId(int channelId);
        int channelId() const;
        void setFreq(qreal freq);
        qreal freq() const;
        bool operator == (const Channel& channel);
    private:
        int m_channelId;
        qreal m_freq = 0;
    };
}

#endif // FREQ_GENERATOR_H
