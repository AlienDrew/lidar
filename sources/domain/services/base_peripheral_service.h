#ifndef BASE_PERIPHERAL_SERVICE_H
#define BASE_PERIPHERAL_SERVICE_H

#include <QObject>
#include "dto_traits.h"

namespace domain
{
    class BasePeripheralService : public QObject
    {
        Q_OBJECT
    public:
        BasePeripheralService(int channelCount, QObject* parent = nullptr);
        dto::ChannelPtr load(int chId);
        ~BasePeripheralService() override;
    signals:
        void chUpdated(dto::ChannelPtr channel);

    protected:
        void updateChannel(int chId, quint32 val);
        void updateChannel(dto::ChannelPtr channel);

        dto::ChannelPtrList channelList;
    };
}

#endif // BASE_PERIPHERAL_SERVICE_H
