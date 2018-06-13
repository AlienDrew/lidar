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

void Channel::setStatus(Channel::ChannelStatus status)
{
    m_status = status;
}

Channel::ChannelStatus Channel::status() const
{
    return m_status;
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

void Channel::setOn(bool val)
{
    m_on = val;
}

bool Channel::isOn() const
{
    return m_on;
}
