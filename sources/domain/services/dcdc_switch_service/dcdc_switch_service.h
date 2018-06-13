#ifndef DCDC_SWITCH_SERVICE_H
#define DCDC_SWITCH_SERVICE_H

#include "base_peripheral_service.h"

namespace domain
{
    class DCDCSwitchService : public BasePeripheralService
    {
        Q_OBJECT
    public:
        DCDCSwitchService(QObject *parent = nullptr);
        void updateSwitch(bool isOn, bool toMCU = true);
        void updateSwitch(dto::ChannelPtr channel, bool toMCU = true);
        ~DCDCSwitchService() override;
    };
}

#endif // DCDC_SWITCH_SERVICE_H
