#ifndef RESULTS_H
#define RESULTS_H

#include <QList>
namespace dto
{
    class Results
    {
    public:
        void addPhase(qreal phase);
        QList<qreal> phaseList() const;
        void addAmplitude(qreal amplitude);
        QList<qreal> amplitudeList() const;
    private:
        QList<qreal> m_phaseList;
        QList<qreal> m_amplitudeList;
    };
}
#endif // RESULTS_H
