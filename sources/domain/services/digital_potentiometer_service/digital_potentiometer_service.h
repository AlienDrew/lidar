#ifndef DIGITAL_POTENTIOMETER_SERVICE_H
#define DIGITAL_POTENTIOMETER_SERVICE_H

#include "base_peripheral_service.h"

namespace domain
{
    class DigitalPotentiometerService : public BasePeripheralService
    {
        Q_OBJECT
    public:
        DigitalPotentiometerService(QObject* parent = nullptr);
        void updatePotentiometer(int chId, quint32 value, bool isOn, bool toMCU = true);
        void updatePotentiometer(dto::ChannelPtr channel, bool toMCU = true);
        ~DigitalPotentiometerService() override;

    private:
    };
}

#endif // DIGITAL_POTENTIOMETER_SERVICE_H
