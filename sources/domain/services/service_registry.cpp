#include "service_registry.h"

#include <QDebug>

#include "presentation_manager.h"
#include "freq_generator_service.h"

using namespace domain;

ServiceRegistry* ServiceRegistry::self = nullptr;

class ServiceRegistry::Impl
{
public:
    FreqGeneratorService freqGenService;
};

ServiceRegistry::ServiceRegistry() : d(new Impl)
{
    ServiceRegistry::self = this;
}

ServiceRegistry* ServiceRegistry::instance()
{
    return ServiceRegistry::self;
}

ServiceRegistry::~ServiceRegistry()
{
    qDebug()<<"service registry destroyed";
}

FreqGeneratorService*ServiceRegistry::freqGeneratorService()
{
    return &d->freqGenService;
}
