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
            digital_pot,
            dcdc_switch,
            usb_connected,
            usb_disconnected,

            temp_sensor_received,
            periph_status,
            adc_data
        };
        enum CommandStatus
        {
            Idle,
            InProgress,
            Rejected,
            Cancelled,
            Completed
        };

        CommandType type() const;
        void setType(CommandType type);

        QVariantList arguments() const;
        bool hasArguments() const;
        void addArgument(const QVariant& argument);

        CommandStatus status() const;
        void setStatus(CommandStatus status);
    private:
        CommandType m_commandType;
        QVariantList m_commandArguments;
        CommandStatus m_status;
    };
}

#endif // COMMAND_H
