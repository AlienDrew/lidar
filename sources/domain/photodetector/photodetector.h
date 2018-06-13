#ifndef PHOTODETECTOR_H
#define PHOTODETECTOR_H

#include <QObject>
#include "channel.h"

namespace domain
{
    class Photodetector : public QObject
    {
        Q_OBJECT
    public:
        explicit Photodetector(QObject *parent = nullptr);
        void setBias(qreal value, bool toMCU = true);
        void setGain(int value, bool toMCU = true);
        qreal bias() const;
        int gain() const;
        void toggleDCDCOnOff(bool val, bool toMCU = true);
        ~Photodetector() override;

    signals:
        void biasUpdated(qreal bias, bool isOn, dto::Channel::ChannelStatus status);
        void gainUpdated(int gain, bool isOn, dto::Channel::ChannelStatus status);
        void dcdcSwitchUpdated(bool isOn, dto::Channel::ChannelStatus status);
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}
#endif // PHOTODETECTOR_H
