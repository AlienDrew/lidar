#include "channel.h"

using namespace dto;

void Channel::setValue(quint32 val)
{
    m_value = val;
}

quint32 Channel::value() const
{
    return m_value;
}

bool Channel::operator ==(const Channel& channel)
{
    if (this->channelId() == channel.channelId())
        return true;
    return false;
}

void Channel::setChannelId(int channelId)
{
    m_channelId = channelId;
}

int Channel::channelId() const
{
    return m_channelId;
}
