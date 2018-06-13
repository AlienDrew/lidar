#ifndef LASER_H
#define LASER_H

#include <QObject>
#include "channel.h"

namespace domain
{
    class Laser : public QObject
    {
        Q_OBJECT
    public:
        explicit Laser(QObject *parent = nullptr);
        void setValue(qreal value, bool toMCU = true);
        qreal value() const;
        ~Laser() override;

    signals:
        void laserUpdate(qreal value, dto::Channel::ChannelStatus status);
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}
#endif // LASER_H
