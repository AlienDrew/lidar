#include "channel.h"

using namespace dto;

void Channel::setFreq(qreal freq)
{
    m_freq = freq;
}

qreal Channel::freq() const
{
    return m_freq;
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
