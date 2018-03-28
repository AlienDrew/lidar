#ifndef FREQ_GENERATOR_SERVICE_H
#define FREQ_GENERATOR_SERVICE_H

#include <QObject>
#include "dto_traits.h"


namespace domain
{
    class FreqGeneratorService : public QObject
    {
        Q_OBJECT
    public:
        explicit FreqGeneratorService(QObject* parent = nullptr);
        ~FreqGeneratorService() override;

        dto::ChannelPtr load(int chId);
        void update(int chId, double freq);
        void update(dto::ChannelPtr generatorChannel);
        //qreal convertFreq(qreal freq, dto::Channel::Units unit);
    signals:
        void chUpdated(int chId);

    private:
        class Impl;
        const QScopedPointer<Impl> d;

        Q_DISABLE_COPY(FreqGeneratorService)
    };
}
#endif // FREQ_GENERATOR_SERVICE_H
