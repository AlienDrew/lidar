#ifndef TRANSFER_SERVICE_H
#define TRANSFER_SERVICE_H

#include <QObject>
#include "dto_traits.h"

class QIODevice;

namespace domain
{
    class TransferService : public QObject
    {
        Q_OBJECT
    public:
        enum Command : uint8_t
        {
            dac = 1,
            gen,
            adc,
            startMeasure
        };
        TransferService();
        ~TransferService() override;

        bool openDevice();
        void closeDevice();
        bool deviceOpened() const;
    public slots:
        void transferCommand(Command cmd);
        void transferChannel(Command cmd, dto::ChannelPtr channel);
        void getAdcData(QIODevice* device);
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // TRANSFER_SERVICE_H
