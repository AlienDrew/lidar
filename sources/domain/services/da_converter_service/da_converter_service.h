#ifndef DA_CONVERTER_SERVICE_H
#define DA_CONVERTER_SERVICE_H

#include <QObject>
#include "dto_traits.h"

namespace domain
{
    class DAConverterService : public QObject
    {
        Q_OBJECT
    public:
        explicit DAConverterService(QObject *parent = nullptr);
        dto::ChannelPtr load(int chId);
        void updateDAC(int chId, int value);
        ~DAConverterService() override;

    signals:
        void chUpdated(dto::ChannelPtr channel);

    public slots:
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };

}

#endif // DA_CONVERTER_SERVICE_H
