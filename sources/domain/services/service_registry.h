#ifndef SERVICE_REGISTRY_H
#define SERVICE_REGISTRY_H

#include <QScopedPointer>

namespace domain
{
    class FreqGeneratorService;
    class TransferService;
    class DAConverterService;

    class ServiceRegistry
    {
    public:
        ServiceRegistry();
        virtual ~ServiceRegistry();
        static ServiceRegistry* instance();
        FreqGeneratorService* freqGeneratorService();
        DAConverterService* daConverterService();
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
