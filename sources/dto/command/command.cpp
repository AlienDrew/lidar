#include "command.h"

using namespace dto;

Command::CommandType Command::type() const
{
    return m_commandType;
}

void Command::setType(CommandType type)
{
    m_commandType = type;
}

QVariantList Command::arguments() const
{
    return m_commandArguments;
}

bool Command::hasArguments() const
{
    return !m_commandArguments.isEmpty();
}

void Command::addArgument(const QVariant& argument)
{
    m_commandArguments.append(argument);
}

Command::CommandStatus Command::status() const
{
    return m_status;
}

void Command::setStatus(Command::CommandStatus status)
{
    m_status = status;
}
