#ifndef FREQ_GENERATOR_SERVICE_H
#define FREQ_GENERATOR_SERVICE_H

#include "base_peripheral_service.h"

namespace domain
{
    class FreqGeneratorService : public BasePeripheralService
    {
        Q_OBJECT
    public:
        explicit FreqGeneratorService(QObject* parent = nullptr);
        ~FreqGeneratorService() override;

        void updateGenerator(int chId, quint32 freq, bool on, bool toMCU = true);
        void updateGenerator(dto::ChannelPtr generatorChannel, bool toMCU = true);

    private:
        class Impl;
        const QScopedPointer<Impl> d;

        Q_DISABLE_COPY(FreqGeneratorService)
    };
}
#endif // FREQ_GENERATOR_SERVICE_H
