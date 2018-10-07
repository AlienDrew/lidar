#include "results.h"

using namespace dto;

void Results::addPhase(qreal phase)
{
    m_phaseList.append(phase);
}

QList<qreal> Results::phaseList() const
{
    return m_phaseList;
}

void Results::addAmplitude(qreal amplitude)
{
    m_amplitudeList.append(amplitude);
}

QList<qreal> Results::amplitudeList() const
{
    return m_amplitudeList;
}
