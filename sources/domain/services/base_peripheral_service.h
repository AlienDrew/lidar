#ifndef BASE_PERIPHERAL_SERVICE_H
#define BASE_PERIPHERAL_SERVICE_H

#include <QObject>
#include "dto_traits.h"
#include "channel.h"

namespace domain
{
    class BasePeripheralService : public QObject
    {
        Q_OBJECT
    public:
        BasePeripheralService(int channelCount, QObject* parent = nullptr);
        dto::ChannelPtr load(int chId);
        void rollbackChannel();
        ~BasePeripheralService() override;
    signals:
        void chUpdated(dto::ChannelPtr channel, bool toMCU = true);
        void chRollback();

    protected:
        void updateChannel(int chId, quint32 val, bool on, bool toMCU = true, dto::Channel::ChannelStatus status = dto::Channel::InProgress);
        void updateChannel(dto::ChannelPtr channel, bool toMCU = true);

        dto::ChannelPtrList channelList;

    private:
        dto::ChannelPtr loadDumpChannel() const;

        class Impl;
        QScopedPointer<Impl> d;
    };
}

#endif // BASE_PERIPHERAL_SERVICE_H
