#ifndef COMMAND_H
#define COMMAND_H

#include <QVariant>

namespace dto
{
    class Command
    {
    public:
        enum CommandType : uint8_t
        {
            dac = 1,
            gen,
            adc,
            startMeasure,
            hv_ctrl,
            digital_pot,
            usb_connected,
            usb_disconnected
        };
        CommandType type() const;
        void setType(CommandType type);

        QVariantList arguments() const;
        bool hasArguments() const;
        void addArgument(const QVariant& argument);
    private:
        CommandType m_commandType;
        QVariantList m_commandArguments;
    };
}

#endif // COMMAND_H