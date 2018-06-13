#ifndef GENERATOR_H
#define GENERATOR_H

#include <QObject>
#include "channel.h"

namespace domain
{
    class Generator : public QObject
    {
        Q_OBJECT
    public:
        Generator(QObject* parent = nullptr);
        void toggleOnOff(int chId, bool isOn, bool toMCU = true);
        void setValue(int chId, quint32 freq, bool toMCU = true);
        quint32 value(int chId) const;
        ~Generator() override;

    signals:
        void generatorUpdate(int chId, quint32 value, bool isOn, dto::Channel::ChannelStatus status);
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}
#endif // GENERATOR_H
