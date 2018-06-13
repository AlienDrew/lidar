#ifndef FREQ_GENERATOR_H
#define FREQ_GENERATOR_H

#include <QMetaType>

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

        enum ChannelStatus
        {
            Idle,
            InProgress,
            Rejected,
            Cancelled,
            Completed
        };

        void setChannelId(int channelId);
        int channelId() const;
        void setOn(bool val);
        bool isOn() const;
        void setValue(quint32 val);
        quint32 value() const;
        void setStatus(ChannelStatus status);
        ChannelStatus status() const;
        bool operator == (const Channel& channel);
    private:
        int m_channelId;
        bool m_on;
        quint32 m_value = 0;
        ChannelStatus m_status = dto::Channel::Idle;
    };
}
Q_DECLARE_METATYPE(dto::Channel*)

#endif // FREQ_GENERATOR_H
