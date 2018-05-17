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
        void updatePotentiometer(int chId, int value);
        void updatePotentiometer(dto::ChannelPtr channel);
        ~DigitalPotentiometerService() override;

    private:
    };
}

#endif // DIGITAL_POTENTIOMETER_SERVICE_H
