#ifndef DA_CONVERTER_SERVICE_H
#define DA_CONVERTER_SERVICE_H

#include "base_peripheral_service.h"

namespace domain
{
    class DAConverterService : public BasePeripheralService
    {
        Q_OBJECT
    public:
        explicit DAConverterService(QObject *parent = nullptr);
        void updateDAC(int chId, quint32 value, bool isOn, bool toMCU = true);
        void updateDAC(dto::ChannelPtr DACChannel, bool toMCU = true);
        ~DAConverterService() override;

    private:
        class Impl;
        QScopedPointer<Impl> d;

        Q_DISABLE_COPY(DAConverterService)
    };

}

#endif // DA_CONVERTER_SERVICE_H
