#ifndef TEMPERATURE_SERVICE_H
#define TEMPERATURE_SERVICE_H

#include "base_peripheral_service.h"

namespace domain
{
    class TemperatureService : public BasePeripheralService
    {
        Q_OBJECT
    public:
        TemperatureService(QObject* parent = nullptr);
        ~TemperatureService() override;
        void updateTemperature(int chId, quint32 temperature);
    signals:
        void temperatureUpdated(qreal temp);
        void temperatureAlert();
    private:
    };
}
#endif // TEMPERATURE_SERVICE_H
