#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <QObject>
#include "dto_traits.h"

namespace domain
{
    class Measurements : public QObject
    {
        Q_OBJECT
    public:
        explicit Measurements(QObject *parent = nullptr);
        ~Measurements() override;
        qreal temperature();
        QList<qreal> phaseList();
        qreal distance();
    signals:
        void temperatureUpdated(qreal temperature);
        void adcResultsUpdated(dto::ResultsPtr results);
        void phaseUpdated(qreal phase);
        void distanceUpdated(qreal distance);
    private:
        class Impl;
        QScopedPointer<Impl> d;
    };
}
#endif // MEASUREMENTS_H
