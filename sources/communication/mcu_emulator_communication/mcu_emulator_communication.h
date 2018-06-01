#ifndef MCU_EMULATOR_COMMUNICATION_H
#define MCU_EMULATOR_COMMUNICATION_H

#include "communication_interface.h"

namespace communication
{
    class MCUEmulatorCommunication : public CommunicationInterface
    {
        Q_OBJECT
    public:
        MCUEmulatorCommunication();
        ~MCUEmulatorCommunication() override;
        bool open() override;
        bool isOpen() override;
        void close() override;
        void read() override;
        void cancelRead() override;
        bool write(QByteArray data) override;
    private:
        class Impl;
        QScopedPointer<Impl> d;

        Q_DISABLE_COPY(MCUEmulatorCommunication)
    };
}
#endif // MCU_EMULATOR_COMMUNICATION_H
