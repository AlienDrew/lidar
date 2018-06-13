#ifndef SERVICE_REGISTRY_H
#define SERVICE_REGISTRY_H

#include <QObject>

namespace domain
{
    class FreqGeneratorService;
    class DAConverterService;
    class DCDCSwitchService;
    class DigitalPotentiometerService;
    class TemperatureService;
    class TransferService;

    class ServiceRegistry : public QObject
    {
        Q_OBJECT
    public:
        ServiceRegistry();
        virtual ~ServiceRegistry();
        static ServiceRegistry* instance();
        FreqGeneratorService* freqGeneratorService();
        DAConverterService* daConverterService();
        DCDCSwitchService* dcdcSwitchService();
        DigitalPotentiometerService* digitalPotentiometerSerivce();
        TemperatureService* temperatureService();
        TransferService* transferService();
    private:
        static ServiceRegistry* self;

        Q_DISABLE_COPY(ServiceRegistry)

        class Impl;
        const QScopedPointer<Impl> d;

    };
}

#define serviceRegistry (domain::ServiceRegistry::instance())

#endif // SERVICE_REGISTRY_H
