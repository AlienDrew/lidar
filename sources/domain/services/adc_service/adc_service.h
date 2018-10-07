#ifndef ADC_SERVICE_H
#define ADC_SERVICE_H

#include "base_peripheral_service.h"

namespace domain
{
    class ADCService : public BasePeripheralService
    {
        Q_OBJECT
    public:
        ADCService(QObject* parent = nullptr);
        void updateADCChannel(const QByteArray& data);
        const QVector<int> ch1Data() const;
        const QVector<int> ch2Data() const;
        ~ADCService();
    signals:
        void adcDataUpdated();
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // ADC_SERVICE_H
