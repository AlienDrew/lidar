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
        TransferService();
        ~TransferService() override;

        bool openDevice();
        void closeDevice();
        bool deviceOpened() const;
    public slots:
        bool transferCommand(const dto::Command& command);
        bool transferChannel(const dto::Command& command, dto::ChannelPtr channel);
        void getAdcData(QIODevice* device);
        void listenData();
        void cancelListen();
    signals:
        void hotPlugDeviceLeft();
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // TRANSFER_SERVICE_H
